cd win

export PATH=~/opt/cmake/bin:$PATH
export PATH=~/opt/msvc/bin/x64:$PATH
CC=cl CXX=cl cmake -DCMAKE_SYSTEM_NAME=Windows -DBUILD_PROGRAMS=ON -DCMAKE_CROSSCOMPILING=ON -DCMAKE_BUILD_TYPE=Release ../