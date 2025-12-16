# Building AegisUI

## Requirements

- CMake 3.20 or later
- C++23 compatible compiler:
  - GCC 13 or later
  - Clang 18 or later

## Build Instructions

### Standard Build

```bash
cmake -B build
cmake --build build
```

### Build with Specific Compiler

#### Using GCC
```bash
cmake -B build-gcc -DCMAKE_CXX_COMPILER=g++
cmake --build build-gcc
```

#### Using Clang
```bash
cmake -B build-clang -DCMAKE_CXX_COMPILER=clang++
cmake --build build-clang
```

### Build Options

- `CMAKE_BUILD_TYPE`: Set to `Release` or `Debug` (default: empty/Debug)
  ```bash
  cmake -B build -DCMAKE_BUILD_TYPE=Release
  ```

## Output

The build produces a static library: `libaegisui.a`

## Compiler Flags

The project enforces strict compilation with:
- `-Wall`: Enable all warnings
- `-Wextra`: Enable extra warnings
- `-Werror`: Treat warnings as errors

Both GCC and Clang builds must pass without warnings.
