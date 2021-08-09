# Stratify OS on the Nucleo 144 Development Boards

This is a collection of Stratify OS board support packages for the STM32 Nucleo-144 development boards.

## Get Started Quickly

You can quickly and easily install pre-built binaries using our [Getting Started Guide](https://docs.stratifylabs.dev/docs/stratify-os/getting-started/hardware/nucleo-144).

## Building

To build Stratify OS for the Nucleo144 boards, you need to have `git` and `cmake` installed on your computer.

> If you are using Windows, I also recommend using [Git Bash](https://gitforwindows.org/).

First set your environment to build just what you need. Just copy and paste the line that corresponds to your hardware

```bash
export CHIP=F446ZE
export CHIP=F746ZG
export CHIP=F767ZI
```

These commands will clone the code, install the tools (compiler and command line tool), setup your environment, and build the code.

```bash
git clone https://github.com/StratifyLabs/StratifyOS-Nucleo144
cmake -P bootstrap.cmake
source profile.sh
```

You may need to update `sl` (the command line tool used with Stratify OS). This requires version `1.14` or above.

```bash
sl --update
slu #if necessary
```

```
cd cmake_arm
cmake .. -GNinja -D${CHIP}=ON
ninja
```
>If you don't want to use Ninja, you can use `-G"Unix Makefiles"` then `make -j12`.

# Installing the Bootloader

The bootloader is installed using the mbed drive mount. Before you install the binary, you need to key it.

To key the binary, run this command from the `StratifyOS-Nucleo144` folder (not `cmake_arm`).

This installs an insecure public key used to validate the OS is signed.

```bash
sl --prepareBootloader=${CHIP}
```

Then copy the file `tmp/Nucleo-${CHIP}_build_boot_release.bin` to the mbed drive. On MacOs, you can use:

```bash
cp tmp/Nucleo-${CHIP}_build_boot_release.bin /Volumes/NODE_${CHIP}
```

# Installing the OS

To install the debug build, use:

> The debug build outputs a ASCII trace stream on the MBED virtual serial port at 115200,8,N,1.

```bash
sl --installDebugOs
```

To install the release build, use:

```bash
sl --installOs
```

# Running the Demo Applications

These keys are both installed and available for application signing. Neither key is secure. The first key has no password. The second key has a password that is publicly available. Either one can be used.

```bash
export SIGN=signkey=RhnmvxQ8D4tlh02L8693
export SIGN=signkey=162ZEPiD33bF1T8diV0t,signkeypassword=4AC673981E969BBC9C33933800960A7F57EC0F9036CAABB2E1CF09402E9B391E
```

```bash
sl app.install:path=apps/HelloWorld,run,terminal,${SIGN}
sl app.install:path=apps/Blinky,run,terminal,${SIGN},args='--pin=1.7'
```

Please see `apps/testsuite/README.md` to run the test applications.
