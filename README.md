
# esp8266-phy

This repo offers an abstraction layer for esp8266 (nonos-sdk-2.0.0) for any
other ip implementation than the originally provided one (patched lwip-1.4). 
The original goal is to try and use clean-lwip2 for stability reasons.

# Note

* ipv6 not tried yet
* tcp it is more stable
* not proven to be rock-stable yet

# Tested to work so far

* NTPClient
* WiFiAccessPoint
* OTA (you'll have to change 1460 to 536 in espota.py)

# Build it now
(makefiles are working with linux/osx, see below for windows)

```
cd <path-to-your>/esp8266
```

get abstraction layer
```
cd tools/sdk
git clone https://github.com/d-a-v/esp8266-phy.git
```

optionnally tune TCP configuration in esp8266-phy/lwip-git/lwipopts.h

build it
```
cd esp8266-phy
# get or update latest git version of lwip
./get-lwip-git
# compile and install lwip
make install
```

try some sketches

revert lwip back to original one (for git operation on esp8266/Arduino)
```
make revert
```

# A blob for a blob

A binary archive to unzip on top of your installed esp8266/Arduino is
available.
https://drive.google.com/open?id=0BxSrXa09wHRlNnRYOWU1Rng1enc
(esp8266-lwip2-20170406-13:17:47.zip)

This is especially useful for windows users who want to try. 

# about OTA

Although it should work with OTA over http, it won't with arduino IDE / espota.py.

In the meantime, either change 1460 to your MSS value (tools/espota.py: line 156: f.read(1460)),

or use 1460 as MSS value (lwip-git/lwipopts.h).

Remember the MSS footprint: 4*MSS bytes in RAM per tcp connection.
The lowest recommanded value is 536 which is the default here.

# some details about makefiles

Makefiles copy original libfile into libfile.orig, moves lwip/ into
lwip.orig/ then symlink both libfile and lwip/ to the new ones.  'make
revert' restores original stuff. Check in tools/sdk/ .
