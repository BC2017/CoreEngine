# External Dependencies

Third-party dependencies are managed as git submodules under this directory.

Active submodules:

- `imgui`: Dear ImGui runtime/debug tooling and future editor UI.

Planned baseline:

- EnTT for ECS.
- Slang source/compiler integration if engine-owned reflection or compiler
  library use requires vendoring it. The current build consumes `slangc` from
  the Vulkan SDK.
- Jolt Physics for physics.
- miniaudio for audio.
- Lua 5.4 and sol2 for the first scripting backend.
- KTX-Software or Basis Universal tooling for KTX2/BasisU texture processing.

The Vulkan SDK, `slangc`, and Windows/DirectX SDK tooling are system
prerequisites rather than vendored source dependencies unless a future decision
changes that policy.
