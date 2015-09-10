LISCH and EISCH Database Algorithms
=================
Both of these algorithms are used in some applications against collisions. The **LISCH** means late insertion standard coalesced hashing that have an insertion at the end. Instead the **LISCH**, the **EISCH** have an insertion at the beginning (early insertion).

### Compile
Use [GCC](https://gcc.gnu.org/) to compile. Using bash, just run a simple command::
```
$ gcc main.c -o output
```

### Usage
First, you need to run the output file from gcc compiler.
```
$ ./output
```
After that, you will choose `l` to use **LISCH** algorithm or choose `e` to use **EISCH** algorithm.

### Output
The binaries files are saved on files folder with the correspondent algorithm name + `.dat` extension.
