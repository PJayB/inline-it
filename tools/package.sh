#!/bin/bash
set -euo pipefail

die() { echo "$*" >&2 ; exit 1 ; }

[ -v MSYSTEM ] && die "Use package.bat on Windows."

build_install() {
    local flavor="$1"

    "tools/build.sh" --preset "$flavor" --configure
    "tools/build.sh" --preset "$flavor" --config Release

    "tools/build.sh" --preset "$flavor" --install --config Release --prefix "install/$flavor" --strip
}

package() {
    local flavor="$1"
    local version="$2"
    tar czv -C "install/$flavor/bin" -f "install/inline-it-$version-$flavor.tgz" $3
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
