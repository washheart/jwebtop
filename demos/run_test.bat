if not exist JAVA_HOME set JAVA_HOME=d:\java\jdk1.6.0_24
set path=%JAVA_HOME%\bin\
set rootpath=%cd%\\..
set appfile="%rootpath%\\jwebtop_c\\Release\\demo\\ruler\\index.app"
rem set appfile="%rootpath%\\jwebtop_c\\Release\\demo\\music\\index.app"
rem set appfile="%rootpath%\\jwebtop_c\\Release\\demo\\tip\\index.app"
set classpath= %rootpath%\\JWebTop_J\\bin;%rootpath%\\demos\JWebTop_test\\bin;
cd %rootpath%\\jwebtop_c\\Release
java -classpath %classpath% org.jwebtop.TestJWebTop JWebTop.dll %appfile%

pause