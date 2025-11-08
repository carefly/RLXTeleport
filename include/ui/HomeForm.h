#pragma once
#include "manager/HomeManager.h"
#include <functional>
#include <mc/world/actor/player/Player.h>
#include <vector>


namespace rlx_teleport {

class HomeForm {
public:
    // 回调函数类型：参数为玩家和选中的 home
    using HomeSelectedCallback = std::function<void(Player&, const HomeManager::HomePoint&)>;

    /**
     * 显示 home 列表表单
     * @param player 玩家对象
     * @param homes home 列表
     * @param onSelected 选择回调函数，当玩家选择一个 home 时调用
     */
    static void
    showHomes(Player& player, const std::vector<HomeManager::HomePoint>& homes, HomeSelectedCallback onSelected);


private:
    HomeForm() = default;

    /**
     * 对 home 列表按首个字符排序
     * @param homes home 列表（会被修改）
     */
    static void sortHomesByFirstChar(std::vector<HomeManager::HomePoint>& homes);
};

} // namespace rlx_teleport
