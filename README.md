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

## Configure

```powershell
cmake -S . -B build
```

## Build

```powershell
cmake --build build
```

For a specific Visual Studio configuration:

```powershell
cmake --build build --config Debug
```

## Run The Project

The current runnable target is the sandbox executable. It verifies that the
engine core can initialize and shut down cleanly with a selected renderer
backend.

After building Debug with Visual Studio:

```powershell
.\build\Debug\HFEngineSandbox.exe
```

Select DirectX 12:

```powershell
.\build\Debug\HFEngineSandbox.exe --renderer dx12
```

Select Vulkan:

```powershell
.\build\Debug\HFEngineSandbox.exe --renderer vulkan
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
ImGui debug tooling will show the active backend and capabilities before a full
editor is added.

## Run Tests

Run all tests with a Visual Studio or other multi-config generator:

```powershell
ctest --test-dir build -C Debug --output-on-failure
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

The GPU smoke tests currently skip until the concrete DX12 and Vulkan triangle
backend implementations are added.

List registered unit tests:

```powershell
.\build\Debug\HFEngineUnitTests.exe --list
```

Run a focused unit test subset:

```powershell
.\build\Debug\HFEngineUnitTests.exe --filter core.version
```

Test system details live in [tests/README.md](tests/README.md).
