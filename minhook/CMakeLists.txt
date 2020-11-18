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

if(NOT MSVC)
target_link_libraries(minhook
    PRIVATE
    -Wl,--enable-stdcall-fixup
    -static-libgcc
    -static
)
endif()

target_include_directories(minhook INTERFACE include)

target_compile_features(minhook PRIVATE c_std_11)

if(NOT MSVC)
    target_compile_options(minhook PRIVATE -masm=intel)
endif()


target_compile_definitions(minhook PRIVATE)
