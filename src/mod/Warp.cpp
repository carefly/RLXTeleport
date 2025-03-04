#include "Warp.h"
#include <nlohmann/json.hpp>


namespace rlx_teleport {

void to_json(nlohmann::json& j, const Warp& w) {
    j = nlohmann::json{
        {"name",        w.name       },
        {"x",           w.x          },
        {"y",           w.y          },
        {"z",           w.z          },
        {"d",           w.d          },
        {"description", w.description}
    };
}

void from_json(const nlohmann::json& j, Warp& w) {
    w.name        = j["name"].get<std::string>();
    w.x           = j["x"].get<float>();
    w.y           = j["y"].get<float>();
    w.z           = j["z"].get<float>();
    w.d           = j["d"].get<int>();
    w.description = j["description"].get<std::string>();
}

} // namespace rlx_teleport