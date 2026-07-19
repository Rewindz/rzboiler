#pragma once
#include <array>
#include <span>
#include <algorithm>

constexpr std::array cxx_std = {98, 3, 11, 14, 17, 20, 23, 26};
constexpr std::array c_std = {89, 99, 11, 17, 18, 23};

inline bool check_std(std::span<const int> stds, int in)
{
    auto it = std::ranges::find_if(stds, [in](const int std){return std == in;});
    return it != stds.end();
}
