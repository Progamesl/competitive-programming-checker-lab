#!/usr/bin/env python3
"""
run_all_tests.py
================
Discover and run all checker test cases for every problem, then report results.

Test-file naming convention (inside each problem's tests/ directory):
  <name>.in                  - problem input
  <name>_<VERDICT>[_*].out   - contestant output; expected verdict: OK, WA, or PE

where VERDICT is literally "OK", "WA", or "PE".

Example:
  01_two_paths.in
  01_two_paths_OK_a.out   -> expect checker exit code 0
  01_two_paths_OK_b.out   -> expect checker exit code 0
  01_two_paths_WA.out     -> expect checker exit code 1
  01_two_paths_PE.out     -> expect checker exit code 2

Exit codes used by checkers:
  0 = OK, 1 = WA, 2 = PE, 3 = FAIL (judge error)

Usage:
  python3 scripts/run_all_tests.py --build-dir build
"""

import argparse
import os
import re
import subprocess
import sys
from pathlib import Path
from typing import Optional

# ANSI colours
GREEN  = "\033[32m"
RED    = "\033[31m"
YELLOW = "\033[33m"
RESET  = "\033[0m"

VERDICT_EXIT_CODE = {"OK": 0, "WA": 1, "PE": 2}

# Map from problem directory name to checker binary name suffix (must match CMakeLists).
PROBLEM_CHECKER = {
    "shortest_path_certificate": "checker_sp_cert",
    "graph_coloring":             "checker_graph_col",
    "topological_order":          "checker_topo_order",
    "tree_construction":          "checker_tree_const",
    "min_cut_certificate":        "checker_min_cut",
}


def find_binary(build_dir: Path, name: str) -> Optional[Path]:
    """Search for a compiled binary (with or without .exe extension)."""
    for root, _dirs, files in os.walk(build_dir):
        for fname in files:
            stem = Path(fname).stem
            if stem == name:
                return Path(root) / fname
    return None


def run_checker(checker: Path, input_file: Path, output_file: Path) -> int:
    """Run the checker and return its exit code."""
    try:
        result = subprocess.run(
            [str(checker), str(input_file), str(output_file)],
            capture_output=True,
            timeout=10,
        )
        return result.returncode
    except subprocess.TimeoutExpired:
        return -1  # treat timeout as failure


def discover_tests(tests_dir: Path):
    """
    Yield (input_path, output_path, expected_exit_code) tuples.
    Pairs each .out file with its matching .in file based on the base name.
    """
    if not tests_dir.is_dir():
        return

    out_pattern = re.compile(r"^(.+?)_(OK|WA|PE)(?:_.+)?\.out$")

    for out_file in sorted(tests_dir.glob("*.out")):
        m = out_pattern.match(out_file.name)
        if not m:
            continue
        base_name, verdict_str = m.group(1), m.group(2)
        in_file = tests_dir / f"{base_name}.in"
        if not in_file.exists():
            print(f"  {YELLOW}SKIP{RESET} {out_file.name}: no matching .in file")
            continue
        yield in_file, out_file, VERDICT_EXIT_CODE[verdict_str]


def main():
    parser = argparse.ArgumentParser(description="Run all checker test cases.")
    parser.add_argument(
        "--build-dir",
        default="build",
        help="CMake build output directory (default: build)",
    )
    parser.add_argument(
        "--problems-dir",
        default="problems",
        help="Root problems directory (default: problems)",
    )
    args = parser.parse_args()

    build_dir   = Path(args.build_dir).resolve()
    problems_dir = Path(args.problems_dir).resolve()

    if not build_dir.is_dir():
        print(f"{RED}ERROR{RESET}: build directory '{build_dir}' not found.")
        print("  Run `cmake -B build && cmake --build build` first.")
        sys.exit(1)

    total   = 0
    passed  = 0
    failed  = 0
    skipped = 0

    for prob_name, checker_name in sorted(PROBLEM_CHECKER.items()):
        prob_dir  = problems_dir / prob_name
        tests_dir = prob_dir / "tests"

        print(f"\n{'-'*60}")
        print(f"  Problem: {prob_name}")

        checker_bin = find_binary(build_dir, checker_name)
        if checker_bin is None:
            print(f"  {YELLOW}SKIP{RESET}: checker binary '{checker_name}' not found in {build_dir}")
            skipped += 1
            continue

        print(f"  Checker: {checker_bin}")

        prob_total  = 0
        prob_passed = 0

        for in_file, out_file, expected_code in discover_tests(tests_dir):
            got_code = run_checker(checker_bin, in_file, out_file)
            exp_name = {0:"OK", 1:"WA", 2:"PE"}.get(expected_code, "?")
            got_name = {0:"OK", 1:"WA", 2:"PE", 3:"FAIL"}.get(got_code, f"code{got_code}")

            label = f"  {out_file.name:<45} expected={exp_name:<3} got={got_name}"
            if got_code == expected_code:
                print(f"{GREEN}PASS{RESET} {label}")
                passed      += 1
                prob_passed += 1
            else:
                print(f"{RED}FAIL{RESET} {label}")
                failed += 1

            total      += 1
            prob_total += 1

        if prob_total == 0:
            print(f"  {YELLOW}(no test files found){RESET}")

    print(f"\n{'='*60}")
    print(f"  Results: {GREEN}{passed}{RESET} passed, "
          f"{RED}{failed}{RESET} failed, "
          f"{YELLOW}{skipped}{RESET} skipped  "
          f"(total {total} tests)")
    print(f"{'='*60}")

    if failed > 0:
        sys.exit(1)


if __name__ == "__main__":
    main()
