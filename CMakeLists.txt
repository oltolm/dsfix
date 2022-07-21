# cmake --preset <clang|gcc>
# cmake --build --preset <clang|gcc>
#
# ninja
# ninja install
# ninja clean
# cpack
# cpack --config CPackSourceConfig.cmake

cmake_minimum_required(VERSION 3.15)
project(dsfix VERSION 2.6.4 LANGUAGES CXX)

if(NOT MSVC)
  find_program(CCACHE ccache)
  if (CCACHE)
    message(STATUS "found ccache: ${CCACHE}")
    set(CMAKE_CXX_COMPILER_LAUNCHER ccache)
  endif()
endif()

execute_process(
  COMMAND git describe --always
  OUTPUT_VARIABLE VCS_TAG
  OUTPUT_STRIP_TRAILING_WHITESPACE
  WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
)

configure_file("version.h.in" "${PROJECT_BINARY_DIR}/version.h")

set(CPACK_GENERATOR ZIP)
set(CPACK_PACKAGE_VERSION "${CMAKE_PROJECT_VERSION}-${VCS_TAG}")

include(CPack)

add_subdirectory(minhook)

add_library(dsfix
  SHARED
  d3d9.cpp
  d3d9dev.cpp
  d3d9int.cpp
  d3d9query.cpp
  Detouring.cpp
  dinput.cpp
  dinput8.def
  dsfix.rc
  Effect.cpp
  FPS.cpp
  FXAA.cpp
  GAUSS.cpp
  main.cpp
  memory.cpp
  RenderstateManager.cpp
  Settings.cpp
  SMAA.cpp
  SSAO.cpp
  ui.cpp
  util.cpp
  WindowManager.cpp
  imgui/imgui.cpp
  # imgui/imgui_demo.cpp
  imgui/imgui_widgets.cpp
  imgui/imgui_draw.cpp
  imgui/imgui_tables.cpp
  imgui/backends/imgui_impl_dx9.cpp
  imgui/backends/imgui_impl_win32.cpp
)

target_link_libraries(dsfix
  PRIVATE
  minhook
  d3d9
  dxguid
  spdlog::spdlog
  dwmapi
)

if(NOT MSVC)
target_link_libraries(dsfix
  PRIVATE
  d3dx9
  dxerr9
  Xinput
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
  "${PROJECT_SOURCE_DIR}/imgui"
  "${PROJECT_BINARY_DIR}"
)
if(NOT MSVC)
target_compile_options(dsfix
  PRIVATE
  -msse2
  -msse
  -Wall
  # -Werror
  -Wno-array-bounds
  # -Wextra
  -Wno-cast-function-type
  -Wno-unused-parameter
  -Wno-unused-but-set-variable
)
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
message(STATUS "CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
  target_compile_options(dsfix
    PRIVATE
    -Wno-microsoft-exception-spec
    -Wno-missing-exception-spec
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
  # find_path(d3dx9_include_dir "d3dx9.h"
  #   REQUIRED
  #   PATH_SUFFIXES "dxsdk-d3dx"
  # )
  # target_include_directories(dsfix PRIVATE ${d3dx9_include_dir})
  # find_library(d3dx9_library "d3dx9.lib" REQUIRED)
  # target_link_libraries(dsfix PRIVATE ${d3dx9_library})
  target_include_directories(dsfix PRIVATE "$ENV{d3dx_path}\\include")
  target_link_libraries(dsfix PRIVATE "$ENV{d3dx_path}\\release\\lib\\x86\\d3dx9.lib")
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
  VERSION 1.10.0
  OPTIONS
  "SPDLOG_WCHAR_SUPPORT ON"
  "SPDLOG_WCHAR_FILENAMES ON"
  "SPDLOG_WCHAR_TO_UTF8_SUPPORT ON"
)
