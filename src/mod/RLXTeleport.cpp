#include "RLXTeleport.h"

#include "WarpManager.h"
#include "command/WarpCommand.h"
#include "ll/api/mod/RegisterHelper.h"
#include <ll/api/Config.h>


using namespace rlx_teleport;

RLXTeleport& RLXTeleport::getInstance() {
    static RLXTeleport instance;
    return instance;
}

bool RLXTeleport::load() {
    WarpManager::getInstance().setDir(getSelf().getModDir().string() + "/");
    WarpManager::getInstance().init();
    std::string error_msg;
    if (WarpManager::WarpResult::Success == WarpManager::getInstance().load(error_msg)) {
        getSelf().getLogger().info("{} warps loaded", WarpManager::getInstance().getWarpCount());
    } else {
        getSelf().getLogger().error("Failed to load warps, error: {}", error_msg);
    }
    return true;
}

bool RLXTeleport::enable() {

    WarpCommand::getInstance().registerCommands();
    getSelf().getLogger().info("Commands registered");

    return true;
}

bool RLXTeleport::disable() {
    getSelf().getLogger().info("can not disable");

    return true;
}

LL_REGISTER_MOD(rlx_teleport::RLXTeleport, rlx_teleport::RLXTeleport::getInstance());
