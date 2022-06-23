# Simple Press 2

Simple Press is a simple presentation program written in C++. It currently supports only it's own file format - spres.

## Dependencies
 - cmake
 - libzip
 - Qt


## Building
#### Currently only MacOS is officially supported, but it should work on other platforms  
### MacOS

```console 
export QTDIR="PATH TO YOUR QT INSTALLATION DIR" # eg. export QTDIR="~/Qt/6.3.0/macos"
mkdir build && cd build
cmake ..
make -j$(sysctl -n hw.logicalcpu)
```

## spres file format 
# CURRENTLY SPRES FORMAT IS DURING DEVELOPMENT - ANYTHING CAN CHANGE ANYTIME
TODO: small format overview <br/>
for now check examples