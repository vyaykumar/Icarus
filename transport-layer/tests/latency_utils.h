#ifndef ICARUS_LATENCY_UTILS_H
#define ICARUS_LATENCY_UTILS_H

#include <algorithm>
#include <vector>
#include <cstdint>
#include <numeric>
#include <cmath>

// Linear interpolation between two points (precise percentile)
inline uint64_t calculate_percentile_linear(const std::vector<uint64_t>& data, const double percentile) {
    if (data.empty())
        return 0;

    std::vector<uint64_t> sorted = data;
    std::sort(sorted.begin(), sorted.end());

    if (percentile == 0) return sorted.front();
    if (percentile == 100) return sorted.back();

    const double rank = (percentile / 100.0) * (sorted.size() - 1);
    const auto lower = static_cast<size_t>(std::floor(rank));
    const auto upper = static_cast<size_t>(std::ceil(rank));
    const double fraction = rank - lower;

    if (lower == upper)
        return sorted[lower];

    return static_cast<uint64_t>(
        sorted[lower] + fraction * (sorted[upper] - sorted[lower])
    );
}

#endif //ICARUS_LATENCY_UTILS_H