# jlspot 

Wrapping [Spot]() using CxxWrap.jl 

- `spot-2.9.7/`: contains the original spot c++ source files 
- `jlspot/`: the cxxwrap c++ files to expose the functions to Julia. 
- `Spot2/`: a trivial julia package to test the expose functions, this is only for debugging, it will be replaced by a new versino of Spot.jl once it works.

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

Use the libfoo template for the CMakelists.txt file, add the include directories `spot-build/include` and link `libspot.so`. 
