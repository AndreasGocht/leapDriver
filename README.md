## leapDriver
leapDriver is a small userspace driver for the Leap Motion. The driver uses the uinput interface of the Linux kernel.

leapDriver is at the moment just able to simulate relative movements (like a mouse) and no absolute ones (like a touchpad)

# License
leapDriver is licensed under the GPLv3. However, please be aware that the software under external has different licenses.

# Compilation
To compile leapDriver you need the Leap Motion SDK. You can download it from:

https://developer.leapmotion.com/v2

You need to install the provides 64 bit Debian package (I haven't tested with 32bit).

Next you need to put the "LeapSDK" folder under "external/" 
 
Now please type:

```
  mkdir build
  cd build
  cmake ../
  make
```

# Installation
leapDriver supports Systemd. The files should work with an Ubuntu 16.04. I haven‘t tested for other Linux distributions.

To install type
```
  sudo make install
```
  
The leapDriver should work now. If not, please create a ticket at guthub.

# Run
If you installed the driver with "make install" this is not necessary
```
  sudo chmod a+rw /dev/uinput
  sudo leapd &
  ./leapdriver -c ../leap_driver_config.json
```

# How to use the driver
Please place the Leap Motion in fornt of you. Now:

* using one finger is interpreted as usual mouse movement. If you move your finger forward, the mouse pointer will lock. If you move your finger further, a click will be send.

* using two fingers will be interpreted as a mouse wheel action (usual scrolling). You need to move your fingers wide enough forward to get an effect.

# How to modify the behaviour

If you installed the leapDriver, you need to modify ´/etc/leap_driver_config.json´. Otherwise you need to modify the json you pass to leapDriver.

```
{
    "mouse_move_multipyer":3,
    "mouse_scroll_multipyer":0.75,
    "mouse_move_smooth_value":5,
    "mouse_scroll_smooth_value":3,
    "mouse_move_threashold":20,
    "mouse_click_prepare_value":-40,
    "mouse_click_value":-50,
    "mouse_click_release_value":-40,
    "mouse_wheel_thresold":-40,
    "vol_up_thr":30
}
```
* `mouse_move_multipyer` factor for the speed of the mouse movement (float)
* `mouse_scroll_multipyer` factor for the speed of the mouse scrolling (float)
* `mouse_move_smooth_value` modifies the smoothing. More is smoother (int).
* `mouse_scroll_smooth_value` modifies the smoothing. More is smoother (int).
* `mouse_move_threashold` modifies the value when mouse action is triggered at all
* `mouse_click_prepare_value` value when the mouse starts to lock
* `mouse_click_value` value when a click happens
* `mouse_click_release_value` value when the mouse click is released
* `mouse_wheel_thresold` value when a scrolling is kicked off.
* `vol_up_thr` not used at the moment.

# Questions, Feadback and Bugs
Please be aware that this is a spare time project. Therefore, it might happen that I decide to put this project aside, not implement feature requests or not to fix bugs. However, feedback is still welcome.

If you do have any questions, feedback or a bug, pleas fill in a request on github, or send a mail to:

andreas . gocht + leapDriver [at] gmail.com
