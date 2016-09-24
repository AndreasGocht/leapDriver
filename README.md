## leapDriver
a small uinput driver for the LeapMotion

# Compilation
* get the LeapMotion SDK, install, and place "LepaSDK" in "external/"

```
  mkdir build
  cd build
  cmake ../
  make
```

#Installation


```
  sudo make install
```

  
# Ru n
If Installed with "make install" this is not necessary
```
  sudo chmod a+rw /dev/uinput
  sudo leapd &
  ./leapdriver -c ../config.json
```
# Updates

Will follow
