#include "ui/HomeForm.h"
#include <algorithm>
#include <ll/api/form/SimpleForm.h>
#include <mc/world/actor/player/Player.h>

using namespace rlx_teleport;

void HomeForm::sortHomesByFirstChar(std::vector<HomeManager::HomePoint>& homes) {
    std::sort(homes.begin(), homes.end(), [](const HomeManager::HomePoint& a, const HomeManager::HomePoint& b) {
        if (a.name.empty() && b.name.empty()) return false;
        if (a.name.empty()) return true;
        if (b.name.empty()) return false;
        return a.name[0] < b.name[0];
    });
}

void HomeForm::showHomes(
    Player&                                    player,
    const std::vector<HomeManager::HomePoint>& homes,
    HomeSelectedCallback                       onSelected
) {
    if (homes.empty()) {
        player.sendMessage("§c没有可用的家");
        return;
    }

    // 创建排序后的副本
    auto sortedHomes = homes;
    sortHomesByFirstChar(sortedHomes);

    ll::form::SimpleForm form("家", "选择一个家");
    for (const auto& home : sortedHomes) {
        form.appendButton(home.name);
    }

    form.sendTo(player, [sortedHomes, onSelected](Player& player, int selected, ll::form::FormCancelReason) {
        if (selected == -1) {
            // 用户关闭了表单
            return;
        }
        if (selected >= 0 && selected < static_cast<int>(sortedHomes.size())) {
            const auto& home = sortedHomes[selected];
            if (onSelected) {
                onSelected(player, home);
            }
        }
    });
}
