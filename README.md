# Simple Press 2

Simple Press is a simple presentation program written in C++. It currently supports only it's own file format - spres. In future support for other file formats will be implemented.

## Dependencies
 - cmake
 - libzip (only Linux + Windows)
 - Qt


## Building
### Linux

```console
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### MacOS

```console 
export QTDIR="PATH TO YOUR QT INSTALLATION DIR" # eg. export QTDIR="~/Qt/6.3.0/macos"
mkdir build && cd build
cmake ..
make -j$(sysctl -n hw.logicalcpu)
```

## spres file format
TODO: small format overview <br/><br/>
for now check examples