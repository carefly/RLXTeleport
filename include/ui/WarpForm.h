#pragma once
#include "manager/WarpManager.h"
#include <functional>
#include <mc/world/actor/player/Player.h>
#include <vector>

namespace rlx_teleport {

class WarpForm {
public:
    // 回调函数类型：参数为玩家和选中的 warp
    using WarpSelectedCallback = std::function<void(Player&, const Warp&)>;

    /**
     * 显示 warp 列表表单
     * @param player 玩家对象
     * @param warps warp 列表
     * @param onSelected 选择回调函数，当玩家选择一个 warp 时调用
     */
    static void showWarps(Player& player, const std::vector<Warp>& warps, WarpSelectedCallback onSelected);

private:
    WarpForm() = default;

    /**
     * 对 warp 列表按首个字符排序
     * @param warps warp 列表（会被修改）
     */
    static void sortWarpsByFirstChar(std::vector<Warp>& warps);
};

} // namespace rlx_teleport
