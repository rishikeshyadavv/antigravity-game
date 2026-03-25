@echo off
REM ─────────────────────────────────────────────────────────────────
REM  Build ANTIGRAVITY for Web (Emscripten / WebAssembly)
REM ─────────────────────────────────────────────────────────────────
REM  Prerequisites:
REM    - Emscripten SDK installed (emsdk folder in project root)
REM    - Run:  emsdk\emsdk.bat activate latest
REM
REM  Usage:  build_web.bat
REM ─────────────────────────────────────────────────────────────────

echo [1/3] Activating Emscripten environment...
call emsdk\emsdk_env.bat

echo [2/3] Creating output directory...
if not exist web\NUL mkdir web

echo [3/3] Compiling to WebAssembly...
emcc antigravity_web.c -o web\index.html ^
    -Os -Wall ^
    -DPLATFORM_WEB ^
    -s USE_GLFW=3 ^
    -s ASYNCIFY ^
    -s TOTAL_MEMORY=67108864 ^
    -s ALLOW_MEMORY_GROWTH=1 ^
    --shell-file web\shell.html ^
    --preload-file resources@resources 2>nul; ^
    echo. & echo If 'resources' folder not found, that's OK — no external assets needed.

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ════════════════════════════════════════════════════
    echo   BUILD SUCCESSFUL!
    echo   Output: web\index.html, web\index.js, web\index.wasm
    echo.
    echo   To test locally:
    echo     cd web
    echo     python -m http.server 8080
    echo     Open http://localhost:8080
    echo.
    echo   To deploy to Vercel:
    echo     vercel --prod
    echo ════════════════════════════════════════════════════
) else (
    echo.
    echo BUILD FAILED — check errors above.
)
