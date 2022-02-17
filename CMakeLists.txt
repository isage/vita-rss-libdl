cmake_minimum_required(VERSION 3.12)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
    include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

project(dl)
option(LIBDL_USE_SHA1 "Use sha1 for NID generation" OFF)


add_library(${PROJECT_NAME} STATIC
  src/main.c
)

if(LIBDL_USE_SHA1)
    target_compile_definitions(${PROJECT_NAME} PRIVATE USESHA1=1)
endif()

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
