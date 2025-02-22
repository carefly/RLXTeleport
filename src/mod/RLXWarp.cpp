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
    getSelf().getLogger().debug("Loading warps");
    WarpManager::getInstance().init();
    getSelf().getLogger().debug("{} warps loaded", WarpManager::getInstance().getWarpCount());

    getSelf().getLogger().debug("Load success");
    return true;
}

bool RLXWarp::enable() {
    getSelf().getLogger().debug("registerCommands");
    WarpCommand::getInstance().registerCommands();
    getSelf().getLogger().debug("Commands registered");

    return true;
}

bool RLXWarp::disable() {
    getSelf().getLogger().debug("can not disable");

    return true;
}

LL_REGISTER_MOD(rlx_warp::RLXWarp, rlx_warp::RLXWarp::getInstance());
