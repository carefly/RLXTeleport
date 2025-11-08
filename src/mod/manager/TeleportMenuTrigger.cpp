#include "manager/TeleportMenuTrigger.h"

#include "manager/ConfigManager.h"
#include "manager/HomeManager.h"
#include "manager/WarpManager.h"
#include "ui/HomeForm.h"
#include "ui/WarpForm.h"

#include <algorithm>
#include <cctype>
#include <ll/api/event/EventBus.h>
#include <ll/api/event/player/PlayerUseItemEvent.h>
#include <ll/api/form/SimpleForm.h>
#include <mc/deps/core/math/Vec3.h>
#include <string>


using namespace rlx_teleport;

void TeleportMenuTrigger::initHooks() {
    auto& eventBus = ll::event::EventBus::getInstance();

    // 监听玩家使用物品事件
    auto itemUseListener =
        eventBus.emplaceListener<ll::event::player::PlayerUseItemEvent>([](ll::event::player::PlayerUseItemEvent& event
                                                                        ) {
            auto& player = event.self();
            auto& item   = event.item();

            // 检查物品是否是触发物品
            auto& instance = TeleportMenuTrigger::getInstance();
            if (instance.isTriggerItem(item)) {
                // 显示主菜单
                instance.showMainMenu(player);
            }
        });
}

bool TeleportMenuTrigger::isTriggerItem(const ItemStack& item) const {
    // 获取物品的ID或名称
    auto itemName = item.getTypeName();

    // 从配置文件获取触发物品关键词
    auto&       configManager = ConfigManager::getInstance();
    std::string keyword       = configManager.getTriggerItemKeyword();

    // 转换为小写进行不区分大小写的匹配
    std::string itemNameLower = itemName;
    std::string keywordLower  = keyword;
    std::transform(itemNameLower.begin(), itemNameLower.end(), itemNameLower.begin(), ::tolower);
    std::transform(keywordLower.begin(), keywordLower.end(), keywordLower.begin(), ::tolower);

    // 检查物品名称是否包含关键词
    return itemNameLower.find(keywordLower) != std::string::npos;
}

void TeleportMenuTrigger::showMainMenu(Player& player) {
    ll::form::SimpleForm form("传送菜单", "选择一个选项");

    form.appendButton("§b家 (Home)");
    form.appendButton("§e传送点 (Warp)");

    form.sendTo(player, [this](Player& player, int selected, ll::form::FormCancelReason) {
        if (selected == -1) {
            // 用户关闭了表单
            return;
        }

        switch (selected) {
        case 0: // Home 按钮
            showHomeForm(player);
            break;
        case 1: // Warp 按钮
            showWarpForm(player);
            break;
        default:
            break;
        }
    });
}

void TeleportMenuTrigger::showHomeForm(Player& player) {
    auto homes = HomeManager::getInstance().getHomes(player.getXuid());
    if (homes.empty()) {
        player.sendMessage("§c你没有设置任何家");
        return;
    }

    HomeForm::showHomes(player, homes, [](Player& player, const HomeManager::HomePoint& home) {
        player.teleport(home.pos, home.d);
        player.sendMessage("§b 成功传送到家 " + home.name);
    });
}

void TeleportMenuTrigger::showWarpForm(Player& player) {
    auto& warps = WarpManager::getInstance().getWarps();
    if (warps.empty()) {
        player.sendMessage("§c没有可用的传送点");
        return;
    }

    WarpForm::showWarps(player, warps, [](Player& player, const Warp& warp) {
        Vec3 pos(warp.x, warp.y, warp.z);
        player.teleport(pos, warp.d);
        player.sendMessage("§b 成功传送到传送点 " + warp.name);
    });
}
