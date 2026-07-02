# External Overlay + Kernel Driver (Archived)

This is an old solo project I built a while back to learn Windows kernel driver development and DirectX overlay rendering. It's no longer maintained, I'm just uploading it here so the code exists somewhere and isn't lost. I'm not actively working on it anymore and have no plans to update it.

It was originally built against a specific game's memory layout, but that's not really the point of the project. The interesting part was everything underneath: getting a kernel driver talking to a user-mode process, reading memory across privilege boundaries, and rendering an overlay on top of DirectX 11 without hooking the game's own render loop.

## What it actually is

- A **kernel-mode driver** written in C, built with the WDK, that reads memory from another process and exposes it to user mode over a shared memory region.
- A **user-mode overlay application** in C++ that talks to the driver, does the world-to-screen math, and draws over the screen using GDI composited on top of a D3D11 swapchain.
- Some **shared code** (structs, offsets, math, IPC definitions) used by both sides.

The offsets are years out of date (the offset dumper still kinda works so i updated them recently, still wont work though!) at this point and tied to a game version that no longer exists, so none of the actual read-memory functionality works anymore. What's left is really just a reference for the plumbing: driver ↔ user-mode communication, shared memory, and the rendering pipeline.

## Project layout

```
Project/
├── Deploy/                 # Built executables and driver
│   ├── config.json         # Overlay configuration
│   ├── Driver.sys          # Kernel driver
│   └── Overlay.exe         # Overlay executable
│
├── Driver/                 # Visual Studio driver project
│   ├── Driver.vcxproj
│   ├── Driver.inf
│   └── x64/Debug/          # Build output
│
├── DriverSRC/               # Driver source
│   ├── main.c               # Driver entry point
│   ├── game.c                # Target process memory reading
│   ├── memory.c              # Kernel memory read/write functions
│   ├── shared_memory.c        # Shared memory management
│   ├── w2s.c                   # World-to-screen math
│   ├── offsets.h                # Memory offsets (outdated)
│   └── structures.h              # Data structures
│
├── Overlay/                 # Overlay application
│   ├── main.cpp              # Entry point
│   ├── overlay.cpp            # Overlay window management
│   ├── renderer.cpp             # DirectX rendering
│   ├── esp.cpp                   # Draw logic
│   ├── client.cpp                  # Driver communication
│   └── pch.h                        # Precompiled headers
│
├── Shared/                   # Shared between driver + overlay
│   ├── Structures/
│   ├── Offsets/                # Outdated
│   ├── Math/
│   └── Communication/            # IPC definitions
│
├── Scripts/                    # Build helper scripts
│   ├── build.bat
│   ├── install.bat
│   └── run.bat
│
└── x64/Debug/                     # Final build output
    ├── Driver.sys
    ├── Overlay.exe
    ├── LoadDriver.bat
    └── UnloadDriver.bat
```

## Building it

You'll need Visual Studio 2022 with:
- Desktop development with C++
- Windows Driver Kit (WDK) 10.0.26100.0
- Windows SDK 10.0.26100.0
- Spectre-mitigated libraries (required for the driver project)

Build order matters: `Shared` first, then `Overlay`, then `Driver`:

```cmd
msbuild Shared\Shared.vcxproj /p:Configuration=Debug /p:Platform=x64
msbuild Overlay\Overlay.vcxproj /p:Configuration=Debug /p:Platform=x64
msbuild Driver\Driver.vcxproj /p:Configuration=Debug /p:Platform=x64
```

## The driver unload issue

This is the main thing worth flagging if anyone actually loads this driver on real hardware: **unloading it can BSOD your machine.** I never got around to properly cleaning up the kernel resources it allocates (shared memory mappings, hooked structures, etc.) before unload, so tearing it down mid-session is unstable. If you do unload it, just expect a crash and a reboot.

Test signing mode also needs to be enabled to load an unsigned driver like this at all, which is a whole extra footgun on its own if you're not used to working with kernel drivers.

## What doesn't work anymore

| Feature | Status |
|---|---|
| ESP-style overlays (boxes, distance, names) | ❌ outdated offsets |
| Health/status readouts | ❌ outdated offsets |
| Memory reading in general | ❌ outdated offsets |
| Crosshair overlay | ✅ still works |
| Menu / config UI | ✅ still works |
| Renderer (D3D11 + GDI composite) | ✅ still works, some flickering on certain setups |

## Known issues

- **BSOD on driver unload:** see above, this is the big one.
- **Outdated offsets:** the memory layout this targeted no longer matches any current build of the game it was written for.
- **Renderer flicker:** the GDI-over-D3D11 compositing approach flickers on some systems; never tracked down why.
- **Driver load requires test signing:** it's an unsigned/self-signed driver, so Windows needs to be in test mode to load it at all.

## Why I'm putting this up

Mostly for my own reference and because I put a decent amount of work into the kernel/user-mode IPC and the rendering side and didn't want it to just disappear. If you're digging through this for the driver architecture or the D3D11 overlay technique, that's the part that's actually still relevant. Everything game-specific is dead weight at this point.
