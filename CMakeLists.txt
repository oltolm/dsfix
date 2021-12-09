# cmake -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
#
# ninja
# ninja install
# ninja clean
# cpack
# cpack --config CPackSourceConfig.cmake

cmake_minimum_required(VERSION 3.15)
project(dsfix VERSION 2.6.0 LANGUAGES CXX)

if(NOT MSVC)
  find_program(CCACHE ccache)
  if (CCACHE)
    message(STATUS "found ccache: ${CCACHE}")
    set(CMAKE_CXX_COMPILER_LAUNCHER ccache)
  endif()
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
  d3d9
  dxguid
  spdlog::spdlog
)

if(NOT MSVC)
target_link_libraries(dsfix
  PRIVATE
  d3dx9
  dxerr9
)
endif()

if(NOT MSVC)
target_link_options(dsfix
  PRIVATE
  -static-libgcc
  -static-libstdc++
  -static
)
endif()
target_compile_features(dsfix PRIVATE cxx_std_17)

target_include_directories(dsfix
  PRIVATE
  "${PROJECT_SOURCE_DIR}/include"
)
if(NOT MSVC)
target_compile_options(dsfix
  PRIVATE
  -msse2
  -msse
  -Wall
  -Werror
  -Wno-array-bounds
  -Wextra
  -Wno-cast-function-type
  -Wno-unused-parameter
  -Wno-unused-but-set-variable
)
endif()
target_compile_definitions(dsfix
  PRIVATE
  -DUNICODE
  -D_UNICODE
#  -DWIN32_LEAN_AND_MEAN
)

set_target_properties(dsfix
  PROPERTIES
  PREFIX ""
  OUTPUT_NAME "dinput8"
)

if(MSVC)
  find_path(d3dx9_include_dir "d3dx9.h"
    REQUIRED
    PATH_SUFFIXES "dxsdk-d3dx"
  )
  target_include_directories(dsfix PRIVATE ${d3dx9_include_dir})
  find_library(d3dx9_library "d3dx9.lib" REQUIRED)
  target_link_libraries(dsfix PRIVATE ${d3dx9_library})
endif()

install(
  TARGETS dsfix
  RUNTIME DESTINATION "."
)

install(
  DIRECTORY "DATA/"
  DESTINATION "."
)

include(cmake/CPM.cmake)
CPMAddPackage(
  NAME spdlog
  GITHUB_REPOSITORY gabime/spdlog
  GIT_TAG b803e768f35db0880db215a9a04684999afc7bf9
  OPTIONS
  "SPDLOG_WCHAR_SUPPORT ON"
  "SPDLOG_WCHAR_FILENAMES ON"
)
