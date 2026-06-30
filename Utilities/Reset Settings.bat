@echo off
setlocal

set "SETTINGS_DIR=%APPDATA%\Light Host Modern"
set "SETTINGS_FILE=%SETTINGS_DIR%\Light Host Modern.settings"
set "CRASHED_PLUGINS_FILE=%SETTINGS_DIR%\RecentlyCrashedPluginsList"

echo Reset settings for Light Host Modern?
choice /C YN /M "Delete saved settings"
if errorlevel 2 (
    echo Settings not altered.
    exit /b 0
)

if exist "%SETTINGS_FILE%" del /F /Q "%SETTINGS_FILE%"
if exist "%CRASHED_PLUGINS_FILE%" del /F /Q "%CRASHED_PLUGINS_FILE%"

echo Settings reset.
