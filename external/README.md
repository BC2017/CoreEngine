# External Dependencies

Third-party dependencies are managed as git submodules under this directory.

Active submodules:

- `imgui`: Dear ImGui runtime/debug tooling and future editor UI.

Planned baseline:

- EnTT for ECS.
- Slang for HLSL-style shader authoring, DXIL output, SPIR-V output, and shader
  reflection.
- Jolt Physics for physics.
- miniaudio for audio.
- Lua 5.4 and sol2 for the first scripting backend.
- KTX-Software or Basis Universal tooling for KTX2/BasisU texture processing.

The Vulkan SDK and Windows/DirectX SDK tooling are system prerequisites rather
than vendored source dependencies unless a future decision changes that policy.
