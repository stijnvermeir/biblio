cd %~dp0
cd ..
rmdir /S /Q deploy
mkdir deploy
cd deploy
copy ..\build-biblio-Desktop_Qt_5_7_1_MSVC2015_32bit-Release\release\biblio.exe .
set PATH=%SystemDrive%\Qt\5.7\msvc2015\bin\
windeployqt --verbose 8 --no-compiler-runtime biblio.exe
"%ProgramFiles(x86)%\Inno Setup 5\ISCC.exe" ..\biblio\setup.iss
pause
