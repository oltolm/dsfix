@echo off
setlocal
cmake -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo -DRELEASE_VER=OFF ..\
