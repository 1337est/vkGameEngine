#pragma once

// std
#include <functional> // for hash function

namespace vge
{
// from: https://stackoverflow.com/a/57595105
template <typename T, typename... Rest>
void hashCombine(std::size_t& seed, const T& v, const Rest&... rest)
{
    seed ^= std::hash<T>{}(v) + 0x9e'37'79'b9 + (seed << 6) + (seed >> 2);
    // C++17 fold expression (f(), ...) to hash remaining element in rest...
    (hashCombine(seed, rest), ...);
};
} // namespace vge
