#!/bin/bash
set -euo pipefail

die() { echo "$*" >&2 ; exit 1 ; }

[ -v MSYSTEM ] && die "Use package.bat on Windows."

xgb_cmake="./tools/xgb/bin/xgb-cmake"

build_install() {
    local flavor="$1"

    "$xgb_cmake" --preset "$flavor" --configure
    "$xgb_cmake" --preset "$flavor" --build --config Release
    "$xgb_cmake" --preset "$flavor" --install --config Release --strip --prefix "install/$flavor"
}

package() {
    local flavor="$1"
    local version="$2"
    local exe="$3"
    tar cz -C "install/$flavor/bin" -f "install/inline-it-$version-$flavor.tgz" "$exe"
}

build_install linux-x86_64
build_install linux-aarch64
build_install windows-mingw64

# Get the version from the host binary
version="$("install/linux-$(uname -m)/bin/inline-it" --version | awk '/.+ version: [0-9.]+/ { print $3 }')"
[ -n "$version" ] || die "Failed to get version number"

package linux-x86_64 "$version" inline-it
package linux-aarch64 "$version" inline-it
package windows-mingw64 "$version" inline-it.exe
