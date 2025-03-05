#pragma once

namespace rlx_teleport {

class HomeCommand {
public:
    static HomeCommand& getInstance() {
        static HomeCommand instance;
        return instance;
    }

    void registerCommands();

private:
    HomeCommand() = default;

    HomeCommand(const HomeCommand&)            = delete;
    HomeCommand& operator=(const HomeCommand&) = delete;
};

} // namespace rlx_teleport
