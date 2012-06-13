@echo off
echo -----------------------------------------------------
echo            Vibe Streamer - Client Build Tool
echo.
echo           Copyright (C) 2005-2010 Erik Nilsson,
echo         	software on versionstudio point com
echo -----------------------------------------------------
echo.

if "%1"=="" goto :usage

set YUICOMPRESSOR_PATH=%1

set SRC_PATH=..\..\src
set BUILD_PATH=..\..\build
set PLUGINS_PATH=..\..\plugins
set TEMP_PATH=%BUILD_PATH%\tmp
set TARGET_FILE="%TEMP_PATH%\combined.js"
set COMPRESSED_TARGET_FILE="%TEMP_PATH%\combined-min.js"

if not exist %YUICOMPRESSOR_PATH% goto :error

rem ########################################################
rem #
rem # Prepare for build
rem #
rem ########################################################

echo * Preparing for build
del /q "%BUILD_PATH%\*.*"
if exist %TEMP_PATH% rd /s/q %TEMP_PATH%
mkdir "%BUILD_PATH%\tmp"

rem ########################################################
rem #
rem # Merge src directory
rem #
rem ########################################################

echo * Merging src directory
type %SRC_PATH%\Application.js >> %TARGET_FILE%
type %SRC_PATH%\Config.js >> %TARGET_FILE%
type %SRC_PATH%\Language.js >> %TARGET_FILE%
type %SRC_PATH%\LibraryFix.js >> %TARGET_FILE%
type %SRC_PATH%\dd\DDHelper.js >> %TARGET_FILE%
type %SRC_PATH%\dd\GridDropTarget.js >> %TARGET_FILE%
type %SRC_PATH%\gui\explorer\ExplorerDDHelper.js >> %TARGET_FILE%
type %SRC_PATH%\gui\explorer\ExplorerPanel.js >> %TARGET_FILE%
type %SRC_PATH%\gui\player\PlayerPanel.js >> %TARGET_FILE%
type %SRC_PATH%\gui\playlist\PlaylistGrid.js >> %TARGET_FILE%
type %SRC_PATH%\gui\playlist\PlaylistGridDDHelper.js >> %TARGET_FILE%
type %SRC_PATH%\gui\playlist\PlaylistGridDropTarget.js >> %TARGET_FILE%
type %SRC_PATH%\gui\search\SearchPanel.js >> %TARGET_FILE%
type %SRC_PATH%\gui\settings\SettingsDialog.js >> %TARGET_FILE%
type %SRC_PATH%\plugin\Plugin.js >> %TARGET_FILE%
type %SRC_PATH%\util\FormatUtil.js >> %TARGET_FILE%
type %SRC_PATH%\util\IndexUtil.js >> %TARGET_FILE%
type %SRC_PATH%\util\JsonLoader.js >> %TARGET_FILE%
type %SRC_PATH%\util\ResourceManager.js >> %TARGET_FILE%
type %SRC_PATH%\util\StringUtil.js >> %TARGET_FILE%
type %SRC_PATH%\widget\BufferedGrid.js >> %TARGET_FILE%
type %SRC_PATH%\widget\BufferedTreeGrid.js >> %TARGET_FILE%
type %SRC_PATH%\widget\LinkButton.js >> %TARGET_FILE%
type %SRC_PATH%\widget\TreeButton.js >> %TARGET_FILE%

rem ########################################################
rem #
rem # Compress src
rem #
rem ########################################################

echo * Compressing src directory
java -jar %YUICOMPRESSOR_PATH% --nomunge --preserve-semi --disable-optimizations %TARGET_FILE% -o %COMPRESSED_TARGET_FILE%

rem ########################################################
rem #
rem # Deploy build
rem #
rem ########################################################

echo * Deploying build
copy /B /Y %COMPRESSED_TARGET_FILE% "%BUILD_PATH%\vibe-all.js"

rem ########################################################
rem #
rem # Build plugins
rem #
rem ########################################################

echo * Building plugins
for /F "tokens=*" %%F in ('dir /b %PLUGINS_PATH%\*.*') do call :buildplugin %%F

rem ########################################################
rem #
rem # Cleanup
rem #
rem ########################################################

echo * Cleaning up
rd /s/q %TEMP_PATH%

echo.
echo Build finished!
goto :end

rem ########################################################
rem #
rem # Function: Builds the plugin with the given name
rem #
rem ########################################################

:buildplugin
set PLUGIN=%1%
echo * Preparing for plugin build (%PLUGIN%)
del /q "%PLUGINS_PATH%\%PLUGIN%\build\*.*"
del /q "%TEMP_PATH%\*.*"

echo 	- Merging plugin
for /F "tokens=*" %%F in ('dir /s /b %PLUGINS_PATH%\%PLUGIN%\src\*.js') do type "%%F" >> %TARGET_FILE%

echo 	- Compressing plugin
java -jar %YUICOMPRESSOR_PATH% --nomunge --preserve-semi --disable-optimizations %TARGET_FILE% -o %COMPRESSED_TARGET_FILE%

echo 	- Deploying plugin
copy /B /Y %COMPRESSED_TARGET_FILE% "%PLUGINS_PATH%\%PLUGIN%\build\%PLUGIN%-all.js"
goto :end

rem ########################################################
rem #
rem # Function: Explains usage
rem #
rem ########################################################

:usage
echo.
echo Usage: make.bat [path to yuicompressor.jar]
echo Example: make.bat c:\dev\js\yuicompressor\yuicompressor-2.4.2.jar
echo Download yuicompressor at http://developer.yahoo.com/yui/compressor/
echo.
goto :end

rem ########################################################
rem #
rem # Function: Reports error
rem #
rem ########################################################

:error
echo.
echo Error: %YUICOMPRESSOR_PATH% does not seem to point to the yuicompressor jar
echo.
goto :end

:end