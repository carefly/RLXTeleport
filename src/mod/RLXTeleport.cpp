#include "RLXTeleport.h"

#include "command/HomeCommand.h"
#include "command/WarpCommand.h"
#include "manager/ConfigManager.h"
#include "manager/HomeManager.h"
#include "manager/WarpManager.h"

#include <ll/api/Config.h>
#include <ll/api/mod/RegisterHelper.h>


using namespace rlx_teleport;

RLXTeleport& RLXTeleport::getInstance() {
    static RLXTeleport instance;
    return instance;
}

bool RLXTeleport::load() {
    ConfigManager::getInstance().init();
    WarpManager::getInstance().init();
    HomeManager::getInstance().init();

    ConfigManager::getInstance().setDir(getSelf().getModDir().string() + "/");
    WarpManager::getInstance().setDir(getSelf().getModDir().string() + "/");
    HomeManager::getInstance().setDir(getSelf().getModDir().string() + "/");

    ConfigManager::getInstance().loadConfig();

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
    getSelf().getLogger().info("Commands registered");

    return true;
}

bool RLXTeleport::disable() {
    getSelf().getLogger().info("can not disable");

    return true;
}

LL_REGISTER_MOD(rlx_teleport::RLXTeleport, rlx_teleport::RLXTeleport::getInstance());
