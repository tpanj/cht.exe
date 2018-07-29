Utillity to gain help, working examples about various commands, APIs, ...

Look in https://github.com/chubin/cheat.sh for more info.

# Installation

Just copy [downloaded](bin/cht.exe) cht.exe to some of your %PATH% dir to use it from every console.

e.g.: copy cht.exe c:\windows\system32 (but you need privileges)

# Usage
Info about shutdown command
```
cht shutdown
```
![Reversing string in c, second alternative](doc/ss_w0.png)

```
cht c/reverse/2
```
![Reversing string in c, second alternative](doc/ss_w1.png)

add `-T` parameter after URL not to use coloring at all
```
cht c/strcpy -T
```
Disabling coloring of saved files localy
```
cht file:///tmp/saved_file -T
```
![Removing colors](doc/ss_lin1.png)

# Compiling
## Linux
```sh
gcc -Wall cht.c -lcurl -o cht
sudo cp ./cht /usr/local/bin/ # "install" it
```
------------------

# TODO:
- [ ] windows compile instructions
- [ ] complete cmake
- [ ] compile with openssl
- [ ] complete functionallity all features of the cht.sh command line

