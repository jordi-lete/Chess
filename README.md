# Chess

A chess game implemented using SDL3 and C++.

## Prerequisites

- [CMake](https://cmake.org/download/) (version 3.10 or higher)
- [Visual Studio](https://visualstudio.microsoft.com/) with C++ development tools installed
- [vcpkg](https://github.com/microsoft/vcpkg) package manager

## Setup Instructions

### 1. Clone the Repository

```bash
git clone https://github.com/jordi-lete/Chess.git
cd Chess
```

### 2. Install vcpkg

If you haven't already installed vcpkg:

```bash
# Clone vcpkg
git clone https://github.com/Microsoft/vcpkg.git

# Navigate to the vcpkg directory
cd vcpkg

# Bootstrap vcpkg
.\bootstrap-vcpkg.bat

# Add vcpkg to your PATH (optional)
$env:VCPKG_ROOT = "C:/path/to/vcpkg"
$env:PATH = "$env:VCPKG_ROOT;$env:PATH"
```

### 3. Install Dependencies with vcpkg

Navigate back to your project directory and install the dependencies:

```bash
# If vcpkg is in your PATH
vcpkg install --triplet=x64-windows

# Or using the full path to vcpkg (if you didn't add to PATH in the previous step)
C:\path\to\vcpkg\vcpkg install --triplet=x64-windows
```

This will read the `vcpkg.json` file and install the required packages (SDL3).

### 4. Configure and Build with CMake

#### Using Visual Studio:

1. Open Visual Studio
2. Select "Open a local folder" and navigate to the Chess directory
3. Visual Studio should automatically detect the CMake configuration
4. Select the build configuration (Debug/Release) from the dropdown
5. Build the project by clicking on "Build" or pressing F7

#### Using CMake from Command Line:

```bash
# Create a build directory
mkdir build
cd build

# Configure CMake
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake

# Build the project
cmake --build . --config Debug
```

### 5. Run the Application

The executable will be located in the build directory, typically:
- `out/build/x64-Debug/Chess.exe` (if using Visual Studio)
- `build/Debug/Chess.exe` (if using command line CMake)

## Project Structure

- `include/` - Header files
- `src/` - Source files
- `assets/` - Texture files
- `sounds/` - Audio files
- `CMakeLists.txt` - CMake build configuration
- `vcpkg.json` - Dependencies manifest

## Contributing

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Open a pull request
