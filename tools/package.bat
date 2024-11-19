@echo off

set FLAVOR=windows-msvc
set BUILD_DIR=build\%FLAVOR%
set INSTALL_DIR=install\%FLAVOR%

cmake -S . -B "%BUILD_DIR%" --preset "%FLAVOR%"
cmake --build "%BUILD_DIR%" --config Release
cmake --install "%BUILD_DIR%" --prefix "%INSTALL_DIR%" --strip

for /f "tokens=3" %%i in ('"%INSTALL_DIR%\bin\inline-it.exe" --version') do set VERSION="%%i"

tar -a -c -v -C "%INSTALL_DIR%\bin" -f "install\inline-it-%VERSION%-%FLAVOR%.zip" "inline-it.exe"

