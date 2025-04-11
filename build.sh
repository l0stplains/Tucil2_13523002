#!/bin/bash
set -euo pipefail

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

BUILD_DIR="build"

echo -e "${YELLOW}==> Cleaning previous build...${NC}"
rm -rf "$BUILD_DIR"

echo -e "${YELLOW}==> Creating build directory...${NC}"
mkdir -p "$BUILD_DIR"

echo -e "${YELLOW}==> Entering build directory...${NC}"
cd "$BUILD_DIR"

echo -e "${GREEN}==> Running CMake...${NC}"
bear -- cmake ..

echo -e "${GREEN}==> Building project with Make using all available cores...${NC}"
bear -- make -j$(nproc)

echo -e "${GREEN}==> Build complete!${NC}"

