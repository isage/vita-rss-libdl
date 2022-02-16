# Really-simple-stupid libdl implementation for PSVita (vitasdk)

## Installation
Install vitasdk
```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make && make install
```

## Usage

Like on any POSIX platform - include `dlfcn.h`  
Link with `-ldl -ltaihen_stub -lSceKernelModulemgr_stub`