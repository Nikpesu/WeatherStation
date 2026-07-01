@echo off
rem ---------------------------------------------------------------------------
rem WeatherStation firmware maker (Windows).
rem
rem Builds firmware.bin + the web-UI filesystem image (littlefs.bin) for every
rem board into  firmware maker\<board>\<board>-firmware.bin  /  -littlefs.bin
rem (plus ESP32 bootloader.bin / partitions.bin for a first-time cable flash).
rem These names match the OTA updater and the GitHub release workflow.
rem
rem Usage:  build.bat                          (all boards)
rem         build.bat seeed_xiao_esp32c3 d1_mini   (only these)
rem ---------------------------------------------------------------------------
setlocal enabledelayedexpansion
set "SCRIPT_DIR=%~dp0"
pushd "%SCRIPT_DIR%.."

rem Find PlatformIO on PATH, else fall back to the bundled penv.
set "PIO=pio"
where pio >nul 2>nul || set "PIO=%USERPROFILE%\.platformio\penv\Scripts\pio.exe"
echo Using PlatformIO: %PIO%

set "ALL_BOARDS=d1_mini lolin_s2_mini seeed_xiao_esp32c3 seeed_xiao_esp32c6 seeed_xiao_esp32s3 esp32doit_devkit_v1 az_delivery_devkit_v4"
if "%~1"=="" ( set "BOARDS=%ALL_BOARDS%" ) else ( set "BOARDS=%*" )

set "OK="
set "FAIL="
for %%E in (%BOARDS%) do (
  echo.
  echo ==============================================================
  echo   Building %%E
  echo ==============================================================
  if not exist "%SCRIPT_DIR%%%E" mkdir "%SCRIPT_DIR%%%E"
  "%PIO%" run -e %%E && "%PIO%" run -e %%E -t buildfs
  if !errorlevel! equ 0 (
    copy /Y ".pio\build\%%E\firmware.bin" "%SCRIPT_DIR%%%E\%%E-firmware.bin" >nul
    if exist ".pio\build\%%E\littlefs.bin" copy /Y ".pio\build\%%E\littlefs.bin" "%SCRIPT_DIR%%%E\%%E-littlefs.bin" >nul
    if exist ".pio\build\%%E\spiffs.bin"   copy /Y ".pio\build\%%E\spiffs.bin"   "%SCRIPT_DIR%%%E\%%E-littlefs.bin" >nul
    if exist ".pio\build\%%E\bootloader.bin" copy /Y ".pio\build\%%E\bootloader.bin" "%SCRIPT_DIR%%%E\" >nul
    if exist ".pio\build\%%E\partitions.bin" copy /Y ".pio\build\%%E\partitions.bin" "%SCRIPT_DIR%%%E\" >nul
    set "OK=!OK! %%E"
  ) else (
    echo !!! %%E FAILED to build
    set "FAIL=!FAIL! %%E"
  )
)

echo.
echo ==============================================================
echo Built:!OK!
if not "!FAIL!"=="" echo Failed:!FAIL!  ^(some boards are known not to compile - see README.md^)
echo Output: %SCRIPT_DIR%^<board^>\
popd
endlocal
