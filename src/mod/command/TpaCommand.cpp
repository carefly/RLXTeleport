#include "command/TpaCommand.h"
#include "manager/TpaManager.h"

#include <fmt/format.h>
#include <ll/api/command/Command.h>
#include <ll/api/command/CommandHandle.h>
#include <ll/api/command/CommandRegistrar.h>
#include <ll/api/service/Bedrock.h>
#include <mc/server/commands/CommandOutput.h>
#include <mc/server/commands/CommandRawText.h>
#include <mc/server/commands/CommandSelector.h>
#include <mc/world/actor/player/Player.h>
#include <mc/world/level/Level.h>


using namespace rlx_teleport;

enum TpaRequestOperation : int { call = 0, to = 1 };
struct TpaRequestCommand {
    TpaRequestOperation     Operation;
    CommandSelector<Player> Player;
};

enum TpaResponseOperation : int { accept = 0, reject = 1 };
struct TpaResponseCommand {
    TpaResponseOperation Operation;
};

enum TpaCommonOperation : int { cancel = 0, query = 1 };
struct TpaCommonCommand {
    TpaCommonOperation Operation;
};

void TpaCommand::registerCommands() {
    using ll::command::CommandRegistrar;
    auto& tpaCommand = CommandRegistrar::getInstance().getOrCreateCommand("tpa", "传送到指定玩家");
    tpaCommand.overload<TpaRequestCommand>()
        .required("Operation")
        .required("Player")
        .execute(
            [](CommandOrigin const& origin, CommandOutput& output, TpaRequestCommand const& param, Command const&) {
                auto* entity = origin.getEntity();
                if (entity == nullptr || !entity->isType(ActorType::Player)) {
                    output.error("Only players can do");
                    return;
                }

                auto  direction   = TpaManager::TpaRequestDirection::goto_player;
                auto  text        = std::string();
                auto* from_player = static_cast<Player*>(entity);
                auto  it          = param.Player.results(origin).data;
                if (it == nullptr || it->size() == 0) {
                    output.error("§a§l[传送请求] §r§c目标玩家不存在");
                    return;
                }

                if (it->size() > 1) {
                    output.error("§a§l[传送请求] §r§c只能选择一个玩家");
                    return;
                }

                auto* to_player = it->at(0);

                switch (param.Operation) {
                case TpaRequestOperation::call: {
                    direction = TpaManager::TpaRequestDirection::come_to_me;
                    text      = fmt::format(
                        "§a§l[传送请求] §f§l{} §r请求你传送到他那里\n 输入 /tpa accept 接受请求\n 输入 /tpa reject "
                             "拒绝请求",
                        from_player->getRealName()
                    );
                    break;
                }
                case TpaRequestOperation::to: {
                    direction = TpaManager::TpaRequestDirection::goto_player;
                    text      = fmt::format(
                        "§a§l[传送请求] §f§l{} §r请求传送到你这里\n 输入 /tpa accept 接受请求\n 输入 /tpa reject "
                             "拒绝请求",
                        from_player->getRealName()
                    );
                    break;
                }
                }

                auto result =
                    TpaManager::getInstance().addTpaRequest(from_player->getXuid(), to_player->getXuid(), direction);
                if (result == TpaManager::TpaAddRequestResult::already_has_request) {
                    output.error("§a§l[传送请求] §r§c你已经发送过传送请求了");
                    return;
                }

                if (result == TpaManager::TpaAddRequestResult::already_has_response) {
                    output.error("§a§l[传送请求] §r§c目标玩家正在处理其他的传送请求");
                    return;
                }

                output.success("§a§l[传送请求] §r传送请求已发送");
                to_player->sendMessage(text);
            }
        );

    tpaCommand.overload<TpaResponseCommand>()
        .required("Operation")
        .execute(
            [](CommandOrigin const& origin, CommandOutput& output, TpaResponseCommand const& param, Command const&) {
                auto* entity = origin.getEntity();
                if (entity == nullptr || !entity->isType(ActorType::Player)) {
                    output.error("Only players can do");
                    return;
                }

                auto* to_player = static_cast<Player*>(entity);
                auto  req       = TpaManager::getInstance().findRequestByReceiver(to_player->getXuid());
                if (req == nullptr) {
                    output.error("§a§l[传送请求] §r§c传送请求不存在或者已过期");
                    return;
                }

                auto* from_player = ll::service::getLevel()->getPlayerByXuid(req->from_xuid);
                if (from_player == nullptr) {
                    output.error("§a§l[传送请求] §r§c目标玩家不存在");
                    return;
                }

                switch (param.Operation) {
                case TpaResponseOperation::accept: {
                    if (req->direction == TpaManager::TpaRequestDirection::goto_player) {
                        from_player->teleport(to_player->getPosition(), to_player->getDimensionId());
                        from_player->sendMessage(
                            fmt::format("§a§l[传送请求] §r§b你已经传送到 §f§l{} §r§b那里", to_player->getRealName())
                        );
                        to_player->sendMessage(
                            fmt::format("§a§l[传送请求] §f§l{} §r§b已传送到你这里", from_player->getRealName())
                        );
                    } else if (req->direction == TpaManager::TpaRequestDirection::come_to_me) {
                        to_player->teleport(from_player->getPosition(), from_player->getDimensionId());
                        from_player->sendMessage(
                            fmt::format("§a§l[传送请求] §f§l{} §r§b已传送到你这里", to_player->getRealName())
                        );
                        to_player->sendMessage(
                            fmt::format("§a§l[传送请求] §r§b你已经传送到 §f§l{} §r§b那里", from_player->getRealName())
                        );
                    }
                    break;
                }
                case TpaResponseOperation::reject: {
                    from_player->sendMessage(
                        fmt::format("§a§l[传送请求] §f§l{} §r§c已拒绝你的传送请求", to_player->getRealName())
                    );
                    to_player->sendMessage(
                        fmt::format("§a§l[传送请求] §r§b你已经拒绝了 §f§l{} §r§b的传送请求", from_player->getRealName())
                    );
                    break;
                }
                }

                auto result = TpaManager::getInstance().onTpaResponse(to_player->getXuid());
                if (result == false) {
                    output.error("§a§l[传送请求] §r§c传送请求不存在或者已过期");
                    return;
                }
            }
        );

    tpaCommand.overload<TpaCommonCommand>()
        .required("Operation")
        .execute([](CommandOrigin const& origin, CommandOutput& output, TpaCommonCommand const& param, Command const&) {
            auto* entity = origin.getEntity();
            if (entity == nullptr || !entity->isType(ActorType::Player)) {
                output.error("Only players can do");
                return;
            }

            auto* from_player = static_cast<Player*>(entity);

            switch (param.Operation) {
            case TpaCommonOperation::cancel: {
                auto result = TpaManager::getInstance().cancelTpaRequest(from_player->getXuid());
                if (result == false) {
                    output.error("§a§l[传送请求] §r§c传送请求不存在或者已过期");
                    return;
                } else {
                    output.success("§a§l[传送请求] §r§b传送请求已取消");
                }
                break;
            }
            case TpaCommonOperation::query: {
                auto result = TpaManager::getInstance().queryTpaRequest(from_player->getXuid());
                if (result == nullptr) {
                    output.error("§a§l[传送请求] §r§c传送请求不存在或者已过期");
                    return;
                } else {
                    auto* to_player = ll::service::getLevel()->getPlayerByXuid(result->to_xuid);
                    if (to_player == nullptr) {
                        TpaManager::getInstance().cancelTpaRequest(from_player->getXuid());
                        output.error("§a§l[传送请求] §r§c目标玩家不存在,传送请求已取消");
                        return;
                    }

                    auto text = std::string();
                    if (result->direction == TpaManager::TpaRequestDirection::goto_player) {
                        text = fmt::format("§a§l[传送请求] §r请求传送到 §r§b{} §r那里", to_player->getRealName());
                    } else if (result->direction == TpaManager::TpaRequestDirection::come_to_me) {
                        text = fmt::format("§a§l[传送请求] §r请求 §r§b{} §r传送到你这里", to_player->getRealName());
                    }

                    output.success(text);
                }
                break;
            }
            }
        });
}