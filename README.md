# mush

## What is `mush`?

`mush` is a software implementation of the Musha Shugyo tabletop game by Luca De
Marini, providing tools to stress-test the combat system in the game, effective
AIs that play the game, and a command-line interface to play the game in
interactive mode.

Musha Shugyo is a game in Italian also available in English. The website however
is in Italian: www.acchiappasogni.org/msrpg

The `mush` interactive interface is entirely in English.

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
subdirectory containing a release of the final product.

Note that the build runs a "stress test" of the Musha Shugyo system and AIs, and
the stress test may take a long time on slow computers.

### Um... can you tell me what to do step by step?

When cloning from Git:

    cd a_path_with_NO_spaces_in_it
    git clone https://github.com/hhexo/mush.git mush
    cd mush
    make everything

When getting a tar or zip file:

    # Extract the archive file into a directory with NO spaces in it
    cd where_you_have_extracted_it
    make everything

### Known issues

Since I have had several reports of this... Be aware that currently the build
system for `mush` does not allow you to have spaces in the path leading to the
directory you have extracted `mush` in.

_This is a limitation of any UNIX Make tool (including GNU Make) since the tool
was born. It's a design choice that Make separates variables and items by white
space: if you have spaces in your path Make will consider it as multiple paths._

I am trying to find a workaround but technically the issue cannot be fixed
within this build system. For now, just put `mush` in a path containing no
spaces.
