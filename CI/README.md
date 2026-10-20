# CI

## Packages' Versions

Abseil: 20250512.1
Protobuf: v36.2
Qt: 6.12.0

## Patches

### 1. MinGW `protoc.exe` crashes with SIGSEGV before `main()` (Dynamic builds only)

- File: `CI/build_protobuf_win64_gcc.sh`
- Patch: `-D CMAKE_C_FLAGS=-ffunction-sections` (applied only when `$shared_libs` is `ON`)
- Cause: GNU ld on MinGW resolves a *weak* symbol reference to the **section base** (offset 0) instead of the symbol's real offset. upb's generated minitables register themselves through a weak `__attribute__((constructor))` function (`upb/port/def.inc`, `UPB_CONSTRUCTOR`), so without one section per function the `.ctors` entry points at whatever function happens to start the `.text` section.
- Symptom: `protoc.exe` segfaults inside `__do_global_ctors` (exit 139) before `main()` runs.
- Effect: `-ffunction-sections` gives every function its own section, so the weak constructor reference resolves correctly. Static builds do not exhibit the crash, so the patch is skipped for them.

### 2. Monolithic Abseil DLL does not link its system libraries (MinGW, Dynamic builds only)

- File: `CI/build_absl_win64_gcc.sh`
- Patch: `-D CMAKE_CXX_STANDARD_LIBRARIES='-ldbghelp -lbcrypt -ladvapi32'` (applied only when `$shared_libs` is `ON`)
- Cause: the single `libabseil_dll.dll` target does not carry the system libraries that its objects need.
- Why this variable: the libs must be placed **after** the objects on the link line. `CMAKE_CXX_STANDARD_LIBRARIES` does that; `CMAKE_SHARED_LINKER_FLAGS` would place them **before** and the link still fails.
- Effect: produces a valid `libabseil_dll.dll` (~1.7 MB). Static builds do not produce the monolithic DLL, and ELF/Ubuntu does not need this patch.

### 3. `ABSL_BUILD_MONOLITHIC_SHARED_LIBS` must follow `$shared_libs`

- Files: `CI/build_absl_win64_gcc.sh`, `CI/build_absl_ubuntu.sh`, `CI/build_absl_win64.ps1`
- Patch: `ABSL_BUILD_MONOLITHIC_SHARED_LIBS='$shared_libs'` (previously hardcoded `OFF`)
- Cause: the option was pinned `OFF`, so a shared build produced ~88 separate DLLs instead of the single monolithic DLL abseil expects on Windows.
- Effect: monolithic ON for every `shared_libs=ON` build. On MSVC this is a no-op (`AbseilConfigureCopts.cmake` forces `ABSL_BUILD_DLL` whenever `BUILD_SHARED_LIBS AND MSVC`); on GCC it is what makes patch #2 necessary.
