
# esp8266-phy

This repo offers an abstraction layer for esp8266 (nonos-sdk-2.0.0) for any
other ip implementation than the originally provided one (patched lwip-1.4). 
The original goal is to try and use clean-lwip2 for stability reasons.

# Note

* ipv6 not tried yet
* tcp it is more stable (lot more)
* not proven to be rock-stable yet

# Tested to work so far

* NTPClient
* WiFiAccessPoint
* OTA (you'll have to change 1460 to 536 in espota.py)

# Build it now
(makefiles are working with linux/osx, I have no windows)

```
cd <path-to-your>/esp8266
```

pull minimalist patch for compiling with lwip2 headers
```
git remote add -t lwip2 lwip2patch https://github.com/d-a-v/Arduino.git
git pull lwip2patch lwip2
```

get abstraction layer
```
cd tools/sdk
git clone https://github.com/d-a-v/esp8266-phy.git
```

build it
```
cd esp8266-phy
./get-lwip-git
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
