 1. Install mandatory dependencies

    ```terminal
    sudo apt-get install gcc make git automake autoconf libgsl-dev
    ```

    If you cannot install `libgsl-dev` but still have `git` and the build toolchain, you can have the `configure` script to download and compile it for you. See point\ 4 below.
    
 2. Install optional dependencies (of course these are _optional_ but recommended)
 
    ```terminal
    sudo apt-get install libsundials-dev petsc-dev slepc-dev
    ```

 3. Clone Github repository
 
    ```terminal
    git clone https://github.com/seamplex/feenox
    ```

 4. Boostrap, configure, compile & make
 
    ```terminal
    cd feenox
    ./autogen.sh
    ./configure
    make
    ```
    
    If you cannot (or do not want) to use `libgsl-dev` from a package repository, call `configure` with `--enable-download-gsl`:
    
    ```terminal
    ./configure --enable-download-gsl
    ```
    
    If you do not have Internet access, get the tarball manually, copy it to the same directory as `configure` and run again.

 5. Run test suite (optional)
 
    ```terminal
    make check
    ```

 6. Install the binary system wide (optional)
 
    ```terminal
    sudo make install
    ```
 