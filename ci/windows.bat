rmdir /q /q build
mkdir build
cd build
cmake -G "MSYS Makefiles" ..
make -j4
