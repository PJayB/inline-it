#!/bin/bash
set -euo pipefail

die() { echo "$*" >&2 ; exit 1 ; }

[ -n "$MSYSTEM" ] || die "Use package.bat on Windows."

build_install_package() {
    local flavor="$1"

    "tools/build.sh" --preset "$flavor" --configure
    "tools/build.sh" --preset "$flavor" --config Release

    "tools/build.sh" --preset "$flavor" --install --config Release --prefix "install/$flavor" --strip

    # todo, zip it
}

build_install_package windows-mingw64
build_install_package linux-x86_64
build_install_package linux-aarch64
