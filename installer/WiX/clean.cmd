@echo off

rem cleanup temporary stuff
echo Cleaning...
del barcodereader.msi 2>nul
del msistuff.exe 2>nul
del setup.exe 2>nul
del *.wixobj 2>nul
del *.wixpdb 2>nul
del barcodereaderdemo.zip 2>nul
del barcodereaderdemo.exe 2>nul
del License.rtf 2>nul

echo.
echo Ready
