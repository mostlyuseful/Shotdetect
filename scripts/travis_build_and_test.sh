mkdir build-gcc5
cd build-gcc5
env CC=$(which gcc-5) CXX=$(which g++-5) cmake ..
make -j 4
