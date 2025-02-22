#include "mod/RLXWarp.h"

#include "WarpCommand.h"
#include "WarpManager.h"
#include "ll/api/mod/RegisterHelper.h"
#include <ll/api/Config.h>


using namespace rlx_warp;

RLXWarp& RLXWarp::getInstance() {
    static RLXWarp instance;
    return instance;
}

bool RLXWarp::load() {
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

bool RLXWarp::enable() {

    WarpCommand::getInstance().registerCommands();
    getSelf().getLogger().info("Commands registered");

    return true;
}

bool RLXWarp::disable() {
    getSelf().getLogger().info("can not disable");

    return true;
}

LL_REGISTER_MOD(rlx_warp::RLXWarp, rlx_warp::RLXWarp::getInstance());
