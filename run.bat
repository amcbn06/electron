@echo off
:: 1. Create build directory if it doesn't exist
if not exist "build" mkdir build

:: 2. Compile the code (outputting the .exe inside the build folder)
:: We use ../src because the compiler will be running from inside the build folder
g++ src/main.cpp src/Renderer.cpp src/Component.cpp ^
    -o build/electron.exe ^
    -I"C:/SFML/include" ^
    -L"C:/SFML/lib" ^
    -lsfml-graphics -lsfml-window -lsfml-system

:: 3. Check if compilation worked
if %errorlevel% neq 0 (
    echo.
    echo [ERROR] Build failed! Check the messages above.
    pause
    exit /b %errorlevel%
)

:: 4. Copy DLLs from SFML/bin to the build folder so the .exe can run
copy "C:\SFML\bin\*.dll" "build\" >nul

echo [SUCCESS] Build complete. Running electron.exe...
echo.

:: 5. Run the program
cd build
electron.exe
cd ..