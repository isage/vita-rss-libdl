cmake_minimum_required(VERSION 3.12)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

project(dl)
include("${VITASDK}/share/vita.cmake" REQUIRED)

add_library(${PROJECT_NAME} STATIC
  src/sha256.c
  src/main.c
)

install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)
install(
     FILES
       ${dl_SOURCE_DIR}/include/dlfcn.h
     TYPE INCLUDE
)
