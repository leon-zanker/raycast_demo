# raycast_demo

This is a simple demo of how a digital differential analysis raycaster works.

It was mostly made for my own learning purposes, however, I'm happy if someone else can also use it as a resource to help them understand the algorithm.

## Install

The following dependencies are needed to compile and run the program:

- a Unix-like OS, e.g. Linux or macOS (it should be possible to also build the project on Windows, though this is neither supported nor tested)
- a C compiler (the default is `clang`, though you can specifiy another compiler of your choice, such as `gcc`, via the `compiler` variable in the `build.sh` script)
- `git`
- `pkg-config`
- `raylib`

These are available via package managers or their respective official websites.

1. Clone the repository to a location of your choice.

```shell
git clone https://github.com/leon-zanker/raycast_demo
```

2. Move into the cloned directory.

```shell
cd raycast_demo
```

3. Run the build script (if this step fails, you may have to run `chmod +x build.sh` first to make the script executable).

```shell
./build.sh
```

If you don't get any errors in the output, the build was successfull and the directory will now contain an executable called `raycast_demo`.

If this is not the case, check if you have installed all the dependencies listed above and if `pkg-config` knows about `raylib`.

## Usage

Run the executable after you've built it successfully.

```shell
./raycast_demo
```

## Uninstall

Delete the raycast_demo directory from its parent directory and uninstall any of the unwanted dependencies you installed to build the project.

```shell
# From inside the raycast_demo directory:
cd ..
rm -rf raycast_demo
```

To remove the dependencies you would typically use the package manager that you used to install them. If you downloaded them from the official websites, there should be instuctions on how to unistall them there.

## Copyright

Copyright (c) 2024 Leon Zanker
