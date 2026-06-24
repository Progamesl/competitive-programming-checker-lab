// checker_core.hpp
// Core types, InStream, and verdict helpers for the checker framework.
//
// Design goals:
//   - Never crash on malformed contestant output; always throw PresentationError.
//   - Distinguish format errors (PE) from semantic errors (WA) from judge bugs (FAIL).
//   - Token-based reading so whitespace layout never matters.
//   - Single-token lookahead for conditional parsing (e.g., "NO PATH" vs. an integer).

#pragma once

#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

namespace checker {

// ─── Verdict ──────────────────────────────────────────────────────────────────
// Exit-code convention (mirrors Codeforces/Polygon checker protocol):
//   0 = OK    – answer is accepted
//   1 = WA    – answer is well-formed but semantically wrong
//   2 = PE    – answer cannot be parsed (bad format / missing tokens / extra tokens)
//   3 = FAIL  – checker internal error or inconsistent judge data

enum class Verdict { OK = 0, WA = 1, PE = 2, FAIL = 3 };

// ─── Exception types ──────────────────────────────────────────────────────────

// Contestant output is malformed (unparseable, missing values, extra tokens, …).
struct PresentationError : std::runtime_error {
    explicit PresentationError(std::string msg) : std::runtime_error(std::move(msg)) {}
};

// Contestant output is well-formed but logically wrong.
struct WrongAnswerError : std::runtime_error {
    explicit WrongAnswerError(std::string msg) : std::runtime_error(std::move(msg)) {}
};

// Judge-side assertion failure or bad test data – never the contestant's fault.
struct FailError : std::runtime_error {
    explicit FailError(std::string msg) : std::runtime_error(std::move(msg)) {}
};

// ─── InStream ─────────────────────────────────────────────────────────────────
// Whitespace-agnostic, token-based stream reader.
// Every read method throws PresentationError rather than invoking UB or crashing.
class InStream {
public:
    // Attach to an existing stream (does not take ownership).
    explicit InStream(std::istream& is, std::string name = "<stream>");

    // Open a named file; throws FailError if the file cannot be opened.
    explicit InStream(const std::string& filename);

    InStream(const InStream&)            = delete;
    InStream& operator=(const InStream&) = delete;
    InStream(InStream&&)                 = default;

    // Read the next whitespace-delimited token.
    // hint is used in the error message when the stream is at EOF.
    std::string readToken(const std::string& hint = "token");

    // Peek at the next token without consuming it; returns nullopt at EOF.
    std::optional<std::string> peekToken();

    // Read an integer in [lo, hi].  Throws PE on bad format or out-of-range value.
    long long readInt(long long lo, long long hi, const std::string& name = "integer");

    // Read an integer with no range constraint.
    long long readInt(const std::string& name = "integer");

    // Assert that no more tokens remain; throws PE if extra tokens are present.
    void expectEOF();

    // Returns true iff the stream has no remaining non-whitespace tokens.
    bool isEOF();

    const std::string& name() const { return name_; }

private:
    std::unique_ptr<std::ifstream> owned_file_;
    std::istream*                  stream_;
    std::string                    name_;
    std::optional<std::string>     peeked_;

    bool fetchToken(std::string& tok);
};

// ─── Verdict helpers ──────────────────────────────────────────────────────────
// Print "VERDICT message\n" to stdout and call std::exit with the matching code.

[[noreturn]] void quitOK  (const std::string& msg = "correct");
[[noreturn]] void quitWA  (const std::string& msg);
[[noreturn]] void quitPE  (const std::string& msg);
[[noreturn]] void quitFAIL(const std::string& msg);

// ─── runChecker ───────────────────────────────────────────────────────────────
// Wraps checker_fn(inf, ouf) with a top-level exception handler that converts
// exceptions to the appropriate exit code.  If checker_fn returns normally,
// quitOK() is called automatically.
//
// Expected CLI:  <checker> <input_file> <output_file>
void runChecker(int argc, char** argv,
                const std::function<void(InStream& inf, InStream& ouf)>& fn);

} // namespace checker
