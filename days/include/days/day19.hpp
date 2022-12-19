#include "days/day.hpp"
#include "owning_span.hpp"

namespace day19 {

enum class resource : u8 { ore = 0, clay = 1, obsidian = 2, geode = 3, none = 0xFF };

// a robot consists of 3 costs
using robot = std::array<u8, 3>;

// a blueprint consists of 4 robots
using blueprint = std::array<robot, 4>;

// "dynamic" array of blueprints
using blueprints = owning_span<blueprint, 30>;
} // namespace day19

using Day19 = Day<19, day19::blueprints, u32>;
