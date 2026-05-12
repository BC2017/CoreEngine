# Test System

The repository starts with an in-tree C++ test harness so tests can run without
downloading third-party dependencies. The harness is intentionally small, but it
already supports:

- Automatic test registration.
- Test suites and names.
- Assertion failures with source locations.
- `--list` for discovery.
- `--filter <text>` for focused local runs.
- Separate unit and integration executables.
- Skipped tests for planned GPU/backend coverage.
- CTest labels for CI selection.

Run all tests with Visual Studio or another multi-config generator:

```powershell
ctest --test-dir build -C Debug --output-on-failure
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

GPU smoke tests must skip cleanly when the backend implementation or required
hardware is unavailable.

List registered unit tests directly:

```powershell
.\build\Debug\HFEngineUnitTests.exe --list
```

Filter registered unit tests directly:

```powershell
.\build\Debug\HFEngineUnitTests.exe --filter core.version
```

## Test Policy

- Unit tests must be deterministic and CPU-only unless clearly marked
  otherwise.
- Integration tests may touch multiple engine systems, temporary files, windows,
  or GPU devices when the test name and label make that dependency clear.
- GPU tests should skip gracefully when backend implementation or required
  hardware features are absent.
- Every bug fix should add a regression test unless automation is technically
  impractical.
- Tests should not rely on execution order.
- Tests should clean up resources they create.

## Future Extensions

The harness should grow only as needed. Expected additions include temporary
directory utilities, log capture, skip support, benchmark separation, image
comparison helpers, and GPU capability filtering.
