#pragma once

#include <mc/deps/core/math/Vec3.h>
#include <string>

namespace rlx_teleport {

class Utils {
public:
    static Vec3 fixPos(const Vec3& pos);
    static bool equalsIgnoreCase(const std::string& str1, const std::string& str2);
};

} // namespace rlx_teleport