# ZAINFUL

ZAINFUL is a small, dependency-free C++17 feature engine and a **Termux console
demo**. The demo uses simulated players. It does **not** connect to Minecraft,
draw an in-game menu, modify an installed APK, or provide a working cheat client.
The old repository claimed to inject into Minecraft, but contained only
Windows-specific placeholder code.

## Build in Termux

```sh
pkg install clang make
git clone https://github.com/WindShark-Z/ZAINFUL.git
cd ZAINFUL
make
./zainful
```

Press `1`–`4` to toggle modules and `t` to simulate a tick. Run `make test`
for the feature checks. On Linux, a C++17 compiler and `make` are sufficient.
The resulting program is a terminal executable, **not an Android APK**.

## Behavior implemented by the engine

| Module | Output produced from a game-state snapshot |
| --- | --- |
| Auto Sprint | Requests sprint while the player holds forward and can sprint. |
| Kill Aura | Selects the nearest live player in the same dimension within **4 blocks**, at most once every 10 ticks. |
| ESP | Returns tracer endpoints and names for live players within **100 blocks**. No lines are drawn by the console demo. |
| Aim Assist | Returns a smoothed rotation toward the nearest live player within 100 blocks. |

All four modules start off. The caller supplies the local player position,
eye position, yaw/pitch, forward movement, and visible player list in `Frame`,
then calls `Client::onPlayerTick`. The `Actions` output is a request to the
host: sprint, attack a player ID, set rotation, and draw tracers with names.
Ticks must increase monotonically within a session; resetting the tick number
also resets the aura cooldown. The host remains responsible for checking
server rules, line of sight, attack validity, rendering, and actual input.

## Making an Android Minecraft client

A working Pocket Edition client needs a separate Android host (for example,
a version-aware Bedrock packet proxy/launcher) that **actually reads game
state, applies `Actions` to gameplay, and renders an in-game menu and ESP**.
This repository does not include that host or Bedrock protocol handling.
Running a Termux program alone cannot read another Android app's private
memory or add controls to its screen. Copying these source files into an APK
also does not provide those capabilities.

A native Android host would have to build its native code for both
`armeabi-v7a` and `arm64-v8a` and test against the installed game's protocol
version. Whether Minecraft was installed from the Play Store does not tell
the client which ABI or protocol it uses. Compatibility with the latest
release and unofficial installations is **not verified** here.

If you want to build that integration, the missing pieces are an Android
launcher/proxy with Bedrock authentication and version negotiation, an adapter
from game/player packets to `Frame` and from `Actions` to valid game actions,
and an Android renderer/menu. A server-side behavior pack alone cannot
provide the requested client-side effects in arbitrary multiplayer servers:
Bedrock scripts run on the host/server, not inside the joining client's UI.

Reference architecture:
[LuminaClient](https://github.com/TheProjectLumina/LuminaClient) and
[WClient](https://github.com/RetrivedMods/WClient) use Android and packet
handling for this purpose. This code is independently written and does not
copy their code or branding.
