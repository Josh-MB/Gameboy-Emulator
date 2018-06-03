
if "%~1"=="" goto blank
	if "%1"=="Debug" (
		set buildType="Debug"
		goto build
		)

:blank
	set buildType="Release"

:build
set buildDir="build_windows"
if not exist %buildDir% mkdir %buildDir%
echo %buildType%
cd %buildDir%
cmake -DCMAKE_BUILD_TYPE=%buildType% .. 
cmake --build .
cd ..