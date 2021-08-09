# Stratify OS on the Nucleo 144 Development Boards

This is a collection of Stratify OS board support packages for the STM32 Nucleo-144 development boards.

# Installing the Bootloader

# Installing the OS

```bash
export CHIP=F446ZE
export ARCH=v7em_f4sh
export CHIP=F746ZG
export ARCH=v7em_f5sh
export CHIP=F767ZI
export ARCH=v7em_f5dh
```


```bash
sl os.install:path=Nucleo-${CHIP},build=boot_release,publickey=RhnmvxQ8D4tlh02L8693,key,dest=host@tmp
```

```bash
sl os.install:build=debug,signkey=RhnmvxQ8D4tlh02L8693
```

Running tests:

```bash
sl app.install:path=apps/testsuite/dhrystone,run,terminal,signkey=162ZEPiD33bF1T8diV0t,signkeypassword=4AC673981E969BBC9C33933800960A7F57EC0F9036CAABB2E1CF09402E9B391E
```