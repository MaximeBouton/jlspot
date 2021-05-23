# jlspot 

Wrapping [Spot]() using CxxWrap.jl 

- `spot-2.9.7/`: contains the original spot c++ source files 
- `jlspot/`: the cxxwrap c++ files to expose the functions to Julia. 

## How is this done

### Step 1: Download, extract and build spot source code

```bash 
wget http://www.lrde.epita.fr/dload/spot/spot-2.9.7.tar.gz
tar -xzf spot-2.9.7.tar.gz
rm spot-2.9.7.tar.gz
cd spot-2.9.7
./configure --disable-python --prefix=/path/to/jlspot/spot-build
make -j4 && make install
```

There should be a `.so` in `spot-2.9.7/spot/.libs/`.

### Step 2: Setup a cxxwrap project 

Follow this tutorial
