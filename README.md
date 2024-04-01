Utillity to gain help, working examples about various commands, APIs, ... 
__Unified access to the best community driven documentation repositories of the world__

    The only cheat sheet you need

Look in https://github.com/chubin/cheat.sh for more info.

See [Motivation](doc/motivation.md) for reasons this software esists.

# Installation

Just copy downloaded [`cht.exe`](https://github.com/tpanj/cht.exe/raw/master/bin/cht.exe)
to some of your %PATH% dir to use it from every console.

e.g.: copy cht.exe c:\windows\system32 (but you need privileges)

or use [`scoop`](https://github.com/lukesampson/scoop) command-line installer for Windows to do it automatically:
```batch
scoop install cht
```

# Usage
Info about shutdown command
```
cht shutdown
```
![Info about shutdown command](doc/ss_w0.png)

Reversing string in c, second alternative
```
cht c/reverse/2
```
![Reversing string in c, second alternative](doc/ss_w1.png)

## Options
Similar as on obove pic; showing first alternative but now with ```-Q```
(or longer variant ```--query```) which switches on __query mode__ -
space delimetered arguments are parts of query.
```
cht -Q c check file exsists /1
```

Get __version__ info with ```-v``` (or longer variant ```--version```)
switch and config file location (if you do not where to store it).
Currently only server can be defined there. Example of file:
```sh
CHTSH_URL="http://cht.sh" # since https:// protocol is not yet supported, default server
#CHTSH_URL=http://secret.my #my own server
```
add `-T` (or longer variant ```--no_colors```) parameter not to use __coloring__ at all
```
cht c/strcpy -T
```
Disabling __coloring__ of saved files localy
```
cht file:///tmp/saved_file -T
```
![Removing colors](doc/ss_lin1.png)

# Compiling

Use [vlang](https://vlang.io)

## Windows
```
v src -o cht.exe
```
## Linux
### Simplest & quickest
```sh
sudo apt install build-essential libcurl4-openssl-dev
v src -o cht
sudo cp ./cht /usr/local/bin/ # "install" it
```
## External sources
* https://github.com/tapio/rlutil for cross platform console coloring
* https://github.com/Malvineous/cfgpath for managing config files
* https://github.com/skeeto/optparse adding getopt alternative for parsing args
* https://github.com/tpanj/sccp reading config files
------------------

# TODO:
- [ ] Config file

# Deprecated:
- [X] windows compile instructions [here](doc/compile_ms.md)
- [X] complete cmake
- [X] ~~compile with openssl~~ → postponed since this is not needed for now
- [ ] complete functionallity all features of the cht.sh command line
- [X] sorry Mac users, only tasted on Linux and winXP-10
