# Personal note

I found this project while looking for a simple but accurate animation tool maker.
I noticed some glitches and some bugfix, the project seems abandoned for some years.
I decided to take care of this baby, it is powerful, it well done, it does the job
very well, but it needs a refresh ;-)

# Build

You have 2 options to build the software: using QtCreator, doing it manually

## Using QtCreator
---

First download ans setup [QtCreator](https://www.qt.io/product/development-tools).
Once QtCreator is installed and running:

1. Open the project `AnimationMaker.pro`
2. Choose/configure your build kit (Qt version you want to use, compiler, ...)
3. Build the project
    - do a right click on the project tree and choose build (or run)
    - click on the build (or run) Icon on the bottom left (position might change)

## Manual build
---
First download Qt packages library and their respective development version using your package manager.

_If you installed QtCreator you don't need to install manual packages you can already build the software manualy you already have all the tools._

### for debian/ubuntu/linuxmint

Install necessary build tools:
```
sudo apt install qtbase5-dev libqt5svg5-dev ffmpeg build-essential
```

### for fedora/centos:

**Warning:** fedora/centos users will need to install FFMpeg first.
To do so you must install then enable some extra repositoty, then install it via yum/dnf.

I do not provide the necessary command lines as they may vary and change over time.

Install necessary build tools:
```
sudo yum install make qt5-qtbase-devel qt5-qtsvg-devel ffmpeg
```

Once all packages are installed you can start building the software

1. go to a dierctory where you want all the generated file to be located
2. Use qmake to generate the makefiles
3. build the softwre
4. The generated software is here `build_folder/bin/AnimationMaker`

```
cd /path/to/repositoty
mkdir build
cd build
qmake ../AnimationMaker.pro
make -j4
```

Done! now you can find you newly built AnimationMaker under `/bin/` folder

# Documentation

Documentation can be built using `doxygen`. you can you build it with the following commands

```
mkdir doc
cd doc
doxygen ../Doxyfile
```

The generated output is an HTML documentation located at `doc/html/index.html`

# AnimationMaker

![Image](AnimationMaker.png) 

AnimationMaker is a software designed to help you to quickly build presentation video which you can upload to youtube or vimeo. These presentation videos can be used as pitch videos for crowdfunding campains for example.
It is also possible to create animated gifs.
The idea for the AnimationMaker comes from Adobe Edge which is not available anymore.

See a video to see basic usage at [youtube](https://youtu.be/Ir7Lvd-O2aE).
Also see a video of the newest features [youtube](https://youtu.be/pa53WfUkbO0).

## Contact
The original author of the project: artanidos@gmail.com
