@echo off
setlocal

set "BUILD_DIR=build_win"

::  Запрашиваем путь к папке bin внутри MinGW
echo Enter path to MinGW bin folder:
echo Example: C:\Program Files\JetBrains\CLion 2026.1.1\bin\mingw\bin
set /p "MINGW_PATH="

::  Проверяем что g++.exe там есть
if not exist "%MINGW_PATH%\g++.exe" (
    echo ERROR: g++.exe not found in %MINGW_PATH%
    exit /b 1
)

::  Добавляем MinGW в PATH
set "PATH=%MINGW_PATH%;%PATH%"

echo.
echo === MinGW found ===
g++ --version | findstr /C:"g++"

::  Чистим старую сборку
echo.
echo === Cleaning old build ===
if exist %BUILD_DIR% rmdir /s /q %BUILD_DIR%

::  Конфигурируем
echo.
echo === Configuring CMake ===
cmake -S . -B %BUILD_DIR% -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release

if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed!
    exit /b 1
)

::  Собираем
echo.
echo === Building ===
cmake --build %BUILD_DIR%

::  Копируем DLL
echo.
echo === Copying runtime DLLs ===
if exist "win32_runtime\*.dll" (
    copy /Y win32_runtime\*.dll %BUILD_DIR%\ >nul
    echo DLLs copied.
) else (
    echo WARNING: win32_runtime folder not found, skipping DLL copy.
)

::  Проверяем результат
if exist %BUILD_DIR%\yuv420_bmp_overlay.exe (
    echo.
    echo SUCCESS: %BUILD_DIR%\yuv420_bmp_overlay.exe is ready!
) else (
    echo.
    echo ERROR: Executable not found.
    exit /b 1
)

endlocal