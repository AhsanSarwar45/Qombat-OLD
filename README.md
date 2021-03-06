# QOMBAT [STALE]

An amateur RTS Game Engine that I was developing as a hobby. Just the core systems and basic editor GUI were implemented. This is a stale project now. I have decided to master stuff like rendering, multi-threading and software architecture before moving back to a project like this.

## Dependencies

- [EASTL](https://github.com/electronicarts/EASTL)
- [Dear ImGui](https://github.com/ocornut/imgui)
- [Glad](https://github.com/Dav1dde/glad)
- [GLFW](https://github.com/glfw/glfw)
- [Magic Enum C++](https://github.com/Neargye/magic_enum)
- [spdlog](https://github.com/gabime/spdlog)
- [Catch2](https://github.com/catchorg/Catch2) (For Testing)

## Build Instructions

### Requirements

- Linux (Windows, Mac are not supported yet, but will be supported in the future)
- GCC/Clang with C++17 (MSVC has not been tested yet. It will be supported in the future)
- [CMake](https://cmake.org/) version 1.16+

### Steps

1. Clone the git repository to local machine using: `git clone https://github.com/AhsanSarwar45/Qombat`. It is recommended not to use `git clone --recurse-submodules` as the project includes the EASTL library which has infinitely-recursive submodules.
2. Initialize all the submodules using `git submodule update --init`.
3. Initialize the EASTL submodules using `cd Engine/Vendor/EASTL` and then `git submodule update --init`.
4. You can now build either using the terminal or VSCode.

### Using the terminal

1. Navigate to the cloned directory.
2. Configure CMake using `cmake -B build -DCMAKE_BUILD_TYPE=[BuildType]`. Example for Debug build: `cmake -B build -DCMAKE_BUILD_TYPE=Debug`.
3. Build using `cmake --build build --config [BuildType] --target Editor`.

### Using VSCode + [CMake Tools extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)

1. Open cloned directory in VSCode.
2. Press <kbd>Ctrl</kbd> + <kbd>Shift</kbd> + <kbd>P</kbd> to open the command pallette.
3. Type `CMake: Configure` to configure the CMake files.
4. Select a compiler from the list.
5. Type `CMake: Set Build Target` and select 'Editor'.
6. Type `CMake: Build` to build the project.
