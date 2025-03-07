#pragma once

#include <mc/deps/core/math/Vec3.h>

namespace rlx_teleport {

class Utils {
public:
    static Vec3 fixPos(const Vec3& pos);
};

} // namespace rlx_teleport