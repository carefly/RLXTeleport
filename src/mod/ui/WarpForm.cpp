#include "ui/WarpForm.h"
#include <algorithm>
#include <ll/api/form/SimpleForm.h>
#include <mc/world/actor/player/Player.h>

using namespace rlx_teleport;

void WarpForm::sortWarpsByFirstChar(std::vector<Warp>& warps) {
    std::sort(warps.begin(), warps.end(), [](const Warp& a, const Warp& b) {
        if (a.name.empty() && b.name.empty()) return false;
        if (a.name.empty()) return true;
        if (b.name.empty()) return false;
        return a.name[0] < b.name[0];
    });
}

void WarpForm::showWarps(Player& player, const std::vector<Warp>& warps, WarpSelectedCallback onSelected) {
    if (warps.empty()) {
        player.sendMessage("§c没有可用的传送点");
        return;
    }

    // 创建排序后的副本
    auto sortedWarps = warps;
    sortWarpsByFirstChar(sortedWarps);

    ll::form::SimpleForm form("传送点", "选择一个传送点");
    for (const auto& warp : sortedWarps) {
        std::string buttonText = warp.name;
        if (!warp.description.empty()) {
            buttonText += "\n§7" + warp.description;
        }
        form.appendButton(buttonText);
    }

    form.sendTo(player, [sortedWarps, onSelected](Player& player, int selected, ll::form::FormCancelReason) {
        if (selected == -1) {
            // 用户关闭了表单
            return;
        }
        if (selected >= 0 && selected < static_cast<int>(sortedWarps.size())) {
            const auto& warp = sortedWarps[selected];
            if (onSelected) {
                onSelected(player, warp);
            }
        }
    });
}
