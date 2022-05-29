rmdir build -Force -Recurse
mkdir build
cp "x64/Release/fmt.dll" build/. 
cp "x64/Release/Planets.exe" build/. 
cp openal32.dll build/.
cp -Recurse bin/ build/.
rm build.zip 
Compress-Archive -Path .\build\* -DestinationPath build.zip