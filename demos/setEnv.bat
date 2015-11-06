if not exist JAVA_HOME set JAVA_HOME=d:\java\jdk1.6.0_24
set path=%JAVA_HOME%\bin\;%PATH%
set rootpath=%cd%\\..
set dest="%rootpath%\jwebtop_c\JWebTop_JNI\Release"
cd %dest%
set classpath=%rootpath%\JWebTop_J\bin;%rootpath%\JWebTop_J\lib\jackson-core-2.4.0.jar;%rootpath%\JWebTop_J\lib\fastjson-1.1.28.jar;

set testprj=%1
set classpath=%rootpath%\demos\%testprj%\bin;%classpath%
xcopy "%rootpath%\jwebtop_c\JWebTop\Release\JWebTop.exe" "%dest%" /S /D /Y /I
xcopy "%rootpath%\demos\%testprj%\res" "%dest%\res" /S /D /Y
xcopy "%rootpath%\jwebtop_c\Release\JWebTopCommon.dll" "%dest%" /S /D /Y /I