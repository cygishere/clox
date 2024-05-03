# clox - A Lox Implementation in C


## Description

🚧 **Work in Progress** 🚧

This is an educational project aimed at learning interpreter development, following the book [Crafting Interpreters](https://craftinginterpreters.com).

## Usage

```shell
clox [lox_source_file]
```

## How to Build from Repo

Clone this repo and make a build directory.

```shell
cd your_source_dir
git clone https://github.com/cygishere/clox.git
mkdir your_build_dir
cd your_build_dir
```

### Using CMake

Make sure you have `cmake` and c compiler suite installed.

```shell
cmake your_source_dir/clox
cmake --build .
```

### Using Autotools

Make sure you have `autoconf` and c compiler suite installed.

```shell
autoreconf your_source_dir/clox
./configure && make
```
