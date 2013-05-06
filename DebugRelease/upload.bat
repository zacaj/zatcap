set /p var= <version.txt
set /a var=var+1
set /a var=var-1
echo version %var%
@echo off
echo cd www>ftpcmd.dat
echo cd zatcapv3>>ftpcmd.dat

if exist zatcap.tar.bz2 echo put zatcap.tar.bz2 zatcap-%var%.tar.bz2>>ftpcmd.dat
echo -rm zatcap.tar.bz2>>ftpcmd.dat
if exist zatcap.tar.bz2 echo ln zatcap-%var%.tar.bz2 zatcap.tar.bz2>>ftpcmd.dat
if exist zatcap.zip echo put zatcap.zip zatcap-%var%.zip>>ftpcmd.dat
echo -rm zatcap.zip>>ftpcmd.dat
if exist zatcap.zip echo ln zatcap-%var%.zip zatcap.zip>>ftpcmd.dat
echo quit>> ftpcmd.dat
sftp -o "batchmode no"  -b ftpcmd.dat zacajcom@zacaj.com

set /a var=var+1
erase version.txt
echo %var% > version.txt
pause
del ftpcmd.dat