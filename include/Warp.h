#include <nlohmann/json.hpp>
#include <string>


namespace rlx_teleport {

struct Warp {
    std::string name;
    float       x;
    float       y;
    float       z;
    int         d;
    std::string description;
};

void to_json(nlohmann::json& j, const Warp& w);
void from_json(const nlohmann::json& j, Warp& w);

} // namespace rlx_teleport