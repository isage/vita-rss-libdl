cmake_minimum_required(VERSION 3.12)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

set(VITA_MKSFOEX_FLAGS "${VITA_MKSFOEX_FLAGS} -d PARENTAL_LEVEL=1")

project(libdltest-vita)

include_directories(
    "src"
)

set(ELFNAME "libdltest")

file(GLOB SOURCES src/*.c)

set(ELF "${ELFNAME}.elf")
set(SELF "${ELFNAME}.self")

add_executable("${ELF}"
    ${SOURCES}
)

target_link_libraries("${ELF}" -Wl,--whole-archive -lpthread -Wl,--no-whole-archive -ldl -ltaihen_stub -lSceKernelModulemgr_stub -lSceSblSsMgr_stub)

# config is important if you need to make functions from app visible to dlsym!
vita_create_self("${SELF}"
    "${ELF}"
    UNSAFE
    CONFIG config.yml
)

# Build VPK

set(VPK "${ELFNAME}.vpk")
set(TITLE_NAME "libdl-test")
set(TITLE_ID "LDLT99999")
set(TITLE_VER "01.00")

vita_create_vpk("${VPK}" "${TITLE_ID}" "${SELF}"
  NAME "${TITLE_NAME}"
  VERSION "${TITLE_VER}"
  FILE sce_sys sce_sys
  FILE module module
)
