# Template C++ Project

This aims to be a good starting point for cross-platform, modern C++ projects using CMake.

Features:
- CMake boilerplate to
  - build an executable which depends on some external libraries.
  - set compiler flags to be strict about warnings.
  - make documentation using doxygen.
  - make use of link-time optimisation if available.
  - make use of a faster linker if available.
- Dependency management using [Conan](https://conan.io/).
- A little example code/boilerplate for libraries I use very frequently:
  - [fmt](https://github.com/fmtlib/fmt)
  - [spdlog](https://github.com/gabime/spdlog)
  - [CLI11](https://github.com/CLIUtils/CLI11)
  - [Catch2](https://github.com/catchorg/Catch2)

Some things this template lacks which I may add in the future:
- Continuous integration configuration
- Code coverage configuration
- Static code analysis configuration
- Code formatting configuration

## Requirements

The only requirements to be manually installed on the system are `Python`, `pip`, `CMake` and a C++ compiler.

Conan requires Python and pip installed on the system, then:
```
pip install conan
```

That should be it, Conan will be run automatically by CMake to download the dependencies.

Edit `conanfile.txt` to add/remove dependencies.
If the library you want to use is on https://conan.io/center/ then simply add it under `[requires]` in
`conanfile.txt` and to the list of libraries to link in `CMakeLists.txt`. If the library is not in conan center
then you'll need to learn about Conan _remotes_ and possibly making your own Conan package _recipe_, see
https://docs.conan.io/en/latest/introduction.html

## Build

As usual for a CMake project.
For example, make a build directory (preferably somewhere outside the source directory) and do
```
cmake path/to/source/directory
cmake --build . -- -j8
```
or, better, with Ninja
```
cmake -GNinja path/to/source/directory
ninja
```

or, worse ;), on Windows
```
cmake -G "Visual Studio 15 2017 Win64" path\to\source\directory
cmake --build . -- -maxcpucount:8
```

## Run

From the build directory
```
./bin/TemplateProject
```
