#include "common/utils.h"
#include <algorithm>
#include <cctype>

namespace rlx_teleport {

Vec3 Utils::fixPos(const Vec3& pos) { return Vec3{pos.x, float(int(pos.y - 1)), pos.z}; }

bool Utils::equalsIgnoreCase(const std::string& str1, const std::string& str2) {
    if (str1.length() != str2.length()) {
        return false;
    }
    return std::equal(str1.begin(), str1.end(), str2.begin(), [](char a, char b) {
        return std::tolower(static_cast<unsigned char>(a)) == std::tolower(static_cast<unsigned char>(b));
    });
}

} // namespace rlx_teleport
