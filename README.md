Piggy for Nintendo
========================

What we needed:

-	devkitPro&devkitARM

-	ctrulib

-	citra 3DS emulator

### devkitPro environment
If you want to know how to configure your devkitPro environment. Please look at [here](./doc/devkitPro.md).
### ctrulib enviroment
If you have succeed to configure the devkitPro environment. The ctrulib will be easy. Look at [here](./doc/ctrulib.md)!
### citra
The citra emulator is a well-developed emulator for nintendo envoronment.[Here](./doc/ctrulib.md) is a good way for installing it.
### About Piggy
Copyright 2015 ZandyWho.

Author:ZandyWho

Name:Piggy

Run Environment:GBA/NintendoDS/Nintendo3DS/Citra Emulator

### Build Piggy

```
cd piggy-master
make
```
And then there will be three more files and one mor folder named 'build' in your project folder.Their extensions are 3dsx,elf and smdh.

### Clean Piggy

```
make clean
```
The folder 'build' and the three files will be moved after cleaning.

### Run Piggy

```
make run
```
The citra emulator will be called and run the game.

### About me
Follow [ZandyWho](http://twitter.com/ZandyWho) on twitter   for more details.