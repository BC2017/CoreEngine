# Milestones

This file tracks the current foundational branch stack. Each milestone branch is
cumulative: later branches include earlier milestone work.

## Milestone 1: Core Runtime

Branch: `codex/milestone-1-core-runtime`

Foundation delivered:

- Runtime startup configuration.
- Logging memory sink.
- Assertion/check result primitive.
- Fixed-step clock.
- Virtual file roots.
- Typed handles.
- Deterministic job queue.
- Unit and integration tests.

## Milestone 2: Build, Dependency, And Tooling Policy

Branch: `codex/milestone-2-build-tooling`

Foundation delivered:

- CMake presets.
- GitHub Actions CI.
- Formatting policy.
- Dependency/submodule policy.

## Milestone 3: Render Hardware Interface

Branch: `codex/milestone-3-rhi-foundation`

Foundation delivered:

- API-neutral resource descriptions.
- RHI validation helpers.
- Backend capability and availability contracts.

## Milestone 0.5A: DirectX 12 Triangle

Branch: `codex/dx12-triangle-milestone`

Foundation delivered:

- Win32 sandbox window.
- Launch-time DX12 backend execution.
- DX12 device, swapchain, command queue, render target, root signature, pipeline,
  vertex buffer, fence, and frame submission.
- Bounded `--frames` mode for automation.
- GPU smoke test that renders one DX12 triangle frame.

Note: the bootstrap DX12 path uses embedded HLSL-style shader source compiled
through `D3DCompile`. Shared Slang compilation remains the planned shader path
for the dual-backend milestone.

## Milestone 0.5B: Vulkan Triangle

Branch: `codex/vulkan-triangle-milestone`

Foundation delivered:

- Vulkan SDK build integration.
- GLSL bootstrap shaders compiled to SPIR-V with `glslc`.
- Vulkan instance, Win32 surface, physical device selection, logical device,
  swapchain, render pass, graphics pipeline, framebuffers, command buffer,
  semaphores, fence, and frame submission.
- Bounded `--frames` mode for Vulkan automation.
- GPU smoke test that renders one Vulkan triangle frame.

Note: GLSL/SPIR-V is used only for the Vulkan bootstrap path. Shared Slang
compilation remains the planned shader path for production rendering.

## Runtime Debug Overlay

Branch: `codex/runtime-debug-shader-foundation`

Foundation delivered:

- Dear ImGui submodule.
- Shared runtime debug overlay data model and panel.
- Win32 ImGui input forwarding.
- DX12 and Vulkan overlay rendering in the sandbox renderer.
- CI submodule checkout.

Next target:

- Completed by `codex/slang-shader-pipeline`.

## Shared Slang Shader Pipeline

Branch: `codex/slang-shader-pipeline`

Foundation delivered:

- Single Slang mesh shader source.
- Build-time Slang compilation to DXIL for DirectX 12.
- Build-time Slang compilation to SPIR-V for Vulkan.
- DX12 runtime loading of compiled DXIL instead of embedded `D3DCompile`
  source.
- DX12 and Vulkan shader paths started from shared source and now feed the
  sandbox mesh renderer.

Next target:

- Start the next RHI slice by moving shader modules, pipeline descriptions, and
  backend capability reporting behind stable public interfaces before renderer
  features grow.

## RHI Mesh Pipeline Foundation

Branch: `codex/rhi-mesh-pipeline-foundation`

Foundation delivered:

- CPU-side RHI graphics pipeline descriptor and validation contract.
- Shared Slang mesh shader source compiled to DXIL and SPIR-V.
- DX12 indexed cube path with vertex buffer, index buffer, depth buffer, and
  depth-tested draw submission.
- Vulkan indexed cube path with vertex buffer, index buffer, depth image,
  depth render pass attachment, and depth-tested draw submission.
- GPU smoke tests now exercise an indexed mesh frame instead of only a
  shader-generated triangle.
- DX12 and Vulkan sandbox renderer files/APIs use neutral names instead of
  triangle-era names.

Next target:

- Establish real camera/view-projection data flow through backend constant
  resources before adding scene transforms, materials, and loaded assets.

## Camera Constant Buffer Foundation

Branch: `codex/camera-constant-buffer-foundation`

Foundation delivered:

- Minimal engine math library with row-vector, row-major matrix helpers for the
  engine's left-handed world convention.
- Sandbox camera helper that builds a shared view-projection matrix from the
  active window aspect ratio.
- Shared Slang mesh shader now consumes `FrameConstants` instead of hardcoded
  projection math.
- DX12 path binds the camera matrix through a root CBV.
- Vulkan path binds the same camera matrix through descriptor set 0, binding 0
  as a uniform buffer.
- Unit tests cover perspective depth mapping, look-at behavior, and sandbox
  camera visibility.

Next target:

- Move command recording and render submission behind backend-neutral RHI
  interfaces instead of calling sandbox backend functions directly.

## RHI Render Resource Foundation

Branch: `codex/rhi-render-resource-foundation`

Foundation delivered:

- Public RHI buffer, texture, and graphics pipeline handle aliases using the
  engine typed handle primitive.
- Public `DrawIndexedDesc` and validation for indexed mesh submission.
- Shared sandbox cube mesh data moved out of DX12/Vulkan backend files.
- Shared sandbox mesh pipeline and draw descriptors consumed by both backend
  renderers.
- DX12 and Vulkan sandbox paths now validate shared pipeline/draw contracts and
  use descriptor draw counts for indexed submission.
- Unit tests cover indexed draw validation and shared sandbox mesh contracts.

Next target:

- Wrap backend frame execution behind a shared renderer-facing submission
  interface, keeping DX12/Vulkan command emission private to backend modules.

## RHI Command Contract Foundation

Branch: `codex/rhi-command-contracts`

Foundation delivered:

- Backend-neutral command list descriptors and graphics render pass
  descriptors.
- Color/depth attachment validation for render pass setup.
- Command sequence recorder that validates begin/end ordering, active render
  pass requirements, pipeline binding, and indexed draw submission.
- Public `ICommandList` contract expanded beyond placeholder `Begin/End`.
- Shared sandbox frame command validation consumed before DX12 and Vulkan
  backend command recording.
- Unit tests cover valid graphics command recording and invalid sequencing.

Next target:

- Start scene-level submission by introducing entities/components for camera,
  transform, and mesh instances that can feed the renderer-facing frame path.

## Renderer Frame Submission Foundation

Branch: `codex/renderer-frame-submission`

Foundation delivered:

- Renderer-facing sandbox frame submission API.
- Backend-specific sandbox window titles centralized behind renderer code.
- Sandbox executable no longer creates renderer windows or calls DX12/Vulkan
  backend renderer functions directly.
- GPU smoke tests exercise the renderer-facing frame submission path for both
  DX12 and Vulkan.
- Unit test coverage for backend-specific sandbox frame metadata.

Next target:

- Introduce a minimal scene/ECS slice with transform, camera, and mesh instance
  data feeding the renderer submission path.

## Milestone 4: Frame Graph

Branch: `codex/milestone-4-frame-graph`

Foundation delivered:

- Pass/resource declaration.
- Dependency sorting.
- Invalid graph detection.

## Milestone 5: Asset Pipeline

Branch: `codex/milestone-5-asset-pipeline`

Foundation delivered:

- Asset IDs.
- Asset registry.
- Dependency graph.
- Cooked asset metadata.
