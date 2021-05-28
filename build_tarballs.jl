# Note that this script can accept some limited command-line arguments, run
# `julia build_tarballs.jl --help` to see a usage message.
using BinaryBuilder, Pkg

name = "jlspot"
version = v"2.9.7"

# Collection of sources required to complete build
sources = [
    GitSource("https://github.com/MaximeBouton/jlspot.git", "01329dd9b54e8a0e3f68298c6078c2453f8382cd")
]

# Bash recipe for building across all platforms
script = raw"""
cd $WORKSPACE/srcdir
cd jlspot/spot-2.9.7/

./configure --prefix=${prefix}/spot-build --build=${MACHTYPE} --host=${target} --disable-python 
make -j4 && make install

# build with cmake 
cd $WORKSPACE/srcdir/jlspot/jlspot
# edit the CMAKE script to find libcxxwrap 
# "/home/maxime/cxxwrap-test/libcxxwrap-julia-build"

# edit the CMAKE script to find spot-build
sed -i 's#\${CMAKE_SOURCE_DIR}/../spot-build/#\${prefix}/spot-build/#g' CMakeLists.txt


# Override compiler ID to silence the horrible "No features found" cmake error
if [[ $target == *"apple-darwin"* ]]; then
  macos_extra_flags="-DCMAKE_CXX_COMPILER_ID=AppleClang -DCMAKE_CXX_COMPILER_VERSION=10.0.0 -DCMAKE_CXX_STANDARD_COMPUTED_DEFAULT=11"
fi
Julia_PREFIX=$prefix
mkdir build
cd build
cmake -DJulia_PREFIX=$Julia_PREFIX -DCMAKE_FIND_ROOT_PATH=$prefix -DJlCxx_DIR=$prefix/lib/cmake/JlCxx -DCMAKE_INSTALL_PREFIX=$prefix -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TARGET_TOOLCHAIN} $macos_extra_flags -DCMAKE_BUILD_TYPE=Release ../
VERBOSE=ON cmake --build . --config Release --target install -- -j${nproc}


exit
"""

# These are the platforms we will build for by default, unless further
# platforms are passed in on the command line
platforms = [
    Platform("x86_64", "linux"; libc = "glibc")
]


# The products that we will ensure are always built
products = [
    ExecutableProduct("genltl", :genltl, "spot-build/bin"),
    LibraryProduct("libspot", :libspot, "spot-build/lib"),
    ExecutableProduct("ltl2tgta", :ltl2tgta, "spot-build/bin"),
    ExecutableProduct("ltlsynt", :ltlsynt, "spot-build/bin"),
    ExecutableProduct("ltlcross", :ltlcross, "spot-build/bin"),
    LibraryProduct("libspotgen", :libspotgen, "spot-build/lib"),
    ExecutableProduct("autcross", :autcross, "spot-build/bin"),
    ExecutableProduct("genaut", :genaut, "spot-build/bin"),
    ExecutableProduct("ltl2tgba", :ltl2tgba, "spot-build/bin"),
    ExecutableProduct("randaut", :randaut, "spot-build/bin"),
    ExecutableProduct("autfilt", :autfilt, "spot-build/bin"),
    ExecutableProduct("ltlfilt", :ltlfilt, "spot-build/bin"),
    ExecutableProduct("ltlgrind", :ltlgrind, "spot-build/bin"),
    ExecutableProduct("ltldo", :ltldo, "spot-build/bin"),
    LibraryProduct("libbddx", :libbddx, "spot-build/lib"),
    LibraryProduct("libspotltsmin", :libspotltsmin, "spot-build/lib"),
    ExecutableProduct("randltl", :randltl, "spot-build/bin"),
    ExecutableProduct("dstar2tgba", :dstar2tgba, "spot-build/bin")
]

# Dependencies that must be installed before this package can be built
dependencies = [
    Dependency("libcxxwrap_julia_jll"),
    BuildDependency(PackageSpec(name="Julia_jll"))
]

# Build the tarballs, and possibly a `build.jl` as well.
build_tarballs(ARGS, name, version, sources, script, platforms, products, dependencies; julia_compat="1.6", preferred_gcc_version = v"9")