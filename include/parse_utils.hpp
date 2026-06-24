// parse_utils.hpp
// Higher-level parsing helpers built on top of InStream.
// These handle common patterns: reading vectors, verifying permutations,
// reading distinct integer sets, etc.

#pragma once

#include "checker_core.hpp"
#include <string>
#include <unordered_set>
#include <vector>

namespace checker {

// ─── Vector readers ───────────────────────────────────────────────────────────

// Read exactly n integers, each in [lo, hi].
inline std::vector<long long>
readIntVector(InStream& s, int n, long long lo, long long hi,
              const std::string& elem_name = "element") {
    std::vector<long long> v;
    v.reserve(static_cast<std::size_t>(n));
    for (int i = 0; i < n; ++i)
        v.push_back(s.readInt(lo, hi, elem_name + "[" + std::to_string(i + 1) + "]"));
    return v;
}

// ─── Permutation reader ───────────────────────────────────────────────────────

// Read n integers that must form a permutation of {1, …, n}.
// Throws PresentationError if any value is out of [1,n] or there are duplicates.
inline std::vector<int> readPermutation(InStream& s, int n) {
    std::vector<bool> seen(static_cast<std::size_t>(n + 1), false);
    std::vector<int>  result;
    result.reserve(static_cast<std::size_t>(n));

    for (int i = 0; i < n; ++i) {
        long long raw = s.readInt(1, n, "permutation[" + std::to_string(i + 1) + "]");
        int x = static_cast<int>(raw);
        if (seen[static_cast<std::size_t>(x)])
            throw PresentationError("permutation contains duplicate value "
                                    + std::to_string(x));
        seen[static_cast<std::size_t>(x)] = true;
        result.push_back(x);
    }
    return result;
}

// ─── Distinct set reader ──────────────────────────────────────────────────────

// Read k distinct integers in [1, n].
// Throws PresentationError on out-of-range or duplicate values.
inline std::vector<int>
readDistinctSubset(InStream& s, int k, int n,
                   const std::string& elem_name = "vertex") {
    std::unordered_set<int> seen;
    std::vector<int>        result;
    result.reserve(static_cast<std::size_t>(k));

    for (int i = 0; i < k; ++i) {
        long long raw = s.readInt(1, n, elem_name + "[" + std::to_string(i + 1) + "]");
        int x = static_cast<int>(raw);
        if (!seen.insert(x).second)
            throw PresentationError(elem_name + " contains duplicate value "
                                    + std::to_string(x));
        result.push_back(x);
    }
    return result;
}

} // namespace checker
