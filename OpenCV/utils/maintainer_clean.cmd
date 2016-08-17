@echo off
cd ..
del /s /f /q *.ncb *.suo *.o *.plg *.opt *.aps *.sum *.lst *.obj *.tds *.ilk *.exp log *.dep *.csv vc.err vc.log bcc.err bcc.log gcc.err gcc.log icl.err icl.log 2> nul
del /s /f /q bin\*.dll 2> nul
del /s /f /q bin\*.exe 2> nul
del /s /f /q bin\*.ax 2> nul
for /d /r %%1 in (bin\*.*) do rmdir %%1
del /s /f /q lib\*.* 2> nul
for /d /r %%1 in (lib\*.*) do rmdir %%1
del /s /f /q _temp\*.* 2>nul
for /d /r %%1 in (_temp\*.*) do rmdir %%1
del /s /f /q samples\c\*.exe samples\c\*.pdb samples\c\*.ilk samples\c\*.obj samples\c\*.o 2>nul
del /s /f /q _make\cbuilderx\windows\*.*
del /s /f /q _make\cbuilderx\*.cbx~
del /s /f /q _make\cbuilderx\*.cbx.local*
del /s /f /q _make\cbuilderx\*.bpgr~
del /s /f /q _make\cbuilderx\*.bpgr.local*


 