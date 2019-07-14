# Template C++ Project

Contains the libraries I use in most C++ codebases:
- [fmt](https://github.com/fmtlib/fmt)
- [spdlog](https://github.com/gabime/spdlog)
- [CLI11](https://github.com/CLIUtils/CLI11)
- [Catch2](https://github.com/catchorg/Catch2)

## Requirements

The only requirements to be manually installed are `Python`, `pip`, `CMake` and a compiler which is at least C++14 compliant.

Dependencies are managed by [Conan](https://conan.io/).
Conan requires Python and pip installed on the system, then:
```
pip install conan
```

The dependences used here require adding the bincrafters remote conan server:
```
conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
```

That should be it, Conan will be run automatically by CMake.

## Build

As usual for a CMake project.
For example from the source directory do
```
mkdir build
cd build
cmake ..
cmake --build . -- -j8
```

or on Windows
```
mkdir build
cd build
cmake -G "Visual Studio 15 2017 Win64" ..
cmake --build . -- -maxcpucount:8
```

## Run

From the build directory
```
./bin/TemplateProject
```
