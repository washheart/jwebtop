echo off

call setEnv.bat JWebTop_test

set appfile="%rootpath%\\jwebtop_c\\Release\\demo\\ruler\\index.app"
set appfile="%rootpath%\\demos\JWebTop_test\\index.app"
rem set appfile="%rootpath%\\jwebtop_c\\Release\\demo\\music\\index.app"
rem set appfile="%rootpath%\\jwebtop_c\\Release\\demo\\tip\\index.app"

java -classpath %classpath% org.jwebtop.TestJWebTop JWebTop.dll %appfile%

pause