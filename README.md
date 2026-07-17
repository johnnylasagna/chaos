# Chaos

**Chaos** (pronounced **Kay-OS**) is a purely virtual operating system that runs on a custom instruction set architecture (ISA). It makes use of a custom assembly language called chasm. Any firmware functionality is implemented through standard C++ functions.

## Requirements

Chaos makes extensive use of **C++23/C++26** features and currently requires:

* GCC 16.1
* CMake
* MinGW Makefiles

## Building

From the project root, configure the project:

```bash
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE
```

Then build the project:

```bash
cmake --build build
```

The compiled binaries will be placed in the `./build` directory.

## Executables

The build produces three executables:

* `chaos-assembler.exe`
* `chaos-formatter.exe`
* `chaos-vm.exe`

## Assembler

The assembler compiles a Chaos assembly (`.chasm`) file into a binary file.

### Usage

```bash
./build/chaos-assembler.exe <file>
```

### Example

Assemble the bootloader:

```bash
./build/chaos-assembler.exe files/boot.chasm
```

This produces:

```text
files/boot.chasm.bin
```

Assemble the kernel:

```bash
./build/chaos-assembler.exe files/kernel.chasm
```

This produces:

```text
files/kernel.chasm.bin
```

The generated binaries can then be written to a Chaos disk image using the formatter.

## Formatter

The formatter writes the compiled bootloader and kernel binaries to a disk image.

Before running the formatter, the following files must exist:

```text
files/boot.chasm.bin
files/kernel.chasm.bin
```

### Disk Layout

The formatter writes the binaries to the following disk offsets:

| Binary                   | Disk Offset |
| ------------------------ | ----------: |
| `files/boot.chasm.bin`   |         `0` |
| `files/kernel.chasm.bin` |       `512` |

### Usage

First, create an empty disk image in the `./img` directory:

```bash
touch img/disk.img
```

Then run the formatter:

```bash
./build/chaos-formatter.exe img/disk.img
```

On success, the formatter will print:

```text
Successfully wrote to file: img/disk.img
```

## Fonts

Chaos uses an 8×8 bitmap font stored as a raw binary file.

The default font can be replaced with any compatible 8×8 IBM PC BIOS font from the `romfont` project:

https://github.com/spacerace/romfont/tree/master/font-bin

### Requirements

The font must:

- Be an 8×8 bitmap font.
- Contain 256 glyphs.
- Store each glyph as 8 bytes (one byte per row).
- Have a total size of 2048 bytes.

### Installing a Font

1. Download any compatible `.bin` font from the `font-bin` directory.
2. Rename it to:

```text
font.bin
```

3. Place it in the project's `./files` directory, replacing the existing font.

The next time the virtual machine is started, the new font will be loaded automatically.

## Virtual Machine

The virtual machine boots and runs a Chaos disk image.

### Usage

```bash
./build/chaos-vm.exe <disk-image>
```

### Example

```bash
./build/chaos-vm.exe img/disk.img
```

The virtual machine loads the provided disk image and begins executing the bootloader.