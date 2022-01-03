cd %~dp0
cd ..
rmdir /S /Q deploy
mkdir deploy
cd deploy
copy ..\biblio-build-release\release\biblio.exe .
set PATH=%SystemDrive%\Qt\5.15.0\msvc2019\bin\
windeployqt --verbose 8 --no-compiler-runtime biblio.exe
"%ProgramFiles(x86)%\Inno Setup 6\ISCC.exe" ..\biblio\setup.iss
pause
