#!/bin/sh
export PATH=${axoloti_runtime}/platform_osx/bin:$PATH

echo "Compiling patch... with ${axoloti_firmware}"
mkdir -p ${axoloti_home}/build
cd "${axoloti_firmware}"
make -f Makefile.patch

echo "Generating firmware artifacts"
make artifacts
