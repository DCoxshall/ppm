# ppm

A PPM image viewer.

## Features
- Cross-platform support (Windows/Linux).
- Fast PPM image loading.
- SDL3 rendering.

## Installation
Download and install a binary from the Releases page. If you wish to build from source, see Build Instructions below.

## Usage
`ppm [image.ppm]`

## Build Instructions
### Windows
Requires Visual Studio 2026. Clone this repository and run the following PowerShell in your copy of the repository:
- `git submodule update --init --recursive`
- `mkdir build`
- `cmake -S . -B .\build\`
Then, open the generated Visual Studio solution and build the project as normal. Once built, the executable can be moved to a desired install location.

### Linux
Requires libxtst-dev. Clone this repository and run the following in your copy of the repository:
- `git submodule update --init --recursive`
- `mkdir build && cd build`
- `cmake ..`
- `make`
This will output a copy of the completed binary in the project root, which can then be moved to a desired install location.