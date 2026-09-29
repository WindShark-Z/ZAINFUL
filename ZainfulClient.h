#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace Zainful {

struct Vec3 {
    float x = 0;
    float y = 0;
    float z = 0;
};

struct Player {
    std::uint64_t id = 0;
    std::string name;
    Vec3 position;
    bool alive = true;
    bool sameDimension = true;
};

struct Frame {
    std::uint64_t tick = 0;
    std::uint64_t localId = 0;
    Vec3 position;
    Vec3 eyePosition;
    float yaw = 0;
    float pitch = 0;
    bool alive = true;
    bool movingForward = false;
    bool canSprint = true;
    std::vector<Player> players;
};

struct Rotation {
    float yaw;
    float pitch;
};

struct Tracer {
    std::uint64_t playerId;
    std::string name;
    Vec3 from;
    Vec3 to;
    float distance;
};

struct Actions {
    bool sprint = false;
    std::optional<std::uint64_t> attackTarget;
    std::optional<Rotation> aim;
    std::vector<Tracer> tracers;
};

enum class Module { AutoSprint, KillAura, Esp, AimAssist };

class Client {
public:
    static constexpr const char* NAME = "ZAINFUL";
    static constexpr float AURA_RANGE = 4.0f;
    static constexpr float ESP_RANGE = 100.0f;
    static constexpr std::uint64_t ATTACK_INTERVAL_TICKS = 10;

    bool enabled(Module module) const;
    void toggle(Module module);
    Actions onPlayerTick(const Frame& frame);
    std::string renderGui() const;

private:
    bool autoSprint_ = false;
    bool killAura_ = false;
    bool esp_ = false;
    bool aimAssist_ = false;
    std::optional<std::uint64_t> lastAttackTick_;
};

}
