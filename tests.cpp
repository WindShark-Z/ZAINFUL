#include "ZainfulClient.h"

#include <cmath>
#include <cstdlib>
#include <iostream>

namespace {

void expect(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(1);
    }
}

bool near(float a, float b) {
    return std::fabs(a - b) < 0.01f;
}

}

int main() {
    using namespace Zainful;
    Client client;
    Frame frame;
    frame.localId = 1;
    frame.eyePosition = {0, 1.62f, 0};
    frame.movingForward = true;
    frame.players = {
        {1, "Self", {0, 0, 0}},
        {2, "Edge", {4, 0, 0}},
        {3, "Closer", {0, 0, 3}},
        {4, "EspEdge", {0, 0, 100}},
        {5, "OutOfRange", {101, 0, 0}},
        {6, "Dead", {1, 0, 0}, false},
        {7, "Elsewhere", {1, 0, 0}, true, false},
    };

    Actions actions = client.onPlayerTick(frame);
    expect(!actions.sprint && !actions.attackTarget && !actions.aim &&
           actions.tracers.empty(), "modules start off");

    client.toggle(Module::AutoSprint);
    client.toggle(Module::KillAura);
    client.toggle(Module::Esp);
    client.toggle(Module::AimAssist);
    actions = client.onPlayerTick(frame);
    expect(actions.sprint, "sprint when holding forward");
    expect(actions.attackTarget == 3, "attack closest player within four blocks");
    expect(actions.aim.has_value() && near(actions.aim->yaw, 0) &&
           near(actions.aim->pitch, 0), "aim toward closest player's head");
    expect(actions.tracers.size() == 3 && actions.tracers[0].playerId == 2 &&
           actions.tracers[2].playerId == 4, "tracers include 100-block boundary");
    expect(actions.tracers[0].name == "Edge" && near(actions.tracers[0].to.y, 1.62f),
           "tracer retains player name and head endpoint");

    frame.tick = 9;
    expect(!client.onPlayerTick(frame).attackTarget, "aura respects cooldown");
    frame.tick = 10;
    expect(client.onPlayerTick(frame).attackTarget == 3, "aura resumes after cooldown");
    frame.movingForward = false;
    expect(!client.onPlayerTick(frame).sprint, "sprint stops on forward release");
    frame.movingForward = true;
    frame.canSprint = false;
    expect(!client.onPlayerTick(frame).sprint, "sprint respects game restrictions");
    frame.canSprint = true;

    frame.players[2].position = {0, 0, 4.01f};
    frame.tick = 20;
    expect(client.onPlayerTick(frame).attackTarget == 2, "four-block limit is strict");

    frame.players[1].position = {0, 0, -101};
    frame.players[2].position = {0, 0, 101};
    frame.players[3].position = {0, 0, 101};
    actions = client.onPlayerTick(frame);
    expect(!actions.attackTarget && !actions.aim && actions.tracers.empty(),
           "distant players are ignored");

    frame.players[1].position = {0, 0, -3};
    frame.yaw = 179;
    actions = client.onPlayerTick(frame);
    expect(actions.aim.has_value() && near(actions.aim->yaw, 179.35f),
           "aim uses shortest yaw path across 180 degrees");

    frame.alive = false;
    actions = client.onPlayerTick(frame);
    expect(!actions.sprint && !actions.attackTarget && !actions.aim &&
           actions.tracers.empty(), "dead local player has no actions");

    std::cout << "ZAINFUL feature checks passed\n";
}
