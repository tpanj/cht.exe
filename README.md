Look in https://github.com/chubin/cheat.sh for more info

# Installation

Just copy [downloaded](https://github.com/TadejPanjtar/cht/raw/master/bin/cht.exe) cht.exe to some of your %PATH% dir to use it from every console.

e.g.: copy cht.exe c:\windows\system32 (but you need privileges)

# Usage
Info about print current directory
```
cht pwd
```
add `-d` parameter after URL to not to use coloring at all
```
cht c/strcpy -d
```
Disabling coloring of saved files localy
```
cht file:///tmp/saved_file -d
```

# Compiling
## Linux
gcc -Wall cht.c -lcurl -o cht

------------------

TODO:
- [ ] windows instructions
- [ ] complete cmake
- [ ] compile with openssl

