@echo off

SET Z88DK_DIR=c:\z88dk\
SET ZCCCFG=%Z88DK_DIR%lib\config\
SET PATH=%Z88DK_DIR%bin;%PATH%

echo.
echo ****************************************************************************
del *.com
del snake

zcc +cpm main.c -DBIN_TYPE=BIN_CPM --list -compiler=sdcc -create-app -O3 --opt-code-speed -o invaders

echo ****************************************************************************

..\tools\mkfs.cpm -f naburn "C:\dev\nabu\Nabu Internet Adapter\Store\c.dsk"

..\tools\cpmcp -f naburn    "C:\dev\nabu\Nabu Internet Adapter\Store\c.dsk" invaders.com 0:
