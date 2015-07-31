echo off
if not exist JAVA_HOME set JAVA_HOME=d:\java\jdk1.6.0_24
set path=%JAVA_HOME%\bin\
set rootpath=%cd%\\..

set classpath= %rootpath%\\JWebTop_J\\bin;%rootpath%\\demos\FileManager\\bin;%rootpath%\\JWebTop_J\\lib\\jackson-core-2.4.0.jar;%rootpath%\\JWebTop_J\\lib\\fastjson-1.1.28-x.jar;
cd %rootpath%\\jwebtop_c\\Release
java -classpath %classpath% org.jwebtop.demos.fm.FileManager
