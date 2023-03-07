@echo off

SET Z88DK_DIR=c:\z88dk\
SET ZCCCFG=%Z88DK_DIR%lib\config\
SET PATH=%Z88DK_DIR%bin;%PATH%

echo.
echo ****************************************************************************
del *.com
del snake

zcc +cpm main.c --list -compiler=sdcc -create-app -O3 --opt-code-speed -o snake

echo ****************************************************************************

mkfs.cpm -f naburn "C:\dev\nabu\Nabu Internet Adapter\Store\c.dsk"

cpmcp -f naburn    "C:\dev\nabu\Nabu Internet Adapter\Store\c.dsk" snake.com 0:
