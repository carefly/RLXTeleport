#pragma once

namespace rlx_warp {

class WarpCommand {
public:
    static WarpCommand& getInstance() {
        static WarpCommand instance;
        return instance;
    }

    void registerCommands();

private:
    WarpCommand() = default;

    WarpCommand(const WarpCommand&)            = delete;
    WarpCommand& operator=(const WarpCommand&) = delete;
};

} // namespace rlx_warp
