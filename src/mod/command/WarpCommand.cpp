#include "command/WarpCommand.h"
#include "WarpManager.h"

#include <ll/api/command/Command.h>
#include <ll/api/command/CommandHandle.h>
#include <ll/api/command/CommandRegistrar.h>
#include <mc/server/commands/CommandOutput.h>
#include <mc/server/commands/CommandRawText.h>
#include <mc/world/actor/Actor.h>
#include <mc/world/actor/player/Player.h>


using namespace rlx_teleport;

struct WarpLsCommad {};
struct WarpGoCommad {
    CommandRawText Name;
};

enum WarpOperation : int { add = 0, del = 1 };
struct WarpOperationCommad {
    WarpOperation  Operation;
    CommandRawText Name;
    CommandRawText Description;
};

struct WarpReloadCommad {};


void WarpCommand::registerCommands() {
    using ll::command::CommandRegistrar;
    auto& warpCommand = CommandRegistrar::getInstance().getOrCreateCommand("warp", "warp to a specific location");
    warpCommand.overload<WarpGoCommad>().required("Name").execute(
        [](CommandOrigin const& origin, CommandOutput& output, WarpGoCommad const& param, Command const&) {
            auto* entity = origin.getEntity();
            if (entity == nullptr || !entity->isType(ActorType::Player)) {
                output.error("Only players can do");
                return;
            }
            auto* sp       = static_cast<Player*>(entity);
            auto  warpName = param.Name.mText;

            if ("ls" == warpName) {
                output.success("§b====Warp 列表====");
                auto& warps = WarpManager::getInstance().getWarps();
                for (auto& warp : warps) {
                    std::string warpInfo = warp.name + "    " + warp.description;
                    output.success(warpInfo);
                }
                output.success("§b===========");
            } else {
                auto* warp = WarpManager::getInstance().getWarp(warpName);
                if (warp == nullptr) {
                    output.error(format("没有 warp 点 {}", warpName));
                    return;
                }
                auto pos = Vec3(warp->x, warp->y, warp->z);
                sp->teleport(pos, warp->d);
                output.success("§b 成功传送到 warp " + warp->name);
            }
        }
    );
    warpCommand.overload<WarpLsCommad>().text("ls").execute(
        [](CommandOrigin const&, CommandOutput&, WarpLsCommad const&, Command const&) {

        }
    );

    auto& warpOperationCommand =
        CommandRegistrar::getInstance().getOrCreateCommand("warpop", "地标管理", CommandPermissionLevel::GameDirectors);
    warpOperationCommand.overload<WarpOperationCommad>()
        .required("Operation")
        .required("Name")
        .optional("Description")
        .execute(
            [&](CommandOrigin const& ori, CommandOutput& output, WarpOperationCommad const& param, Command const&) {
                if ((int)ori.getPermissionsLevel() < 1) return;

                Actor*      actor = ori.getEntity();
                Warp        wp;
                std::string warp = param.Name.mText;

                switch (param.Operation) {
                case WarpOperation::add: {
                    auto pos       = actor->getPosition();
                    pos.y          = float(int(pos.y - 1));
                    wp.name        = warp;
                    wp.d           = actor->getDimensionId();
                    wp.x           = pos.x;
                    wp.y           = pos.y;
                    wp.z           = pos.z;
                    wp.description = param.Description.mText;

                    auto result = WarpManager::getInstance().addWarp(wp);
                    if (WarpManager::WarpResult::Success == result) {
                        output.success(format("成功添加warp点 {}", warp));
                    } else if (WarpManager::WarpResult::Duplicate == result) {
                        output.error(format("重复的warp点 {}", warp));
                    } else if (WarpManager::WarpResult::LoadFailed == result) {
                        output.error("未能正确加载warp.json文件，请检查文件是否正确，执行 /warpop reload 重新加载");
                    }
                    break;
                }
                case WarpOperation::del: {
                    auto result = WarpManager::getInstance().delWarp(warp);
                    if (WarpManager::WarpResult::Success == result) {
                        output.success(format("成功删除warp点 {}", warp));
                    } else if (WarpManager::WarpResult::LoadFailed == result) {
                        output.error("未能正确加载warp.json文件，请检查文件是否正确，执行 /warpop reload 重新加载");
                    } else {
                        output.error(format("没有warp点 {}, 无法删除", warp));
                    }
                    break;
                }
                default:
                    break;
                }
            }
        );

    warpOperationCommand.overload<WarpReloadCommad>().text("reload").execute(
        [&](CommandOrigin const& ori, CommandOutput& output, WarpReloadCommad const&, Command const&) {
            if ((int)ori.getPermissionsLevel() < 1) return;

            std::string error_msg;
            auto        result = WarpManager::getInstance().load(error_msg);
            if (WarpManager::WarpResult::Success == result) {
                output.success("成功加载warp点, 当前有 {} 个warp点", WarpManager::getInstance().getWarpCount());
            } else {
                output.error("未能正确加载warp.json文件，请检查文件是否正确，错误信息：" + error_msg);
            }
        }
    );
}
