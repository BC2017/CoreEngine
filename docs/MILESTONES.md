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

Foundation target:

- API-neutral resource descriptions.
- RHI validation helpers.
- Backend capability and availability contracts.

## Milestone 4: Frame Graph

Branch: `codex/milestone-4-frame-graph`

Foundation target:

- Pass/resource declaration.
- Dependency sorting.
- Invalid graph detection.

## Milestone 5: Asset Pipeline

Branch: `codex/milestone-5-asset-pipeline`

Foundation target:

- Asset IDs.
- Asset registry.
- Dependency graph.
- Cooked asset metadata.
