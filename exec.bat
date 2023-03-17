# bat

cd ./WindowsBuild
cmake
mingw32-make
copy ./main.exe ../WindowsRelease/main.exe
cd ../WindowsRelease/
./Robot_gui.exe -m ./map.txt "./main.exe"
