# cmake -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
#
# ninja
# ninja install
# ninja clean
# cpack
# cpack --config CPackSourceConfig.cmake

cmake_minimum_required(VERSION 3.12)
project(dsfix VERSION 2.6.0 LANGUAGES CXX)

find_program(CCACHE ccache)
if (CCACHE)
  message(STATUS "found ccache: ${CCACHE}")
  set(CMAKE_CXX_COMPILER_LAUNCHER ccache)
endif()

set(CPACK_GENERATOR ZIP)
include(CPack)

add_subdirectory(minhook)

add_library(dsfix
  SHARED
  dinput8.def
  dsfix.rc
  d3d9.cpp
  d3d9dev.cpp
  d3d9int.cpp
  d3d9query.cpp
  Detouring.cpp
  dinput.cpp
  Effect.cpp
  FPS.cpp
  FXAA.cpp
  GAUSS.cpp
  Hud.cpp
  KeyActions.cpp
  main.cpp
  memory.cpp
  RenderstateManager.cpp
  Settings.cpp
  SMAA.cpp
  SSAO.cpp
  TextureManager.cpp
  util.cpp
  WindowManager.cpp
)

target_link_libraries(dsfix
  PRIVATE
  minhook
  -ld3d9
  -ld3dx9
  -ldxguid
  -ldxerr9
)

target_link_options(dsfix
  PRIVATE
  -static-libgcc
  -static-libstdc++
  -static
)

target_compile_features(dsfix PRIVATE cxx_std_17)

target_include_directories(dsfix
  PRIVATE
  "${PROJECT_SOURCE_DIR}/include"
)

target_compile_options(dsfix
  PRIVATE
  -msse2
  -msse
  -Wall
  -Werror
  -Wno-array-bounds
  -Wextra
  -Wno-cast-function-type
)

target_compile_definitions(dsfix
  PRIVATE
  -DTINYFORMAT_ALLOW_WCHAR_STRINGS=1
  -DUNICODE
  -DWIN32_LEAN_AND_MEAN
)

option(RELEASE_VER "build a release version with turned off logging (default is ON)" ON)

if(RELEASE_VER)
  target_compile_definitions(dsfix PRIVATE -DRELEASE_VER)
endif()

set_target_properties(dsfix
  PROPERTIES
  PREFIX ""
  OUTPUT_NAME "dinput8"
)

install(
  TARGETS dsfix
  RUNTIME DESTINATION "."
)

install(
  DIRECTORY "DATA/"
  DESTINATION "."
)
