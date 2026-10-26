#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
# Build script for Krema

set -euo pipefail

# ANSI color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT_DIR"

PRESET="dev"
CLEAN=false
RUN_TESTS=false
JOBS=""

print_usage() {
    cat << EOF
Usage: $(basename "$0") [OPTIONS]

Build script for Krema dock.

Options:
    -p, --preset PRESET   Specify CMake preset: dev (default), release, asan
    -r, --release         Shorthand for --preset release
    -c, --clean           Clean build directory before configuring/building
    -t, --test            Run unit/integration tests after successful build
    -j, --jobs N          Number of parallel compile jobs (default: all cores)
    -h, --help            Show this help message

Examples:
    ./build.sh                     # Standard development build (Debug, tests enabled)
    ./build.sh -r                  # Release build
    ./build.sh -c                  # Clean rebuild
    ./build.sh -t                  # Build and execute tests
EOF
}

# Parse command line options
while [[ $# -gt 0 ]]; do
    case "$1" in
        -p|--preset)
            PRESET="$2"
            shift 2
            ;;
        -r|--release)
            PRESET="release"
            shift
            ;;
        --asan)
            PRESET="asan"
            shift
            ;;
        -c|--clean)
            CLEAN=true
            shift
            ;;
        -t|--test)
            RUN_TESTS=true
            shift
            ;;
        -j|--jobs)
            JOBS="$2"
            shift 2
            ;;
        -h|--help)
            print_usage
            exit 0
            ;;
        *)
            echo -e "${RED}Error: Unknown option '$1'${NC}" >&2
            print_usage
            exit 1
            ;;
    esac
done

# Validate prerequisites
for cmd in cmake ninja; do
    if ! command -v "$cmd" &> /dev/null; then
        echo -e "${RED}Error: '$cmd' is required but not installed.${NC}" >&2
        exit 1
    fi
done

BUILD_DIR="build/${PRESET}"

# Handle clean
if [ "$CLEAN" = true ]; then
    echo -e "${YELLOW}Cleaning build directory: ${BUILD_DIR}${NC}"
    rm -rf "$BUILD_DIR"
fi

# Configure if not configured yet or if clean was run
if [ ! -f "${BUILD_DIR}/CMakeCache.txt" ]; then
    echo -e "${BLUE}Configuring with preset '${PRESET}'...${NC}"
    cmake --preset "$PRESET"
else
    echo -e "${BLUE}Using existing configuration in '${BUILD_DIR}'...${NC}"
fi

# Build
echo -e "${BLUE}Building target with preset '${PRESET}'...${NC}"
BUILD_CMD=(cmake --build --preset "$PRESET")
if [ -n "$JOBS" ]; then
    BUILD_CMD+=(--parallel "$JOBS")
fi

"${BUILD_CMD[@]}"

BINARY_PATH="${BUILD_DIR}/bin/krema"
if [ -f "$BINARY_PATH" ]; then
    echo -e "${GREEN}==> Build completed successfully!${NC}"
    echo -e "Binary available at: ${GREEN}${BINARY_PATH}${NC}"
fi

# Run tests if requested
if [ "$RUN_TESTS" = true ]; then
    echo -e "${BLUE}Running tests...${NC}"
    if ctest --preset "$PRESET" --show-only &> /dev/null; then
        ctest --preset "$PRESET"
    else
        ctest --test-dir "$BUILD_DIR" --output-on-failure
    fi
fi
