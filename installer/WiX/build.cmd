@echo off

rem cleanup temporary stuff
call clean.cmd

echo Refreshing license...
copy /y ..\..\barcodereader\License.rtf . 

echo Copying MSI tools...
copy /y e:\mdev\lightning_cube\SRC\oem\msistuff.exe . 
copy /y e:\mdev\lightning_cube\SRC\oem\setup.exe .

rem build the installation package
echo.
echo Building installer
"C:\Program Files\Windows Installer XML v3\bin\candle" -sw -wx -nologo barcodereader.wsx MyExitDialog.wxs MyWixUI_InstallDir.wxs

"C:\Program Files\Windows Installer XML v3\bin\light" -sw -wx -out barcodereader.msi -nologo barcodereader.wixobj MyExitDialog.wixobj MyWixUI_InstallDir.wixobj -ext "D:\Program Files\Windows Installer XML v3\bin\WixUIExtension.dll" 

msistuff setup.exe /d barcodereader.msi /n "BarcodeReaderDemo" /o INSTALLUPD /v 300 /w InstMsiW.exe 

"C:\program files\7-Zip\7z.exe" a -tzip -mx8 barcodereaderdemo.zip setup.exe barcodereader.msi

"C:\program files\Chilkat Software Inc\ZIP 2 Secure EXE\ChilkatZipSE.exe" -autotemp -run setup.exe barcodereaderdemo.zip

echo.
echo Ready
