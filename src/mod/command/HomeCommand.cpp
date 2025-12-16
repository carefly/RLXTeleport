#include "command/HomeCommand.h"
#include "common/utils.h"
#include "manager/HomeManager.h"
#include "ui/HomeForm.h"
#include <ll/api/command/Command.h>
#include <ll/api/command/CommandHandle.h>
#include <ll/api/command/CommandRegistrar.h>
#include <ll/api/event/player/PlayerDieEvent.h>
#include <mc/server/commands/CommandOutput.h>
#include <mc/server/commands/CommandRawText.h>
#include <string>


using namespace rlx_teleport;


struct SuicideCommand {};
struct BackCommand {};
struct HomeCommad {};

enum HomeCommandOperation : int { add = 0, del = 1, go = 2 };
struct HomeOperationCommand {
    HomeCommandOperation Operation;
    CommandRawText       Name;
};
struct HomeLsCommad {};

void HomeCommand::registerCommands() {
    using ll::command::CommandRegistrar;
    auto& suicideCommand = CommandRegistrar::getInstance().getOrCreateCommand("suicide", "原地躺下");
    suicideCommand.overload<SuicideCommand>().execute(
        [](CommandOrigin const& origin, CommandOutput& output, SuicideCommand const&, Command const&) {
            auto* entity = origin.getEntity();
            if (entity == nullptr || !entity->isType(ActorType::Player)) {
                output.error("Only players can do");
                return;
            }

            auto* sp = static_cast<Player*>(entity);

            sp->kill();
            output.success("你已阵亡");
        }
    );

    auto& backCommand = CommandRegistrar::getInstance().getOrCreateCommand("back", "返回上次挂掉的位置");
    backCommand.overload<SuicideCommand>().execute(
        [](CommandOrigin const& origin, CommandOutput& output, SuicideCommand const&, Command const&) {
            auto* entity = origin.getEntity();
            if (entity == nullptr || !entity->isType(ActorType::Player)) {
                output.error("Only players can do");
                return;
            }

            auto* sp = static_cast<Player*>(entity);

            auto it = HomeManager::getInstance().getDeathPoint(sp->getXuid());
            if (!it) output.error("没到找死亡点");
            else sp->teleport(it->pos, it->d);
            // HomeManager::getInstance().clearDeathPoint(sp->getXuid());
        }
    );

    auto& homeCommand = CommandRegistrar::getInstance().getOrCreateCommand("home", "家");
    homeCommand.overload<HomeOperationCommand>()
        .required("Operation")
        .required("Name")
        .execute(
            [](CommandOrigin const& origin, CommandOutput& output, HomeOperationCommand const& param, Command const&) {
                auto* entity = origin.getEntity();
                if (entity == nullptr || !entity->isType(ActorType::Player)) {
                    output.error("Only players can do");
                    return;
                }

                auto* sp = static_cast<Player*>(entity);

                std::string name = param.Name.mText;

                if (HomeCommandOperation::add == param.Operation) {
                    auto pos = sp->getPosition();
                    pos      = Utils::fixPos(pos);

                    auto home = HomeManager::HomePoint{name, pos, sp->getDimensionId()};
                    auto hp   = HomeManager::getInstance().addHome(home, sp->getXuid(), sp->getNameTag());
                    if (hp == HomeManager::HomeResult::LimitExceeded) {
                        output.error("家的数量不能超过 " + std::to_string(HomeManager::getInstance().getHomeLimit()));
                        return;
                    } else if (hp == HomeManager::HomeResult::Duplicate) {
                        output.error("已经有家的名称为 " + name + " 不能重复添加");
                        return;
                    } else if (hp == HomeManager::HomeResult::Success) {
                        output.success(
                            "成功添加家 " + name + " 目前家的数量为 "
                            + std::to_string(HomeManager::getInstance().getHomeCount(sp->getXuid()))
                        );
                    }
                } else if (HomeCommandOperation::del == param.Operation) {
                    auto hp = HomeManager::getInstance().delHome(name, sp->getXuid(), sp->getNameTag());
                    if (hp == HomeManager::HomeResult::NotFound) {
                        output.error("没有家 " + name + " 无法删除");
                        return;
                    } else if (hp == HomeManager::HomeResult::Success) {
                        output.success(
                            "成功删除家 " + name + " 目前家的数量为 "
                            + std::to_string(HomeManager::getInstance().getHomeCount(sp->getXuid()))
                        );
                    }
                } else if (HomeCommandOperation::go == param.Operation) {
                    auto homes = HomeManager::getInstance().getHomes(sp->getXuid());
                    for (HomeManager::HomePoint mhp : homes) {
                        if (Utils::equalsIgnoreCase(mhp.name, name)) {
                            sp->teleport(mhp.pos, mhp.d);
                            output.success("§b 成功传送到家 " + mhp.name);
                            return;
                        }
                    }
                    output.error(format("没有家 {} 无法传送", name));
                }
            }
        );
    homeCommand.overload<HomeLsCommad>().text("ls").execute(
        [](CommandOrigin const& origin, CommandOutput& output, HomeLsCommad const&, Command const&) {
            auto* entity = origin.getEntity();
            if (entity == nullptr || !entity->isType(ActorType::Player)) {
                output.error("Only players can do");
                return;
            }
            auto* sp = static_cast<Player*>(entity);

            auto homes = HomeManager::getInstance().getHomes(sp->getXuid());
            if (homes.empty()) {
                output.error("没有家");
                return;
            }

            output.success("§b====家 列表====");
            for (HomeManager::HomePoint hp : homes) {
                output.success(hp.name);
            }
            output.success("§b===========");
        }
    );
    homeCommand.overload<HomeCommad>().execute(
        [](CommandOrigin const& origin, CommandOutput& output, HomeCommad const&, Command const&) {
            auto* entity = origin.getEntity();
            if (entity == nullptr || !entity->isType(ActorType::Player)) {
                output.error("Only players can do");
                return;
            }
            auto* sp    = static_cast<Player*>(entity);
            auto  homes = HomeManager::getInstance().getHomes(sp->getXuid());
            if (homes.empty()) {
                output.error("没有家");
                return;
            }
            HomeForm::showHomes(*sp, homes, [](Player& player, const HomeManager::HomePoint& home) {
                player.teleport(home.pos, home.d);
                player.sendMessage("§b 成功传送到家 " + home.name);
            });
        }
    );
}