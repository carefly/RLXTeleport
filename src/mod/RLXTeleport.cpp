#include "RLXTeleport.h"

#include "command/HomeCommand.h"
#include "command/TpaCommand.h"
#include "command/WarpCommand.h"
#include "manager/HomeManager.h"
#include "manager/TeleportMenuTrigger.h"
#include "manager/TeleportConfig.hpp"
#include "manager/WarpManager.h"

#include <ll/api/Config.h>
#include <ll/api/mod/RegisterHelper.h>


using namespace rlx_teleport;

RLXTeleport& RLXTeleport::getInstance() {
    static RLXTeleport instance;
    return instance;
}

bool RLXTeleport::load() {

    // 初始化配置单例
    TeleportConfig::initWithName("RLXTeleport.json");

    auto dir = getSelf().getModDir().string() + "/../RLXModeResources";

    WarpManager::getInstance().setDir(dir + "/data/");
    HomeManager::getInstance().setDir(dir + "/data/");

    getSelf().getLogger().info("Config loaded: homeLimit={}, tpaTimeout={}, triggerItemKeyword={}",
        Config().homeLimit,
        Config().tpaTimeout,
        Config().triggerItemKeyword
    );

    std::string error_msg;
    if (WarpManager::WarpResult::Success == WarpManager::getInstance().load(error_msg)) {
        getSelf().getLogger().info("{} warps loaded", WarpManager::getInstance().getWarpCount());
    } else {
        getSelf().getLogger().error("Failed to load warps, error: {}", error_msg);
    }
    if (HomeManager::HomeResult::Success == HomeManager::getInstance().load(error_msg)) {
        getSelf().getLogger().info("{} homes loaded", HomeManager::getInstance().getHomeCount());
    } else {
        getSelf().getLogger().error("Failed to load homes, error: {}", error_msg);
    }
    return true;
}

bool RLXTeleport::enable() {

    WarpCommand::getInstance().registerCommands();
    HomeCommand::getInstance().registerCommands();
    TpaCommand::getInstance().registerCommands();

    HomeManager::getInstance().initHooks();
    TeleportMenuTrigger::getInstance().initHooks();

    getSelf().getLogger().info("Commands registered");

    return true;
}

bool RLXTeleport::disable() {
    getSelf().getLogger().info("can not disable");

    return true;
}

LL_REGISTER_MOD(rlx_teleport::RLXTeleport, rlx_teleport::RLXTeleport::getInstance());
