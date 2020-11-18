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
  d3dutil.cpp
  Detouring.cpp
  dinputWrapper.cpp
  Effect.cpp
  FPS.cpp
  FXAA.cpp
  GAUSS.cpp
  Hud.cpp
  KeyActions.cpp
  main.cpp
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
  -Wl,--enable-stdcall-fixup
  -static-libgcc
  -static-libstdc++
  -static
  minhook
  -ld3d9
  -ld3dx9
  -ldxguid
  -ldxerr9
)

target_compile_features(dsfix PRIVATE cxx_std_17)

target_include_directories(dsfix
  PRIVATE
  "${PROJECT_SOURCE_DIR}/include"
)

target_compile_options(dsfix PRIVATE -fno-omit-frame-pointer -msse2 -msse)

target_compile_definitions(dsfix PRIVATE
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

if(NOT MSVC)
  find_program(cv2pdb cv2pdb)
  include(cmake/cv2pdb.cmake)
  if(cv2pdb)
    add_cv2pdb_target(dsfix dinput.dll dinput.pdb)
  endif()
endif()

include(cmake/CPM.cmake)
CPMAddPackage(
  NAME ThreadPool
  GIT_REPOSITORY "https://github.com/jhasse/ThreadPool"
  GIT_TAG ae65651
)
target_link_libraries(dsfix PRIVATE ThreadPool)

# if(NOT MSVC)
#   include(cmake/CPM.cmake)
#   CPMAddPackage(
#     NAME mingw-std-threads
#     GITHUB_REPOSITORY oltolm/mingw-std-threads
#     GIT_TAG a1e82eea75c65a278bf7f220d75e536f1e5d821a
#     OPTIONS "MINGW_STDTHREADS_GENERATE_STDHEADERS ON"
#   )
#   target_link_libraries(dsfix PRIVATE mingw_stdthreads)
# endif()
