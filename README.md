# Challengers2Kao3
Utility that helps porting "Kao Challengers" (PSP) levels to "Kao: Mystery of Volcano" (PC) engine.

![Kao Challengers screenshot](https://i.imgur.com/P2HzDTT.png)

## List of required external files

#### `https://sourceforge.net/projects/maiat3plusdec/`
#### "MaiAT3PlusDecoder" (An open source Atrac3+ compatible decoder)
```
include/MaiAT3PlusDecoder/MaiAT3PlusCoreDecoder.h
include/MaiAT3PlusDecoder/MaiAT3PlusDecoder.h
include/MaiAT3PlusDecoder/MaiAT3PlusFrameDecoder.h
include/MaiAT3PlusDecoder/Mai_Base0.h
include/MaiAT3PlusDecoder/SUB/Heap_Alloc0.h
include/MaiAT3PlusDecoder/SUB/MaiBitReader.h
include/MaiAT3PlusDecoder/SUB/MaiBufIO.h
include/MaiAT3PlusDecoder/SUB/MaiCriticalSection.h
include/MaiAT3PlusDecoder/SUB/MaiFile.h
include/MaiAT3PlusDecoder/SUB/MaiQueue0.h
include/MaiAT3PlusDecoder/SUB/MaiString.h
include/MaiAT3PlusDecoder/SUB/MaiThread.h
include/MaiAT3PlusDecoder/SUB/Mai_All.h
include/MaiAT3PlusDecoder/SUB/Mai_mem.h
include/MaiAT3PlusDecoder/SUB/Mai_Sleep.h
include/MaiAT3PlusDecoder/SUB/Unicode.h
source/MaiAT3PlusDecoder/MaiAT3PlusCoreDecoder.cpp
source/MaiAT3PlusDecoder/MaiAT3PlusCoreDecoder_DecFunc.cpp
source/MaiAT3PlusDecoder/MaiAT3PlusCoreDecoder_StaticData.cpp
source/MaiAT3PlusDecoder/MaiAT3PlusCoreDecoder_SubFunc.cpp
source/MaiAT3PlusDecoder/MaiAT3PlusDecoder.cpp
source/MaiAT3PlusDecoder/MaiAT3PlusFrameDecoder.cpp
source/MaiAT3PlusDecoder/base/Heap_Alloc0.cc
source/MaiAT3PlusDecoder/base/MaiBitReader.cc
source/MaiAT3PlusDecoder/base/MaiBufIO.cc
source/MaiAT3PlusDecoder/base/MaiCriticalSection.cc
source/MaiAT3PlusDecoder/base/MaiFile.cc
source/MaiAT3PlusDecoder/base/MaiQueue0.cc
source/MaiAT3PlusDecoder/base/MaiString.cc
source/MaiAT3PlusDecoder/base/MaiThread.cc
source/MaiAT3PlusDecoder/base/Mai_mem.cc
source/MaiAT3PlusDecoder/base/Mai_Sleep.cc
source/MaiAT3PlusDecoder/base/Unicode.cc
```

#### `https://xiph.org/vorbis/`
#### Build "libogg" and "libvorbis" dynamic linked libraries
```
libraries/include/ogg/ogg.h
libraries/include/ogg/os_types.h
libraries/include/vorbis/codec.h
libraries/include/vorbis/vorbisenc.h
libraries/Win32/Debug/libogg.lib
libraries/Win32/Debug/libvorbis.lib
libraries/Win32/Release/libogg.lib
libraries/Win32/Release/libvorbis.lib
```
