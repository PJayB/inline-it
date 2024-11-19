#!/bin/bash
set -e
set -o pipefail

die() { echo "$*" >&2 ; exit 1 ; }

podman_image="jammy-build"

cd "$(realpath "$(dirname "$0")/..")"

if [ "$(uname -o)" = "Msys" ]; then
    is_msys=1
fi

if [ -n "$is_msys" ]; then
    default_preset="mingw64"
else
    default_preset="linux-$(uname -m)"
fi

# Parse the options
og_args=( "$@" )
while true; do
    case "$1" in
    -p|--preset)        preset="$2" ; shift ;;
    "")                 break ;;
    esac
    shift
done

# If no preset was specified, default it
if [ -z "$preset" ]; then
    preset="${default_preset}"
    og_args+=( --preset "$preset" )
fi

xgb_cmake="./tools/xgb/bin/xgb-cmake"

if [ ! -d "./build/${preset}" ]; then
    "$xgb_cmake" --configure -i "$podman_image" "${podman_args[@]}" --preset "$preset" -- -S .
fi
"$xgb_cmake" -i "$podman_image" "${podman_args[@]}" "${og_args[@]}"
