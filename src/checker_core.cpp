// checker_core.cpp
// Implementation of InStream methods and verdict helpers declared in checker_core.hpp.

#include "checker_core.hpp"

#include <cctype>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

namespace checker {

// ─── InStream ─────────────────────────────────────────────────────────────────

InStream::InStream(std::istream& is, std::string name)
    : stream_(&is), name_(std::move(name)) {}

InStream::InStream(const std::string& filename)
    : owned_file_(std::make_unique<std::ifstream>(filename))
    , stream_(owned_file_.get())
    , name_(filename) {
    if (!owned_file_->is_open())
        throw FailError("cannot open file: " + filename);
}

bool InStream::fetchToken(std::string& tok) {
    tok.clear();
    char c;
    // Skip leading whitespace.
    while (stream_->get(c)) {
        if (!std::isspace(static_cast<unsigned char>(c))) {
            tok += c;
            break;
        }
    }
    if (tok.empty()) return false; // pure EOF

    // Accumulate the rest of the token.
    while (stream_->get(c)) {
        if (std::isspace(static_cast<unsigned char>(c))) break;
        tok += c;
    }
    return true;
}

std::string InStream::readToken(const std::string& hint) {
    if (peeked_) {
        std::string t = std::move(*peeked_);
        peeked_.reset();
        return t;
    }
    std::string tok;
    if (!fetchToken(tok))
        throw PresentationError("expected " + hint + " but got end-of-file");
    return tok;
}

std::optional<std::string> InStream::peekToken() {
    if (!peeked_) {
        std::string tok;
        if (fetchToken(tok))
            peeked_ = std::move(tok);
    }
    return peeked_;
}

long long InStream::readInt(long long lo, long long hi, const std::string& name) {
    std::string tok = readToken(name);

    // Validate: optional leading sign, then one or more decimal digits.
    std::size_t i = 0;
    if (!tok.empty() && (tok[0] == '+' || tok[0] == '-')) ++i;
    if (i >= tok.size())
        throw PresentationError("expected " + name + " but got '" + tok + "'");
    for (; i < tok.size(); ++i) {
        if (!std::isdigit(static_cast<unsigned char>(tok[i])))
            throw PresentationError("expected " + name + " but got '" + tok + "'");
    }

    long long val;
    try {
        std::size_t pos = 0;
        val = std::stoll(tok, &pos);
        if (pos != tok.size())
            throw PresentationError("expected " + name + " but got '" + tok + "'");
    } catch (const std::out_of_range&) {
        throw PresentationError(name + " overflows 64-bit integer: '" + tok + "'");
    } catch (const std::invalid_argument&) {
        throw PresentationError("expected " + name + " but got '" + tok + "'");
    }

    if (val < lo || val > hi) {
        throw PresentationError(
            name + " value " + std::to_string(val)
            + " is outside allowed range ["
            + std::to_string(lo) + ", " + std::to_string(hi) + "]");
    }
    return val;
}

long long InStream::readInt(const std::string& name) {
    return readInt(std::numeric_limits<long long>::min(),
                   std::numeric_limits<long long>::max(),
                   name);
}

void InStream::expectEOF() {
    if (!isEOF()) {
        throw PresentationError(
            "extra tokens found after expected end of output (first extra token: '"
            + *peekToken() + "')");
    }
}

bool InStream::isEOF() {
    return !peekToken().has_value();
}

// ─── Verdict helpers ──────────────────────────────────────────────────────────

[[noreturn]] void quitOK(const std::string& msg) {
    std::cout << "OK " << msg << "\n";
    std::exit(0);
}

[[noreturn]] void quitWA(const std::string& msg) {
    std::cout << "WA " << msg << "\n";
    std::exit(1);
}

[[noreturn]] void quitPE(const std::string& msg) {
    std::cout << "PE " << msg << "\n";
    std::exit(2);
}

[[noreturn]] void quitFAIL(const std::string& msg) {
    std::cout << "FAIL " << msg << "\n";
    std::exit(3);
}

// ─── runChecker ───────────────────────────────────────────────────────────────

void runChecker(int argc, char** argv,
                const std::function<void(InStream&, InStream&)>& fn) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0]
                  << " <input_file> <output_file>\n";
        std::exit(3);
    }
    try {
        InStream inf(argv[1]);
        InStream ouf(argv[2]);
        fn(inf, ouf);
        quitOK(); // checker_fn returned normally
    } catch (const PresentationError& e) {
        quitPE(e.what());
    } catch (const WrongAnswerError& e) {
        quitWA(e.what());
    } catch (const FailError& e) {
        quitFAIL(e.what());
    } catch (const std::exception& e) {
        quitFAIL(std::string("unexpected exception: ") + e.what());
    }
}

} // namespace checker
