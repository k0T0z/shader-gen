# CI

## Packages' Versions

Abseil: 20250512.1
Protobuf: v36.2
Qt: 6.12.0

## Monolithic Abseil

Abseil is internally ~200 small libraries (`absl::strings` ,`absl::time` ,`absl::crc32c` , …). A "monolithic" build compiles all of their sources into one`abseil_dll.dll` instead of one DLL per module.

- Monolithic ON → 1 DLL (`libabseil_dll.dll` , ~1.7 MB), everything inside it. All abseil targets link against that single DLL. This is what abseil forces on Windows/MSVC (vcpkg too), and it's the option your script sets when`shared_libs=ON` .
- Monolithic OFF → 88 separate DLLs (`libabsl_strings.dll` ,`libabsl_time.dll` , …), linked to each other. Normal on Linux, unusual on Windows.
