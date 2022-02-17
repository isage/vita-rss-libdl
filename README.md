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
Link with `-ldl -ltaihen_stub -lSceKernelModulemgr_stub -lSceSblSsMgr_stub`  
Build your `suprx` without custom NIDs and use `dlopen` to load it.

libdl uses sha256 for NID generation, because vitasdk uses it, with fallback to sha1 if sha256 NID isn't found.

## Thanks
* [Graphene](https://github.com/GrapheneCt)
* [PoS](https://github.com/Princess-of-Sleeping)