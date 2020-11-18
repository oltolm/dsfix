cmake_minimum_required(VERSION 3.12)
project(minhook VERSION 1.3.3 LANGUAGES C)

add_library(minhook
    STATIC
    src/buffer.c
    src/hook.c
    src/trampoline.c
    src/hde/hde32.c
    src/hde/hde64.c
    dll_resources/MinHook.rc
    dll_resources/MinHook.def
)

target_link_libraries(minhook
    PRIVATE
    -Wl,--enable-stdcall-fixup
    -static-libgcc
    -static
)

target_include_directories(minhook INTERFACE include)

if("${CMAKE_VERSION}" VERSION_GREATER "3.7")
  target_compile_features(minhook PRIVATE c_std_11)
else()
  set_property(TARGET minhook PROPERTY C_STANDARD 11)
endif()

target_compile_options(minhook PRIVATE -masm=intel)

target_compile_definitions(minhook PRIVATE)
