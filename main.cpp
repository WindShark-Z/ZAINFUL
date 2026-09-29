#include "ZainfulClient.h"

#include <iostream>
#include <string>

int main() {
    Zainful::Client client;
    Zainful::Frame frame;
    frame.localId = 1;
    frame.eyePosition = {0, 1.62f, 0};
    frame.movingForward = true;
    frame.players = {
        {2, "Nearby", {3, 0, 0}},
        {3, "FarAway", {75, 0, 0}},
    };

    std::cout << "ZAINFUL Termux demo (simulated players; not connected to Minecraft)\n";
    std::string command;
    while (std::cout << client.renderGui() && std::getline(std::cin, command)) {
        if (command == "q") break;
        if (command == "1") client.toggle(Zainful::Module::AutoSprint);
        else if (command == "2") client.toggle(Zainful::Module::KillAura);
        else if (command == "3") client.toggle(Zainful::Module::Esp);
        else if (command == "4") client.toggle(Zainful::Module::AimAssist);
        else if (command == "t") {
            const Zainful::Actions actions = client.onPlayerTick(frame);
            std::cout << "Tick " << frame.tick << ": sprint=" << (actions.sprint ? "yes" : "no");
            if (actions.attackTarget) std::cout << ", attack player " << *actions.attackTarget;
            if (actions.aim) {
                std::cout << ", aim yaw=" << actions.aim->yaw
                          << " pitch=" << actions.aim->pitch;
                frame.yaw = actions.aim->yaw;
                frame.pitch = actions.aim->pitch;
            }
            std::cout << ", tracers=" << actions.tracers.size() << '\n';
            for (const auto& tracer : actions.tracers) {
                std::cout << "  " << tracer.name << " (" << tracer.distance << " blocks)\n";
            }
            ++frame.tick;
        } else {
            std::cout << "Choose 1-4, t, or q.\n";
        }
    }
}
