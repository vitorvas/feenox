---
title: Distribution instructions
lang: en-US
titleblock: |
 Distribution instructions
 ========================
...

This directory contains scripts that create source and binary tarballs for FeenoX.
Also Debian `.deb` packages can be created (if running in a Debian box).

# Source tarball 

 1. Run `./src.sh` (in GNU/Linux) to get a source tarball. This step will convert the documentation from Markdown into PDF so it needs `pandoc`, `pandoc-crossref`, `xelatex`, `inkscape`, etc.

# Binary tarball

 1. Edit `versions.sh` and set the version of both PETSc and SLEPc, for instance

    ```bash
    petsc_ver=3.17.2
    slepc_ver=3.17.1
    ```
   
 2. Run `./petsc.sh` to download, configure and compile the selected version of PETSc and SLEPc.
These will be unpacked in the current directory `dist` but are `.gitignore`d.

 3. Run `./bin.sh` to compile FeenoX and get a tarball with a statically-linked executable.
 Currently the supported architectures architectures are
   
    * Debian-based GNU/Linux
    * Windows with Cygwin

 Contributions to build FeenoX in other architectures (other Unixes such as MacOS) are welcome.
 
 The resulting binary has PETSc and SLEPc statically linked, but only their non-MPI versions.
 Also, it does not include MUMPS. Feel free to play with the scripts to see if you can make them work.

# Debian packages

So far the `deb.sh` uses the source tarball, compiles it and copies the binary and other plain-text docs into a binary-only tree that gets packed with `dpkg-deb`. 

We should investigate using `dpk-buildpackage` to build both the source and the binary packages.

Note that since the `.deb` package contains a dynamically linked binary, it has to be compiled with the available headers each dependency has in the `apt` repository for each release. Hopefully, the `deb.sh` package should figure out the dependencies automatically (i.e. `libpetsc-3.18-real` or `libepetsc-3.16-real` or 3.12 or whatever, etc).


## Using Vagrant

```terminal
mkdir jammy
cd jammy
vagrant init ubuntu/jammy64       # <- this needs virtualbox
vagrant init generic/ubuntu2204   # <- this works with libvrt
vagrant up
vagrant ssh
```

Then clone FeenoX, compile, go to `dist` and run `deb.sh`:

```terminal
sudo apt-get install -y git autoconf automake make gcc g++ libgsl-dev libsundials-dev wget
sudo apt-get install -y petsc-dev slepc-dev           # <- these names depend on the linux dist/version
sudo apt-get install -y libpetsc-dev libslepc-dev     # <- these names depend on the linux dist/version
git clone https://github.com/seamplex/feenox/
cd feenox
./autogen.sh
./configure
make
cd dist
./deb.sh
```

## Names

 * `bookworm` is Debian 12
 * `bullseye` is Debian 11
 * `buster` is Debian 10
 * `kinetic` is Ubuntu 22.10
 * `jammy` is Ubuntu 22.04
 * `focal` is Ubuntu 20.04
