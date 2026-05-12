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
