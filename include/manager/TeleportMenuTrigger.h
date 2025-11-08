#pragma once

#include <mc/world/actor/player/Player.h>
#include <mc/world/item/ItemStack.h>

namespace rlx_teleport {

class TeleportMenuTrigger {

public:
    static TeleportMenuTrigger& getInstance() {
        static TeleportMenuTrigger instance;
        return instance;
    }

    /**
     * 初始化事件监听
     */
    void initHooks();

private:
    TeleportMenuTrigger() = default;

    TeleportMenuTrigger(const TeleportMenuTrigger&)            = delete;
    TeleportMenuTrigger& operator=(const TeleportMenuTrigger&) = delete;

    // 检查物品是否是触发物品
    bool isTriggerItem(const ItemStack& item) const;

    // 显示主菜单（供事件监听器使用）
    void showMainMenu(Player& player);

    // 显示 Home 表单
    void showHomeForm(Player& player);

    // 显示 Warp 表单
    void showWarpForm(Player& player);
};

} // namespace rlx_teleport

