set SRC_DIR=%CD%
set BUILD_DIR=%SRC_DIR%\cmake-win

rmdir %BUILD_DIR% /s /q
mkdir %BUILD_DIR%

set CMAKE_DIR=C:\Program Files\CMake\bin
set CMAKE_EXE="%CMAKE_DIR%\cmake.exe"

%CMAKE_EXE% %SRC_DIR% -B %BUILD_DIR% -G"Visual Studio 16 2019" 
