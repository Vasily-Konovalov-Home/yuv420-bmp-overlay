#!/bin/bash

#  Сборка yuv420_bmp_overlay под Linux (GCC/Clang)

BUILD_DIR="build_linux"

#  Проверяем что есть компилятор
if command -v g++ &> /dev/null; then
    echo "=== Compiler found: g++ ==="
    g++ --version | head -n 1
elif command -v clang++ &> /dev/null; then
    echo "=== Compiler found: clang++ ==="
    clang++ --version | head -n 1
else
    echo "ERROR: No compiler found (g++ or clang++)"
    exit 1
fi

#  Чистим старую сборку
echo ""
echo "=== Cleaning old build ==="
rm -rf "$BUILD_DIR"

#  Конфигурируем
echo ""
echo "=== Configuring CMake ==="
cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release

if [ $? -ne 0 ]; then
    echo "CMake configuration failed!"
    exit 1
fi

#  Собираем
echo ""
echo "=== Building ==="
cmake --build "$BUILD_DIR"

#  Проверяем результат
if [ -f "$BUILD_DIR/yuv420_bmp_overlay" ]; then
    echo ""
    echo "SUCCESS: $BUILD_DIR/yuv420_bmp_overlay is ready!"
else
    echo ""
    echo "ERROR: Executable not found."
    exit 1
fi