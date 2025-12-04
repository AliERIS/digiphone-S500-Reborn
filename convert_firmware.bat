@echo off
REM Convert all firmware binary files to C arrays

echo Converting firmware files to C arrays...

python bin2c.py ARM_BL.BIN arm_bl
if errorlevel 1 goto error

python bin2c.py ARM_EXT_BL.BIN arm_ext_bl
if errorlevel 1 goto error

python bin2c.py PRIMARY_MAUI.BIN primary_maui
if errorlevel 1 goto error

python bin2c.py VIVA.BIN viva
if errorlevel 1 goto error

echo.
echo All files converted successfully!
echo Moving generated files to Source/Application/Drivers/...

move *_data.h Source\Application\Drivers\
move *_data.c Source\Application\Drivers\

echo Done!
goto end

:error
echo.
echo Error: Conversion failed!
echo Make sure firmware .BIN files are in this directory.
pause

:end
