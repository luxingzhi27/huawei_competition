# bat

cd ./build
mingw32-make
copy ./main.exe ../WindowsRelease/
cd ../WindowsRelease/
./Robot_gui.exe -m ./map.txt "./main.exe"
