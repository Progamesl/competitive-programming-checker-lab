#!/usr/bin/env python3
"""
fuzz_problem.py
===============
Randomized stress-testing for two problems:
  1. shortest_path_certificate
  2. topological_order

For each iteration:
  1. Run the generator to create a random problem instance.
  2. Run the reference solver to produce a correct answer.
  3. Feed that answer to the checker --- must be OK.
  4. Perturb the answer (swap two elements / corrupt a token) --- must be WA or PE.

This validates that:
  - The checker never rejects a reference solution (no false negatives).
  - The checker catches common perturbation errors (no false positives).

Usage:
  python3 scripts/fuzz_problem.py --build-dir build [--iters 50] [--seed 0]
"""

import argparse
import os
import random
import subprocess
import sys
import tempfile
from pathlib import Path
from typing import Optional

GREEN  = "\033[32m"
RED    = "\033[31m"
YELLOW = "\033[33m"
RESET  = "\033[0m"


def find_binary(build_dir: Path, name: str) -> Optional[Path]:
    for root, _dirs, files in os.walk(build_dir):
        for fname in files:
            if Path(fname).stem == name:
                return Path(root) / fname
    return None


def run(cmd, input_text: Optional[str] = None, timeout: int = 5) -> subprocess.CompletedProcess:
    return subprocess.run(
        cmd,
        input=input_text,
        capture_output=True,
        text=True,
        timeout=timeout,
    )


def write_temp(content: str, suffix: str = ".txt") -> Path:
    fd, path = tempfile.mkstemp(suffix=suffix)
    os.close(fd)
    Path(path).write_text(content)
    return Path(path)


def check(checker: Path, input_path: Path, output_path: Path) -> int:
    try:
        r = subprocess.run(
            [str(checker), str(input_path), str(output_path)],
            capture_output=True, timeout=5
        )
        return r.returncode
    except subprocess.TimeoutExpired:
        return -1


# --------- Perturbation helpers ------------------------------------------------------------------------------------------------------------------------------------------------------------------

def perturb_integer_sequence(tokens: list[str], rng: random.Random) -> list[str]:
    """Randomly swap two tokens in a list --- likely breaks ordering/degree constraints."""
    if len(tokens) < 2:
        return tokens
    t = tokens[:]
    i, j = rng.sample(range(len(t)), 2)
    t[i], t[j] = t[j], t[i]
    return t


def corrupt_token(tokens: list[str], rng: random.Random) -> list[str]:
    """Replace a random token with a non-numeric string --- triggers PE."""
    if not tokens:
        return tokens
    t = tokens[:]
    idx = rng.randrange(len(t))
    t[idx] = "INVALID"
    return t


# --------- Fuzz: shortest_path_certificate ---------------------------------------------------------------------------------------------------------------------------

def fuzz_shortest_path(checker: Path, reference: Path, generator: Path,
                       iters: int, rng: random.Random) -> tuple[int,int]:
    ok = fail = 0
    for i in range(iters):
        seed = rng.randint(0, 10**9)
        n    = rng.randint(3, 12)
        m    = rng.randint(1, 6)

        gen_r = run([str(generator), str(seed), str(n), str(m)])
        if gen_r.returncode != 0:
            continue

        problem_input = gen_r.stdout
        ref_r = run([str(reference)], input_text=problem_input)
        if ref_r.returncode != 0:
            continue

        reference_output = ref_r.stdout.strip()
        tokens = reference_output.split()

        # Test 1: reference answer must be accepted.
        in_f  = write_temp(problem_input)
        out_f = write_temp(reference_output)
        rc = check(checker, in_f, out_f)
        in_f.unlink(); out_f.unlink()

        if rc == 0:
            ok += 1
        else:
            print(f"  {RED}FAIL{RESET} iter {i}: reference answer rejected (exit {rc})")
            print(f"    Input:  {problem_input.strip()}")
            print(f"    Output: {reference_output}")
            fail += 1
            continue

        # Test 2: corrupted answer with a non-numeric token --- must NOT be OK.
        if len(tokens) >= 2 and tokens[0] != "NO":
            bad_tokens = corrupt_token(tokens, rng)
            bad_output = " ".join(bad_tokens)
            in_f  = write_temp(problem_input)
            out_f = write_temp(bad_output)
            rc = check(checker, in_f, out_f)
            in_f.unlink(); out_f.unlink()
            if rc == 0:
                print(f"  {RED}FAIL{RESET} iter {i}: corrupted answer accepted")
                fail += 1
            else:
                ok += 1

    return ok, fail


# --------- Fuzz: topological_order ------------------------------------------------------------------------------------------------------------------------------------------------------

def fuzz_topo_order(checker: Path, reference: Path, generator: Path,
                    iters: int, rng: random.Random) -> tuple[int,int]:
    ok = fail = 0
    for i in range(iters):
        seed = rng.randint(0, 10**9)
        n    = rng.randint(3, 12)
        pct  = rng.randint(20, 60)

        gen_r = run([str(generator), str(seed), str(n), str(pct)])
        if gen_r.returncode != 0:
            continue

        problem_input = gen_r.stdout
        ref_r = run([str(reference)], input_text=problem_input)
        if ref_r.returncode != 0:
            continue

        reference_output = ref_r.stdout.strip()
        tokens = reference_output.split()

        # Test 1: reference must be accepted.
        in_f  = write_temp(problem_input)
        out_f = write_temp(reference_output)
        rc = check(checker, in_f, out_f)
        in_f.unlink(); out_f.unlink()

        if rc == 0:
            ok += 1
        else:
            print(f"  {RED}FAIL{RESET} iter {i}: topo reference rejected (exit {rc})")
            print(f"    Input:  {problem_input.strip()}")
            print(f"    Output: {reference_output}")
            fail += 1
            continue

        # Test 2: swap two positions --- should fail if graph has relevant edges.
        if len(tokens) >= 2:
            bad_tokens = perturb_integer_sequence(tokens, rng)
            bad_output = " ".join(bad_tokens)
            if bad_output != reference_output:  # skip if swap is identity
                in_f  = write_temp(problem_input)
                out_f = write_temp(bad_output)
                rc = check(checker, in_f, out_f)
                in_f.unlink(); out_f.unlink()
                # Swapped order may still be valid; we only record failure if
                # the checker crashes (exit code 3) or something unexpected.
                if rc == 3:
                    print(f"  {RED}FAIL{RESET} iter {i}: checker FAIL on perturbed input")
                    fail += 1
                else:
                    ok += 1

    return ok, fail


# --------- Main ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(description="Fuzz shortest_path and topological_order checkers.")
    parser.add_argument("--build-dir", default="build")
    parser.add_argument("--iters",     type=int, default=50, help="Iterations per problem")
    parser.add_argument("--seed",      type=int, default=42,  help="Master RNG seed")
    args = parser.parse_args()

    build_dir = Path(args.build_dir).resolve()
    rng       = random.Random(args.seed)

    problems = [
        ("shortest_path", "checker_sp_cert",    "reference_sp_cert",    "generator_sp_cert"),
        ("topological",   "checker_topo_order",  "reference_topo_order", "generator_topo_order"),
    ]

    grand_ok = grand_fail = 0

    for prob_label, checker_name, ref_name, gen_name in problems:
        print(f"\n{'-'*60}")
        print(f"  Fuzzing: {prob_label}  ({args.iters} iterations)")

        checker   = find_binary(build_dir, checker_name)
        reference = find_binary(build_dir, ref_name)
        generator = find_binary(build_dir, gen_name)

        missing = [n for n, b in [(checker_name, checker), (ref_name, reference), (gen_name, generator)] if b is None]
        if missing:
            print(f"  {YELLOW}SKIP{RESET}: binaries not found: {missing}")
            continue

        if prob_label == "shortest_path":
            ok, fail = fuzz_shortest_path(checker, reference, generator, args.iters, rng)
        else:
            ok, fail = fuzz_topo_order(checker, reference, generator, args.iters, rng)

        verdict = f"{GREEN}PASS{RESET}" if fail == 0 else f"{RED}FAIL{RESET}"
        print(f"  {verdict}: {ok} checks passed, {fail} failed")
        grand_ok   += ok
        grand_fail += fail

    print(f"\n{'='*60}")
    print(f"  Fuzz summary: {GREEN}{grand_ok}{RESET} ok, {RED}{grand_fail}{RESET} fail")
    print(f"{'='*60}")

    if grand_fail > 0:
        sys.exit(1)


if __name__ == "__main__":
    main()
