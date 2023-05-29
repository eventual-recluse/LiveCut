# LiveCut

A version of the [Livecut](https://github.com/mdsp/Livecut) beat-slicer audio plugin adapted for the [DISTRHO Plugin Framework](https://github.com/DISTRHO/DPF)

LiveCut uses [Dear ImGui](https://github.com/ocornut/imgui) for the GUI.

![LiveCut](https://raw.githubusercontent.com/eventual-recluse/LiveCut/master/plugins/LiveCut/LiveCut_Screenshot.png "LiveCut")<br/>

# Build instructions

LV2, VST2, VST3 and CLAP plugins, and a JACK standalone app are built by default. Delete the relevant lines beginning with TARGETS += from the Makefile in plugins/LiveCut if you don't want to build them all.

## Build Instructions: Ubuntu
Install dependencies, clone this repository, enter the cloned repository, then make.
```
sudo apt-get install build-essential git pkg-config freeglut3-dev
git clone --recursive https://github.com/eventual-recluse/LiveCut.git
cd LiveCut
make
```
After building, the plugins can be found in the 'bin' folder.

## Build Instructions: Windows 10 64-bit.
LiveCut can be built using [msys2](https://www.msys2.org/)
After installing msys2, launch the MinGW64 shell and enter the following commands to install dependencies, clone this repository, enter the cloned repository, then make.
```
pacman -S base-devel git mingw-w64-x86_64-gcc mingw-w64-x86_64-freeglut
git clone --recursive https://github.com/eventual-recluse/LiveCut.git
cd LiveCut
make -f Makefile-windows
```
After building, the plugins can be found in the 'bin' folder.

## Building on macOS.
It should be possible to build on macOS using the Xcode Command Line Tools.

Install Xcode Command Line Tools from the Terminal:
```
xcode-select --install
```
Then enter the following commands in the Terminal to install dependencies, clone this repository, enter the cloned repository, then make.
```
git clone --recursive https://github.com/eventual-recluse/LiveCut.git
cd LiveCut
make
```
After building, the plugins can be found in the 'bin' folder.

# Credits
[Livecut](https://github.com/mdsp/Livecut) by mdsp @ smartelectronix. Livecut Copyright 2004 by Remy Muller. GPL license.

[DISTRHO Plugin Framework.](https://github.com/DISTRHO/DPF) ISC license.

[Dear ImGui.](https://github.com/ocornut/imgui) MIT license.

[Bruno Ace Font](https://fonts.google.com/specimen/Bruno+Ace) designed by Astigmatic. [Open Font License.](https://scripts.sil.org/cms/scripts/page.php?site_id=nrsi&id=OFL)

[Bruno Ace SC Font](https://fonts.google.com/specimen/Bruno+Ace+SC) designed by Astigmatic. [Open Font License.](https://scripts.sil.org/cms/scripts/page.php?site_id=nrsi&id=OFL)



Livecut
=======

Livecut is based on [BBCut](http://www.cus.cam.ac.uk/~nc272/papers/pdfs/bbcutlib.pdf) the BreakBeat Cutting library written by [Nick Collins](http://www.cus.cam.ac.uk/~nc272/) for [SuperCollider](http://supercollider.sourceforge.net/)

The BBCut Library began out of work on an algorithm to simulate the automatic cutting of breakbeats in the style of early jungle or drum and bass, Nick Collins.

Livecut is only a small subset of what is possible with BBCut, but as it is available as a VST plugin, it is much easier to start experimenting with it.

It is a live beat-slicer but instead of manipulating equal chunks of audio like most beatslicer do, it works on the notion of audio *cuts* whose length and number of repetition depends on the context and the cutting procedure. 
*Cuts* are organized in *blocks* which then form a *phrase*. see Image below. And each phrase can be ended by a *roll* or *fill*.
