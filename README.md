# DdDToVbi
Utility to convert ld-decode/DdD VBI data into a .vbi binary file used by .ldimg files

## Caveat

This is a very quick n' dirty utility that I whipped up to convert VBI data from ld-process-vbi's JSON files ( https://github.com/happycube/ld-decode ) to .VBI binary files ( https://www.daphne-emu.com:9443/mediawiki/index.php/VBIInfo#VBI_File_Format ).

This library depends on mpolib2 ( https://github.com/RulecityLLC/mpolib2 ) and mpojson ( https://github.com/RulecityLLC/mpojson ) .

## To build

These instructions assume that your dependencies are already installed to /tmp/cmake_install.  For Windows, you can substitute 'c:\temp\cmake_install' for '/tmp/cmake_install'.

```
mkdir build
cd build
cmake -DCMAKE_SYSTEM_PREFIX_PATH=/tmp/cmake_install ..
make
```

This will create a single executable.  There is no installation step.
