#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include "common/ConfigManager.hpp"

namespace rlx_teleport {

/// @brief 传送系统配置数据结构
struct TeleportConfigData {
    // ===== Home 配置 =====
    int homeLimit = 16;  // 每个玩家最大家数量

    // ===== TPA 配置 =====
    int tpaTimeout = 60;  // 传送请求超时时间（秒）

    // ===== 物品触发配置 =====
    std::string triggerItemKeyword = "sword";  // 触发传送菜单的物品关键词
};

// 定义 JSON 序列化（nlohmann/json 宏）
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TeleportConfigData, homeLimit, tpaTimeout, triggerItemKeyword);

// 类型别名：简化单例访问
using TeleportConfig = rlx::common::Config<TeleportConfigData>;

// 便捷访问函数
inline const TeleportConfigData& Config() {
    return TeleportConfig::getInstance().get();
}

} // namespace rlx_teleport
