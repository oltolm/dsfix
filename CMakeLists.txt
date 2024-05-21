cmake_minimum_required(VERSION 3.15)
project(
  dsfix
  VERSION 2.6.4
  LANGUAGES CXX
)

if(NOT MSVC)
  find_program(CCACHE ccache)

  if(CCACHE)
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

add_library(
  dsfix SHARED
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
  SMAA/SMAA.cpp
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

if(NOT MSVC)
  set_source_files_properties(SMAA/SMAA.cpp PROPERTIES COMPILE_OPTIONS -Wno-unused-but-set-variable)
endif()

target_link_libraries(dsfix PRIVATE minhook d3d9 dxguid spdlog::spdlog dwmapi)

if(NOT MSVC)
  target_link_libraries(dsfix PRIVATE d3dx9 dxerr9 Xinput)
endif()

if(NOT MSVC)
  target_link_options(dsfix PRIVATE -static-libgcc -static-libstdc++ -static)
endif()

target_compile_features(dsfix PRIVATE cxx_std_17)

target_include_directories(
  dsfix PRIVATE "${PROJECT_SOURCE_DIR}/include" "${PROJECT_SOURCE_DIR}/imgui"
  "${PROJECT_BINARY_DIR}"
)

if(NOT MSVC)
  target_compile_options(
    dsfix
    PRIVATE -msse2
    -msse
    -Wall
    # -Werror
    # -Wno-array-bounds
    # -Wextra
    # -Wno-cast-function-type
    # -Wno-unused-parameter
    # -Wno-unused-but-set-variable
    # -Wno-unknown-pragmas
    -Wno-dangling-reference
  )
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  message(STATUS "CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
  target_compile_options(dsfix PRIVATE -Wno-microsoft-exception-spec -Wno-missing-exception-spec)
endif()

target_compile_definitions(dsfix PRIVATE -DUNICODE -D_UNICODE)

set_target_properties(dsfix PROPERTIES PREFIX "" OUTPUT_NAME "dinput8")

if(MSVC)
  target_include_directories(dsfix PRIVATE "$ENV{d3dx_path}\\include")
  target_link_libraries(dsfix PRIVATE "$ENV{d3dx_path}\\release\\lib\\x86\\d3dx9.lib")
endif()

install(TARGETS dsfix RUNTIME DESTINATION ".")

install(
  DIRECTORY "DATA/"
  DESTINATION "."
  PATTERN "DATA/dsfix/*" EXCLUDE
  PATTERN "DATA/dsfix" EXCLUDE
)

include(cmake/CPM.cmake)
cpmaddpackage(
  NAME
  spdlog
  GITHUB_REPOSITORY
  gabime/spdlog
  VERSION
  1.14.1
  OPTIONS
  "SPDLOG_WCHAR_SUPPORT ON"
  "SPDLOG_WCHAR_FILENAMES ON"
)
