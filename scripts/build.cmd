@echo off
setlocal
pushd "%~dp0..\build"
ninja dsfix_pdb && ^
xcopy /y "pdb\dinput8.dll" "C:\Games\DARK SOULS - Prepare To Die Edition\DATA\" && ^
xcopy /y "pdb\dinput8.pdb" "C:\Games\DARK SOULS - Prepare To Die Edition\DATA\"
rem xcopy /y "..\DATA\DSfix.ini" "C:\Games\DARK SOULS - Prepare To Die Edition\DATA\" && ^
rem xcopy /y "..\DATA\DSfixKeys.ini" "C:\Games\DARK SOULS - Prepare To Die Edition\DATA\"
popd
