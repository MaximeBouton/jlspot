# jlspot 

Wrapping [Spot]() using CxxWrap.jl 


## How is this done

### Step 1: Download, extract and build spot source code

```bash 
wget http://www.lrde.epita.fr/dload/spot/spot-2.9.7.tar.gz
tar -xzf spot-2.9.7.tar.gz
rm spot-2.9.7.tar.gz
cd spot-2.9.7
./configure --disable-python --enable-c++17 --prefix=/path/to/jlspot/spot-build
make -j4 && make install
```

There should be a `.so` in `spot-2.9.7/spot/.libs/`.

### Step 2: Setup a cxxwrap project 

Follow this [tutorial](https://www.youtube.com/watch?v=VoXmXtqLhdo), the source files for the tutorial are here:
https://github.com/barche/cxxwrap-juliacon2020 

Setup a cmake environment in vscode

Create jlspot folder with two files: 
- CMakelists.txt contains the build instruction
- jlspot.cpp: contains the cpp code with the function to expose

To wrap overloaded functions man must use c++ lambdas or `static_cast`. 

Use the libfoo template for the CMakelists.txt file, add the include directories `spot-build/include` and link `libspot.so`. 

### Step 3: BinaryBuilder 

Package everything in binary builder

```bash
# Override compiler ID to silence the horrible "No features found" cmake error
if [[ $target == *"apple-darwin"* ]]; then
  macos_extra_flags="-DCMAKE_CXX_COMPILER_ID=AppleClang -DCMAKE_CXX_COMPILER_VERSION=10.0.0 -DCMAKE_CXX_STANDARD_COMPUTED_DEFAULT=11"
fi
Julia_PREFIX=$prefix
mkdir build
cd build
cmake -DJulia_PREFIX=$Julia_PREFIX -DCMAKE_FIND_ROOT_PATH=$prefix -DJlCxx_DIR=$prefix/lib/cmake/JlCxx -DCMAKE_INSTALL_PREFIX=$prefix -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TARGET_TOOLCHAIN} $macos_extra_flags -DCMAKE_BUILD_TYPE=Release ../jlspot/
VERBOSE=ON cmake --build . --config Release --target install -- -j${nproc}
```
