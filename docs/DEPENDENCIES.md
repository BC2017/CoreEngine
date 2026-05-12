# Dependency Policy

Dependencies are added as git submodules under `external/` only when milestone
code needs them. Planned dependencies that are not yet used stay documented
rather than vendored.

## Active Submodules

- `external/imgui`: Dear ImGui runtime/debug tooling and future editor UI.

## Active SDK Tools

- Vulkan SDK: Vulkan headers/libraries and `slangc` for compiling shared Slang
  shader source to DXIL and SPIR-V. CI pins 1.4.309.0 because that GitHub
  Actions SDK layout is known-compatible with CMake Vulkan discovery.

## Planned Baseline

- EnTT: ECS.
- Dear ImGui: active submodule for runtime/debug tooling first, full editor
  later.
- Slang: currently consumed through the Vulkan SDK compiler. A source submodule
  remains deferred until engine-owned compiler integration or reflection
  libraries require it.
- Jolt Physics: physics backend.
- miniaudio: audio backend.
- Lua 5.4 and sol2: first scripting backend.
- KTX-Software or Basis Universal: texture processing.

## Rules

- Prefer source submodules over package managers for project-owned builds.
- Keep backend SDKs such as the Windows SDK, DirectX tooling, and Vulkan SDK as
  system prerequisites unless there is a specific reason to vendor a component.
- Do not add unused submodules.
- Pin submodules to known commits.
- Document any dependency-specific build flags in this file before enabling
  them in CMake.

## Commands

Clone with submodules:

```powershell
git clone --recurse-submodules https://github.com/BC2017/CoreEngine.git
```

Initialize submodules in an existing clone:

```powershell
git submodule update --init --recursive
```
