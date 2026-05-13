# Engine Source Of Truth

Status: Draft 2  
Owner: Engine team  
Last updated: 2026-05-12

## 1. Purpose

This document is the source of truth for the engine architecture, development
sequence, feature expectations, and test strategy. Future implementation work
should either follow this document or update it intentionally before changing
direction.

The engine is a Windows-only native C++ 3D game engine with two first-class
rendering backends: DirectX 12 and Vulkan. Both backends are reached through a
shared render hardware interface. It will use ImGui for runtime/debug tooling
first, then editor UI later. The engine should be designed for high-fidelity
real-time rendering, including hardware ray tracing, path tracing, physically
based materials, a particle system, and a regression-resistant test suite.

## 2. Non-Negotiable Engineering Goals

- C++20 minimum, with a path to C++23 when compiler support and dependencies
  justify it.
- Explicit ownership and lifetime management for GPU, asset, scene, and ECS
  resources.
- Rendering backends must be isolated behind stable engine interfaces.
- DirectX 12 and Vulkan must support equivalent engine features unless a
  documented platform limitation prevents parity.
- The renderer must be debuggable with validation layers, GPU markers, capture
  tooling, and deterministic frame graph logging.
- Tests must be part of normal development, not a late addition.
- All systems must expose enough diagnostics to make bugs reproducible.
- Editor and tools must use engine APIs rather than bypassing runtime systems.
- Runtime code must remain usable without the editor.
- Windows is the only supported platform until the source of truth explicitly
  reopens platform support.
- Backend selection must be easy and visible from the first rendering
  milestone.

## 3. Locked Decisions

These decisions are active project policy until this document is intentionally
revised.

- Target operating system: Windows only.
- Platform layer: Win32 native windowing and input.
- Graphics APIs: DirectX 12 and Vulkan are both production targets behind the
  RHI.
- Backend switching: launch-time selection through `--renderer dx12` or
  `--renderer vulkan` for the first milestone. Runtime hot switching is
  deferred.
- UI: ImGui is for editor and debug tooling, not player-facing shipped UI.
- Tooling priority: runtime/debug tooling comes before a full editor.
- Build system: CMake is the default project generator.
- Package management: git submodules under `external/`.
- Shader language: Slang using HLSL-style source. DirectX 12 uses DXIL output;
  Vulkan uses SPIR-V output.
- ECS: EnTT through a git submodule.
- Scripting: Lua 5.4 plus sol2 is the first planned scripting backend. The
  engine scripting API must stay backend-neutral enough to add C# or another
  language later.
- Physics: Jolt Physics through a git submodule.
- Audio: miniaudio through a git submodule.
- Asset source formats: glTF 2.0 for mesh/material/scene import, KTX2/BasisU
  for runtime texture delivery, OpenEXR/HDR for environment and reference
  images, WAV/OGG for audio, and an engine-owned cooked asset format for
  runtime.
- Coordinate system: meters, Y-up, left-handed world convention. Vulkan
  clip-space differences are handled inside the RHI/backend layer.
- Minimum high-fidelity GPU target: NVIDIA RTX 4060. Lower hardware may run
  basic paths but is not the feature baseline.
- Path tracing: both real-time and offline/editor modes are in scope.
- Plugin/mod support: deferred.
- Networking: out of scope until explicitly reopened.
- License: MIT.
- Threading model: engine-managed job system with deterministic shutdown.

## 3.1 Decision Log

- 2026-05-12: Locked Windows-only scope, Win32 platform layer, and RTX 4060
  high-fidelity GPU target.
- 2026-05-12: Locked RHI-first DirectX 12 and Vulkan development with
  launch-time backend selection.
- 2026-05-12: Chose Slang with HLSL-style source for shared shader authoring,
  DXIL output, SPIR-V output, and reflection.
- 2026-05-12: Chose EnTT for ECS, Lua 5.4 plus sol2 for scripting, Jolt for
  physics, and miniaudio for audio.
- 2026-05-12: Chose glTF 2.0, KTX2/BasisU, OpenEXR/HDR, WAV/OGG, and cooked
  engine assets as the asset baseline.
- 2026-05-12: Deferred networking and plugin/mod support.
- 2026-05-12: Implemented the first shared Slang shader path: one triangle
  shader source compiles to DXIL for DirectX 12 and SPIR-V for Vulkan.
- 2026-05-12: Promoted the visible GPU smoke path from a shader-generated
  triangle to an indexed colored cube with vertex buffers, index buffers,
  perspective projection, and depth testing in both backends.
- 2026-05-12: Renamed remaining triangle-era backend sandbox renderer files and
  APIs to neutral sandbox renderer names.
- 2026-05-12: Moved sandbox camera projection out of shader literals and into a
  shared CPU-built view-projection matrix bound through DX12 constant buffers
  and Vulkan uniform descriptors.

## 4. Architecture Principles

### 4.1 Layering

The engine should be organized into clear layers:

1. Platform layer: Win32 windows, input, files, timers, dynamic libraries.
2. Core layer: memory, logging, assertions, diagnostics, jobs, containers,
   handles, reflection primitives, serialization primitives.
3. Asset layer: import, cook, cache, dependency tracking, hot reload.
4. Render hardware interface: API-independent rendering contracts.
5. Backend layer: DirectX 12 and Vulkan implementations.
6. Renderer layer: frame graph, materials, lighting, visibility, ray tracing,
   post processing, debug rendering.
7. Scene/runtime layer: EnTT ECS, components, transforms, cameras, lights,
   physics integration, particles, animation, audio integration.
8. Tools/editor layer: ImGui editor, inspectors, profilers, asset browser,
   render debugging panels.
9. Game layer: sample projects and user game modules.

Dependencies must generally point downward. The renderer may depend on core,
platform, assets, and RHI. The editor may depend on nearly everything. Runtime
systems must not depend on editor-only code.

### 4.2 Backend Independence

The engine must not leak DirectX 12 or Vulkan types through high-level runtime
or renderer APIs. API-specific objects belong in backend modules.

Required backend boundary concepts:

- Device and adapter selection.
- Queues and command submission.
- Command lists or command buffers.
- Buffers, textures, samplers, acceleration structures.
- Resource states and barriers.
- Descriptor/resource binding.
- Pipeline state objects.
- Shader modules and shader reflection.
- Render pass or dynamic rendering abstraction.
- Query heaps, timestamps, and pipeline statistics.
- Swapchains and presentation.
- Debug names and GPU markers.

### 4.3 Data Ownership

All long-lived engine objects need explicit owners. Borrowed references must be
obvious in type names, comments, or API shape.

Expected patterns:

- Handles for renderer resources exposed outside the renderer.
- RAII wrappers around API objects.
- Generational handles for ECS and asset resources.
- Immutable cooked asset blobs where possible.
- Versioned resource records for hot reload.
- No raw owning pointers in public APIs.

### 4.4 Determinism

The engine should make deterministic behavior possible even when the final game
does not require strict determinism.

Required support:

- Seeded random streams.
- Stable entity IDs for serialization.
- Deterministic asset cooking for identical inputs.
- Testable fixed-step simulation path.
- Explicit frame timing source for tests.

## 5. Development Roadmap

### Phase 0: Repository Foundation

Goals:

- Establish CMake build.
- Establish source tree.
- Establish test harness and CI-ready commands.
- Add style, formatting, and documentation conventions.
- Add engine source-of-truth document.
- Add MIT license.
- Document git submodule dependency policy.

Exit criteria:

- `cmake -S . -B build` succeeds.
- `cmake --build build` succeeds.
- `ctest --test-dir build -C Debug --output-on-failure` succeeds with Visual
  Studio generators.
- Test executable can distinguish unit and integration suites.
- Documentation records locked architectural decisions.

### Phase 0.5: Dual-Backend Triangle Milestone

Goals:

- Create a Win32 sandbox window.
- Add launch-time renderer backend selection with `--renderer dx12` and
  `--renderer vulkan`.
- Add RHI interfaces before backend code grows.
- Compile one Slang/HLSL-style triangle shader for both DXIL and SPIR-V.
- Render the same triangle through DirectX 12 and Vulkan.
- Add ImGui runtime/debug overlay showing active backend, adapter name,
  validation state, frame time, and capabilities.

Exit criteria:

- `HFEngineSandbox.exe --renderer dx12` opens a window and renders a triangle.
- `HFEngineSandbox.exe --renderer vulkan` opens a window and renders the same
  triangle.
- Backend-specific types do not leak into runtime or renderer public APIs.
- Backend selection requires restart; hot switching is documented as deferred.
- GPU smoke tests cover device creation, swapchain creation, and triangle frame
  submission for both backends, skipping clearly when unavailable.

### Phase 1: Core Runtime

Systems:

- Logging.
- Assertions and fatal error handling.
- Memory allocation interfaces.
- File system abstraction.
- Time and frame clock.
- Math library decision and integration.
- UUID or stable ID generation.
- Job system.
- Configuration system.
- Platform window creation.
- Input events.

Features:

- Structured logs with categories and severity.
- Debug and release assertion behavior.
- Optional allocation tracking in debug builds.
- Virtual file roots for assets, cache, user settings, and project files.
- Monotonic engine clock and fixed simulation clock.
- Thread pool with named worker threads.
- Work stealing or dependency-based jobs after the simple implementation.
- Graceful startup and shutdown sequencing.

Tests:

- Unit tests for handles, IDs, clocks, configuration parsing, and math helpers.
- Integration tests for startup/shutdown and virtual file roots.
- Stress tests for job scheduling and shutdown.
- Unit tests for command-line parsing and backend selection.

### Phase 2: Build, Dependency, And Tooling Policy

Systems:

- Dependency management.
- Compiler warnings.
- Static analysis.
- Formatting.
- Optional sanitizers where supported.
- CI pipeline.

Features:

- One command to configure, build, and test.
- Warning-as-error option for CI.
- Presets for Debug, RelWithDebInfo, and Release.
- Validation build preset with tests and diagnostics enabled.
- Dependency lock file or version manifest.
- Git submodule update instructions.

Tests:

- CI must build at least Debug and Release-like configurations.
- CI must run all non-GPU unit tests.
- GPU tests should run on machines with known adapters and be marked
  separately from CPU-only tests.

### Phase 3: Render Hardware Interface

Systems:

- RHI API definitions.
- Resource creation.
- Command recording.
- Synchronization.
- Descriptor binding model.
- Shader module abstraction.
- Pipeline abstraction.
- Swapchain abstraction.
- Debug layer integration.

Features:

- API-independent buffer and texture descriptors.
- Explicit resource usage states.
- Upload and readback paths.
- Command allocator recycling.
- Multi-frame resource lifetime management.
- Fence-based CPU/GPU synchronization.
- Debug names on all GPU objects.
- GPU timestamp queries.
- Backend capability reporting.
- Launch-time backend factory.

DirectX 12 backend:

- Adapter enumeration.
- Device creation.
- Debug layer and DRED support.
- Command queues.
- Descriptor heaps.
- Root signatures.
- Pipeline state objects.
- Resource barriers.
- DXGI swapchain.
- DXC shader compilation path.
- Slang DXIL target integration.

Vulkan backend:

- Instance creation.
- Validation layers.
- Physical device selection.
- Logical device creation.
- Queues.
- VMA or equivalent allocator decision.
- Descriptor sets or descriptor indexing.
- Pipeline layouts.
- Render pass or dynamic rendering path.
- Vulkan swapchain.
- SPIR-V shader path.
- Slang SPIR-V target integration.

Tests:

- CPU-only unit tests for descriptor validation and resource state tracking.
- Backend smoke tests for device creation where hardware is available.
- Integration tests for swapchain creation and one clear-color frame.
- GPU smoke tests for triangle submission on each backend.
- GPU capture markers verified manually at first, automated later if practical.

### Phase 4: Frame Graph

Systems:

- Render pass declaration.
- Resource lifetime inference.
- Barrier generation.
- Async compute scheduling foundation.
- Transient resource allocator.
- Render graph visualization.

Features:

- Pass inputs and outputs are declared before execution.
- Graph compiler inserts barriers and detects invalid access.
- Transient textures and buffers are aliased when safe.
- Passes can be enabled, disabled, or inspected from tools.
- Debug output includes pass order and resource lifetime.

Tests:

- Unit tests for graph dependency sorting.
- Unit tests for invalid graph detection.
- Unit tests for barrier generation.
- Integration tests that execute a small graph on each backend.

### Phase 5: Asset Pipeline

Systems:

- Source asset import.
- Cooked asset format.
- Asset registry.
- Dependency tracking.
- Background asset loading.
- Hot reload.
- Shader compilation pipeline.

Features:

- Stable asset IDs.
- Content hashing.
- Incremental cooking.
- Deterministic cooked outputs.
- Versioned asset schemas.
- Asset dependency graph.
- Editor-visible import diagnostics.
- Runtime asset streaming hooks.

Tests:

- Unit tests for asset IDs, hashing, and dependency graph behavior.
- Golden-file tests for deterministic cooking.
- Integration tests that import a minimal mesh, material, and texture.
- Hot reload tests using temporary project directories.

### Phase 6: Scene And Entity Model

Systems:

- Entity/component model.
- Transform hierarchy.
- Cameras.
- Lights.
- Mesh render components.
- Serialization.
- Scene streaming foundation.

Features:

- Stable entity IDs.
- Parent/child transforms.
- Local and world transform caching.
- Dirty propagation.
- Serialization to a versioned format.
- Scene loading without editor dependencies.
- Runtime creation/destruction APIs.

Tests:

- Unit tests for entity lifetime and generational handles.
- Unit tests for transform propagation.
- Serialization round-trip tests.
- Integration test that loads a scene and submits visible objects to renderer.

### Phase 7: Material System

Systems:

- Material asset model.
- Shader permutation management.
- Material parameter layout.
- Texture binding.
- Render pipeline classification.
- Material editor UI.

Features:

- Physically based material core.
- Base color, metallic, roughness, normal, emissive, opacity.
- Clearcoat, sheen, anisotropy, transmission, subsurface as planned extensions.
- Material instances with overrides.
- Texture channel packing policy.
- Bindless or descriptor-indexed texture strategy where supported.
- Shader reflection-driven parameter validation.
- Material preview scene.
- Fallback material for broken assets.

Tests:

- Unit tests for parameter packing and default values.
- Unit tests for shader permutation keys.
- Asset tests for material serialization and version migration.
- Integration tests rendering reference material spheres.
- Image comparison tests once deterministic rendering support exists.

### Phase 8: Forward, Deferred, And Hybrid Rendering

Systems:

- Visibility and culling.
- Mesh submission.
- Depth prepass.
- G-buffer.
- Lighting.
- Shadow maps.
- Sky and environment lighting.
- Post processing.
- Debug rendering.

Features:

- Frustum culling.
- Optional occlusion culling.
- Static and skinned mesh paths.
- Directional, point, and spot lights.
- Cascaded shadow maps.
- Image-based lighting.
- Tone mapping.
- Bloom.
- Temporal anti-aliasing.
- Debug views for depth, normals, roughness, metallic, motion vectors, lighting,
  and overdraw.

Tests:

- Unit tests for culling math.
- Unit tests for light bounds.
- Integration tests for simple scenes.
- Backend parity tests for equivalent render pass outputs.
- Image comparison tests with tolerances per backend.

### Phase 9: Hardware Ray Tracing

Systems:

- Acceleration structure abstraction.
- Bottom-level acceleration structures.
- Top-level acceleration structures.
- Shader binding table abstraction.
- Ray tracing pipeline abstraction.
- Ray traced shadows, reflections, and ambient occlusion.

Features:

- Capability detection for DXR and Vulkan ray tracing.
- Static and dynamic geometry support.
- Instance masks.
- Compaction support where available.
- Update versus rebuild policy.
- Debug views for acceleration structure contents.
- Fallback paths when ray tracing is unavailable.

Tests:

- CPU tests for acceleration structure build descriptions.
- Backend capability tests.
- Integration tests for a ray traced triangle or simple Cornell-style scene.
- Image comparison tests for deterministic ray traced outputs.

### Phase 10: Path Tracing

Systems:

- Path tracing render mode.
- Sampling.
- Material evaluation.
- Environment sampling.
- Denoising integration decision.
- Progressive accumulation.
- Ground-truth reference scenes.

Features:

- Progressive accumulation with reset on camera or scene changes.
- Multiple importance sampling.
- Physically based BRDF evaluation.
- Emissive geometry support.
- Environment map importance sampling.
- Russian roulette termination.
- Optional ReSTIR exploration only after a stable baseline.
- Saveable reference frames.

Tests:

- Unit tests for sampling distributions.
- Unit tests for BRDF energy sanity checks.
- Statistical tests with tolerances.
- Integration tests for reference scenes.
- Image comparison tests with fixed seeds and sample counts.

### Phase 11: Particle System

Systems:

- CPU particle simulation.
- GPU particle simulation.
- Emitters.
- Particle renderers.
- Sorting.
- Collision hooks.
- Editor authoring UI.

Features:

- Spawn rate, burst, and event-driven emission.
- Lifetime, velocity, acceleration, drag, color, size, rotation curves.
- Sprite, mesh, ribbon, and beam renderers.
- GPU simulation for high particle counts.
- Soft particles.
- Lit and unlit particle materials.
- Deterministic CPU simulation path for tests.
- Bounds calculation for culling.

Tests:

- Unit tests for emitter curves and spawn counts.
- Unit tests for deterministic simulation with fixed seeds.
- Integration tests for CPU and GPU particle scenes.
- Performance tests for high particle counts.

### Phase 12: Animation

Systems:

- Skeletons.
- Animation clips.
- Blend trees.
- State machines.
- Skinning.
- Retargeting decision.

Features:

- CPU skinning for validation and fallback.
- GPU skinning for runtime.
- Animation events.
- Root motion policy.
- Editor timeline preview.

Tests:

- Unit tests for interpolation.
- Unit tests for hierarchy evaluation.
- Golden tests for sampled poses.
- Integration tests for skinned mesh rendering.

### Phase 13: Physics Integration

Systems:

- Physics backend selection.
- Collision shapes.
- Rigid bodies.
- Character controller.
- Scene queries.
- Debug visualization.

Features:

- Deterministic fixed-step simulation path where possible.
- Physics material properties.
- Raycast, sweep, and overlap queries.
- Editor collider authoring.

Tests:

- Unit tests around wrapper conversions and filters.
- Integration tests for fixed-step scenes.
- Regression scenes for collision and stacking.

### Phase 14: Audio

Systems:

- Audio device.
- Sound assets.
- Mixing.
- 3D spatial audio.
- Streaming.

Features:

- One-shot and looping sounds.
- Listener component.
- Attenuation curves.
- Debug audio panel.

Tests:

- Unit tests for attenuation and mixing math.
- Integration tests for loading and playing silent or generated buffers.

### Phase 15: Editor And ImGui Tools

Systems:

- Editor shell.
- Docking layout.
- Scene hierarchy.
- Inspector.
- Asset browser.
- Material editor.
- Renderer debug panels.
- Profiler views.
- Console.

Features:

- ImGui docking.
- Multi-viewport decision after platform stability.
- Editor commands with undo/redo.
- Property editing through reflection or explicit metadata.
- Project settings.
- Render graph viewer.
- GPU resource viewer.
- Shader permutation viewer.
- Particle editor.

Tests:

- Unit tests for editor command history.
- Serialization tests for editor settings.
- Integration tests for opening a project and loading a scene.
- Manual exploratory UI test checklist until UI automation is justified.

### Phase 16: Performance And Memory

Systems:

- CPU profiler.
- GPU profiler.
- Memory tracking.
- Frame capture support.
- Performance regression tests.

Features:

- Named CPU scopes.
- Named GPU scopes.
- Per-system memory accounting.
- Asset memory budgets.
- Render resource budget view.
- Performance captures tied to test scenes.

Tests:

- Microbenchmarks for critical containers and jobs.
- Performance smoke tests with thresholds on known machines.
- Memory leak tests for startup/shutdown and scene reload loops.

## 6. Core System Feature Contracts

### 6.1 Logging

Required features:

- Categories.
- Severity levels.
- Thread ID or thread name.
- Timestamp.
- File output.
- Console output.
- Editor console sink.
- Test sink.

### 6.2 Assertions

Required features:

- Debug break in development builds.
- Fatal error path for unrecoverable failures.
- Test-overridable behavior.
- Clear distinction between programmer errors and recoverable runtime errors.

### 6.3 Memory

Required features:

- Central allocation interface.
- Optional allocation tracking.
- Per-system tags.
- Leak reporting.
- GPU memory reporting through backend APIs.

### 6.4 Job System

Required features:

- Named jobs.
- Dependencies.
- Wait groups.
- Main-thread dispatch.
- Worker-thread shutdown.
- Test mode with deterministic execution.

### 6.5 File System

Required features:

- Virtual roots.
- Binary and text reads.
- Atomic writes where practical.
- Directory watching for tools.
- Path normalization.

## 7. Renderer Feature Contracts

### 7.1 Renderer Front End

Required features:

- Camera submission.
- Mesh submission.
- Light submission.
- Particle submission.
- Debug primitive submission.
- Frame settings.
- Render target selection.

### 7.2 Renderer Back End

Required features:

- Frame graph execution.
- Resource lifetime tracking.
- Barrier generation.
- Descriptor management.
- Pipeline cache.
- Shader hot reload.
- GPU markers.
- Timestamp queries.

### 7.3 Shader System

Required features:

- Slang with HLSL-style source.
- DXIL path for DirectX 12.
- SPIR-V path for Vulkan.
- Shared shader source strategy.
- Shader reflection.
- Permutation keys.
- Shader cache.
- Debug and optimized compile modes.

Policy:

- Shader source should be shared unless a backend-specific limitation is
  documented.
- Slang reflection is the preferred source of truth for binding and parameter
  validation.
- Backend-specific shader code is allowed only behind an explicit renderer
  decision record.

## 8. Material System Contract

Required features:

- Material asset schema.
- Material instance schema.
- Runtime material handle.
- Reflection-validated parameter layout.
- Texture binding validation.
- Default/fallback material.
- Physically based shading model.
- Editor preview.
- Version migration.

Initial material model:

- Base color.
- Metallic.
- Roughness.
- Normal.
- Ambient occlusion.
- Emissive color and intensity.
- Opacity.
- Alpha mode: opaque, masked, blended.
- Two-sided flag.

Advanced material model:

- Clearcoat.
- Sheen.
- Anisotropy.
- Transmission.
- Subsurface.
- Thin surface.
- Displacement or parallax decision.

## 9. Particle System Contract

Required features:

- Emitter asset.
- Runtime particle system component.
- CPU simulation path.
- GPU simulation path.
- Deterministic seeded mode.
- Curves for scalar and color values.
- Bounds generation.
- Sorting policy.
- Renderer integration.
- Editor preview and scrubbing.

Renderer modes:

- Billboard sprites.
- Stretched billboards.
- Mesh particles.
- Ribbons.
- Beams.

## 10. Testing Strategy

### 10.1 Test Categories

Unit tests:

- Fast.
- Deterministic.
- CPU-only by default.
- No GPU, window, filesystem writes, or network unless explicitly marked.
- Run on every local build and every CI build.

Integration tests:

- Exercise multiple engine systems together.
- May use temporary files.
- May create windows or GPU devices only when explicitly marked.
- Run in CI where environment allows.

GPU tests:

- Marked separately from CPU integration tests.
- Include backend name and adapter requirements in output.
- Must skip gracefully when required features are unavailable.

Performance tests:

- Not mixed with correctness tests.
- Use stable scenes and known thresholds.
- Track trends rather than relying only on hard pass/fail numbers.

Golden/image tests:

- Use fixed seeds, fixed camera transforms, fixed resolution, and fixed sample
  counts.
- Store references with metadata.
- Use tolerance thresholds by backend and platform.
- Record failure images and diffs.

### 10.2 Test Infrastructure Requirements

The test system must support:

- Registering test cases by suite and name.
- Clear failure messages.
- Source location reporting.
- Unit and integration executables.
- CTest integration.
- Filtering tests locally.
- Labels for CI selection.
- Temporary directory utilities.
- Test doubles for logs, files, clocks, and renderer backends.
- GPU test skip mechanism.
- GPU smoke tests for DirectX 12 and Vulkan device/swapchain/triangle paths.
- Future migration path to Catch2, doctest, or GoogleTest if desired.

### 10.3 Regression Policy

- Every fixed bug should add a regression test unless technically impossible.
- If a bug cannot be covered by automation, add a manual test note or diagnostic
  assertion explaining why.
- Renderer regressions should prefer small deterministic scenes over large
  content-heavy scenes.
- Tests must avoid depending on test order.
- Tests must clean up resources they create.

### 10.4 CI Gates

Minimum initial gates:

- Configure.
- Build.
- Unit tests.
- CPU integration tests.

Future gates:

- Static analysis.
- Formatting check.
- GPU smoke tests on known hardware.
- Render image comparisons.
- Asset cooking determinism tests.
- Performance trend reporting.

## 11. Documentation Requirements

Each major system needs:

- Purpose.
- Public API boundary.
- Ownership rules.
- Threading rules.
- Serialization rules if applicable.
- Test strategy.
- Debugging and diagnostics.
- Known limitations.

Architecture decisions should be recorded as ADRs under `docs/adr/`.

## 12. Proposed Repository Layout

```text
cmake/
docs/
docs/adr/
external/
include/Engine/
src/Engine/
src/Engine/RHI/
src/Engine/RHI/DX12/
src/Engine/RHI/Vulkan/
src/Editor/
src/Runtime/
src/Sandbox/
tests/harness/
tests/unit/
tests/integration/
tools/
assets/
samples/
```

## 13. Immediate Next Steps

1. Move backend shader module and graphics pipeline creation behind the public
   RHI descriptors.
2. Add EnTT as a git submodule and establish ECS wrapper boundaries.
3. Add coding standards and formatting configuration.
4. Add CI formatting or static-analysis gates after style rules are locked.

## 14. Open Questions

These questions remain open and should be answered before their affected phases
begin:

1. What is the expected scale of worlds and scenes?
2. What serialization format should scene/project files use?
3. Should the engine use a custom reflection system, generated reflection, or a
   third-party reflection library?
4. What coding style and formatting rules should CI enforce?
5. Which CI provider should be used?
6. What is the first sample scene after the dual-backend triangle?
