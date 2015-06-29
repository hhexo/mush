# mush

## What is `mush`?

`mush` is a software implementation of the Musha Shugyo game by Luca De Marini,
providing tools to test the game system, effective AIs that play the game, and
a command-line interface to play the game.

## Who develops `mush`?

`mush` is being developed by me, Dario Domizioli. Credit for the design of the
game system goes to Luca De Marini.

## What are the requirements of `mush`?

### Operating System

* Linux or MacOSX

Technically, I think any UNIX-like system is suitable as an operating system in
which to build `mush`. However, I have only tested Linux (Ubuntu and SuSE) and
MacOSX 10.

### Compiler

* GCC 4.8 (or later), or Clang 3.4 (or later)
* A C++ standard library that supports the C++11 standard

The build system will prefer Clang if both compilers are available. If none is
found, it will use whatever is defined in $CC, $CXX and $LD, however these might
not be compatible with the Clang or GCC flags.

### Tools

* GNU Make 3.81 or later
* Python 2.7
* [optional] Git
* [optional] Doxygen 1.8.x or later, for generating documentation

I think Python 3.x should be OK too, but I have not tested it.

### ... and that's it.

## How to compile and build the suite of tools

First of all get a clone of `mush` through either Git or a tarfile.

Then, `make everything` will build all projects and will create a `product`
subdirectory containing a release of the final products.
