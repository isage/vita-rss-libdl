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
set (${PROJECT_NAME}_VERSION_MAJOR 0)
set (${PROJECT_NAME}_VERSION_MINOR 2)
set (${PROJECT_NAME}_VERSION_RELEASE 0)

add_library(${PROJECT_NAME} STATIC
  src/main.c
  src/taiutils.c
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
