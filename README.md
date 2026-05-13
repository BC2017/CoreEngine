# High Fidelity Engine

This repository is the starting point for a C++ 3D game engine targeting
DirectX 12 and Vulkan, with ImGui tooling, high-fidelity rendering, a robust
material system, particles, and regression-focused tests.

The project source of truth is:

- [docs/ENGINE_SOURCE_OF_TRUTH.md](docs/ENGINE_SOURCE_OF_TRUTH.md)

## Prerequisites

- CMake 3.22 or newer.
- A C++20 compiler.
- On Windows, Visual Studio with the C++ toolchain and Windows SDK.
- Vulkan SDK with Vulkan headers/libraries and `slangc`. CI currently pins
  1.4.309.0; newer local SDKs are fine when they expose the same tools and
  loader library layout.

## Configure

If this is a fresh clone, initialize submodules first:

```powershell
git submodule update --init --recursive
```

```powershell
cmake -S . -B build
```

Or use the Visual Studio preset:

```powershell
cmake --preset vs-debug
```

## Build

```powershell
cmake --build build
```

For a specific Visual Studio configuration:

```powershell
cmake --build build --config Debug
```

Using presets:

```powershell
cmake --build --preset vs-debug
```

## Run The Project

The current runnable target is the sandbox executable. It verifies that the
engine core can initialize and shut down cleanly with a selected renderer
backend. The mesh shader is authored once in Slang and compiled during the
build to DXIL for DirectX 12 and SPIR-V for Vulkan. The sandbox currently
renders a small indexed colored cube so both backends exercise vertex buffers,
index buffers, depth testing, a shared camera view-projection constant buffer,
and the ImGui overlay.

After building Debug with Visual Studio:

```powershell
.\build\Debug\HFEngineSandbox.exe
```

Select DirectX 12:

```powershell
.\build\Debug\HFEngineSandbox.exe --renderer dx12
```

Render a bounded number of DX12 frames for automation:

```powershell
.\build\Debug\HFEngineSandbox.exe --renderer dx12 --frames 3
```

Select Vulkan:

```powershell
.\build\Debug\HFEngineSandbox.exe --renderer vulkan
```

Render a bounded number of Vulkan frames for automation:

```powershell
.\build\Debug\HFEngineSandbox.exe --renderer vulkan --frames 3
```

Disable validation diagnostics:

```powershell
.\build\Debug\HFEngineSandbox.exe --renderer dx12 --no-validation
```

After building with a single-config generator such as Ninja:

```powershell
.\build\HFEngineSandbox.exe
```

Expected output includes the engine name, version, selected backend, validation
state, RTX 4060 high-fidelity target note, backend implementation status, and a
runtime shutdown confirmation.

Backend switching is launch-time only for the first visible milestone. Runtime
ImGui debug tooling now shows the active backend, adapter, validation state,
frame counter, and frame timing placeholder before a full editor is added.

## Run Tests

Run all tests with a Visual Studio or other multi-config generator:

```powershell
ctest --test-dir build -C Debug --output-on-failure
```

Using presets:

```powershell
ctest --preset vs-debug
```

Run all tests with a single-config generator:

```powershell
ctest --test-dir build --output-on-failure
```

Run only unit tests:

```powershell
ctest --test-dir build -C Debug -L unit --output-on-failure
```

Run only integration tests:

```powershell
ctest --test-dir build -C Debug -L integration --output-on-failure
```

Run GPU smoke tests:

```powershell
ctest --test-dir build -C Debug -L gpu --output-on-failure
```

The GPU smoke tests require local graphics runtime support. CI builds the GPU
code but runs non-GPU tests only because hosted Windows runners do not guarantee
a Vulkan-capable adapter or ICD.

List registered unit tests:

```powershell
.\build\Debug\HFEngineUnitTests.exe --list
```

Run a focused unit test subset:

```powershell
.\build\Debug\HFEngineUnitTests.exe --filter core.version
```

Test system details live in [tests/README.md](tests/README.md).

Milestone tracking lives in [docs/MILESTONES.md](docs/MILESTONES.md).
Dependency policy lives in [docs/DEPENDENCIES.md](docs/DEPENDENCIES.md).
