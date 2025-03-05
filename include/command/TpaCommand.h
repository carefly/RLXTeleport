#pragma once

namespace rlx_teleport {

class TpaCommand {
public:
    static TpaCommand& getInstance() {
        static TpaCommand instance;
        return instance;
    }

    void registerCommands();

private:
    TpaCommand() = default;

    TpaCommand(const TpaCommand&)            = delete;
    TpaCommand& operator=(const TpaCommand&) = delete;
};

} // namespace rlx_teleport