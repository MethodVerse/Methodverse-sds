# methodverse-seq
Sequence Design &amp; Simulation System


This project is a C++-based sequence framework built with CMake and designed for cross-platform development and debugging in Visual Studio Code.

## 🛠️ Build Instructions

Follow these steps to configure and build the project:

### ✅ Step 1: Clean Up

Remove previous build artifacts and intermediate files (when necessary):

```bash
.\tools\clean.py
```

---

### ✅ Step 2: Generate VS Code Task and Launch Files

This step sets up:
- **VS Code tasks** for CMake configure/build
- **VS Code launch configs** for running and debugging

```bash
.\tools\generate_vscode_files.py
```

> 📌 **Note:**
> - Task files help automate CMake configure and build.
> - Launch files are for running and debugging executables in VS Code.

---

### ✅ Step 3: Configure with CMake Preset

Configure the project using a CMake preset defined in `CMakePresets.json`:

```bash
cmake --preset x64-debug
```

Other available presets:
- `x64-release`
- `linux-debug`
- `linux-release`

---

### ✅ Step 4: Build the Project

Compile using the same preset:

```bash
cmake --build . --preset x64-debug
```

---

### ✅ Step 5: Run Tests

Run the test executable (example: parameter module test):

```bash
.\build\x64-debug\tst\parameter_test.exe
```

---

## 🚧 Additional Steps

More steps and modules will be documented as the project evolves.

Stay tuned!

---

## 📁 File Structure (partial)

```
tools/
  clean.py                   # Cleanup script
  generate_vscode_files.py  # Generates VS Code task/launch files

build/
  x64-debug/
    tst/
      parameter_test.exe     # Unit test binary

CMakePresets.json            # Preset definitions for build configurations
```

---

## 📎 Requirements

- CMake ≥ 3.21
- Ninja (recommended)
- GCC (MinGW-w64 on Windows or system compiler on Linux)
- Python 3 (for tools)
- VS Code with CMake Tools & C++ extensions

---
Android builds are supported, and macOS support is planned for future updates. Android platform is currently tested via android emulator in linux runners through GitHub Actions, rather than the local development environment.
Feel free to share feedback, submit issues or feature requests, and contribute to or extend the build system. 🚀