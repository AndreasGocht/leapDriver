## leapDriver
a small uinput driver for the LeapMotion

# Install
* get the LeapMotion SDK, install, and place "LepaSDK" in "../"

```
  mkdir build
  cd build
  cmake ../
  make
```
  
# Run
```
  sudo chmod a+rw /dev/uinput
  sudo leapd &
  ./leapdriver
```
# Updates

Will follow
