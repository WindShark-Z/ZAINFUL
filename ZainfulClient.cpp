#include "ZainfulClient.h"

#include <cmath>
#include <sstream>

namespace Zainful {
namespace {

constexpr float HEAD_HEIGHT = 1.62f;
constexpr float AIM_STRENGTH = 0.35f;
constexpr float RAD_TO_DEG = 57.2957795f;

float distanceSquared(const Vec3& a, const Vec3& b) {
    const float dx = a.x - b.x;
    const float dy = a.y - b.y;
    const float dz = a.z - b.z;
    return dx * dx + dy * dy + dz * dz;
}

float angleDifference(float target, float current) {
    return std::remainder(target - current, 360.0f);
}

Rotation aimToward(const Frame& frame, const Player& player) {
    const float dx = player.position.x - frame.eyePosition.x;
    const float dy = player.position.y + HEAD_HEIGHT - frame.eyePosition.y;
    const float dz = player.position.z - frame.eyePosition.z;
    const float horizontal = std::hypot(dx, dz);
    const float yaw = horizontal > 0.0001f ? std::atan2(-dx, dz) * RAD_TO_DEG : frame.yaw;
    const float pitch = -std::atan2(dy, horizontal) * RAD_TO_DEG;
    return {frame.yaw + angleDifference(yaw, frame.yaw) * AIM_STRENGTH,
            frame.pitch + (pitch - frame.pitch) * AIM_STRENGTH};
}

}

bool Client::enabled(Module module) const {
    switch (module) {
        case Module::AutoSprint: return autoSprint_;
        case Module::KillAura: return killAura_;
        case Module::Esp: return esp_;
        case Module::AimAssist: return aimAssist_;
    }
    return false;
}

void Client::toggle(Module module) {
    switch (module) {
        case Module::AutoSprint: autoSprint_ = !autoSprint_; break;
        case Module::KillAura:
            killAura_ = !killAura_;
            lastAttackTick_.reset();
            break;
        case Module::Esp: esp_ = !esp_; break;
        case Module::AimAssist: aimAssist_ = !aimAssist_; break;
    }
}

Actions Client::onPlayerTick(const Frame& frame) {
    Actions actions;
    if (!frame.alive) return actions;

    actions.sprint = autoSprint_ && frame.movingForward && frame.canSprint;
    const Player* auraTarget = nullptr;
    const Player* aimTarget = nullptr;
    float auraDistance = AURA_RANGE * AURA_RANGE;
    float aimDistance = ESP_RANGE * ESP_RANGE;

    for (const Player& player : frame.players) {
        if (!player.alive || !player.sameDimension || player.id == frame.localId) continue;
        const float distance = distanceSquared(frame.position, player.position);
        if (killAura_ && distance <= auraDistance) {
            auraDistance = distance;
            auraTarget = &player;
        }
        if (aimAssist_ && distance <= aimDistance) {
            aimDistance = distance;
            aimTarget = &player;
        }
        if (esp_ && distance <= ESP_RANGE * ESP_RANGE) {
            Vec3 head = player.position;
            head.y += HEAD_HEIGHT;
            actions.tracers.push_back({player.id, player.name, frame.eyePosition,
                                       head, std::sqrt(distance)});
        }
    }

    if (auraTarget && (!lastAttackTick_ || frame.tick < *lastAttackTick_ ||
                       frame.tick - *lastAttackTick_ >= ATTACK_INTERVAL_TICKS)) {
        actions.attackTarget = auraTarget->id;
        lastAttackTick_ = frame.tick;
    }
    if (aimTarget) actions.aim = aimToward(frame, *aimTarget);
    return actions;
}

std::string Client::renderGui() const {
    std::ostringstream out;
    out << "\n=== " << NAME << " | Modules ===\n"
        << "1  Auto Sprint  [" << (autoSprint_ ? "ON" : "OFF") << "]\n"
        << "2  Kill Aura   [" << (killAura_ ? "ON" : "OFF") << "]  <= 4 blocks\n"
        << "3  ESP         [" << (esp_ ? "ON" : "OFF") << "]  <= 100 blocks\n"
        << "4  Aim Assist  [" << (aimAssist_ ? "ON" : "OFF") << "]\n"
        << "t  Simulate one game tick | q  Quit\n> ";
    return out.str();
}

}
