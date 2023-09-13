@echo off
cd %~dp0%
for %%f in (test*.cpp) do (
	call build.bat	%%~nf
	call run.bat	%%~nf || call :FAILED || goto END
)
:END
call clean.bat
exit /b

:FAILED
echo TEST FAILED
echo Would you like to continue testing? y/n
set /p choice=
if "%choice%" equ "n" (
	echo Testing was stopped by the user
	exit /b 1
)
exit /b 0
