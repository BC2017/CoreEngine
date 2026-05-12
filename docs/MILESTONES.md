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
- DX12 and Vulkan overlay rendering in the triangle sandbox.
- CI submodule checkout.

Next target:

- Completed by `codex/slang-shader-pipeline`.

## Shared Slang Shader Pipeline

Branch: `codex/slang-shader-pipeline`

Foundation delivered:

- Single Slang triangle shader source.
- Build-time Slang compilation to DXIL for DirectX 12.
- Build-time Slang compilation to SPIR-V for Vulkan.
- DX12 runtime loading of compiled DXIL instead of embedded `D3DCompile`
  source.
- DX12 and Vulkan triangle paths both draw from `SV_VertexID`, keeping the first
  visible shader path backend-neutral.

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

Next target:

- Rename the remaining triangle-era sandbox entrypoint files/APIs into neutral
  sandbox renderer names and start moving shader/pipeline creation behind the
  public RHI descriptors.

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
