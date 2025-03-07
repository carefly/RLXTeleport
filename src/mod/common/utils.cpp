#include "common/utils.h"

namespace rlx_teleport {

Vec3 Utils::fixPos(const Vec3& pos) { return Vec3{pos.x, float(int(pos.y - 1)), pos.z}; }

} // namespace rlx_teleport
