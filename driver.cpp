#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include "LeapSDK/include/Leap.h"

// g++ -std=c++11 driver.cpp -o driver ./LeapSDK/lib/x64/libLeap.so -Wl,-rpath,./LeapSDK/lib/x64

#define die(str, args...) do { \
        std::cout << str << "\n" << strerror(errno) << std::endl; \
        exit(EXIT_FAILURE); \
    } while(0)
    
using namespace Leap;

int fd;
float old_x = 0;
float old_y = 0;
float old_z = 0;
float dx_smooth[3];
float dy_smooth[3];
int smooth_size = 3;
bool init = false;
bool btn_left_click = false;
bool btn_left_clicked = false;


class SampleListener : public Listener {
    public:
    virtual void onConnect(const Controller&);
    virtual void onFrame(const Controller&);
};

void SampleListener::onConnect(const Controller& controller) {
    std::cout << "Connected" << std::endl;
}

void SampleListener::onFrame(const Controller& controller) {
    float dx_current = 0;
    float dy_current = 0;
    float z_abs = 0;
    float dx = 0;
    float dy = 0;
    bool some_change = false;
    
    struct input_event     ev;    
    
    const Frame frame = controller.frame();
    
    FingerList allTheFingers = frame.fingers().fingerType(Finger::TYPE_INDEX).extended();    
    
    for(Leap::FingerList::const_iterator fl = allTheFingers.begin(); fl != allTheFingers.end(); fl++)
    {
        
        z_abs = (*fl).tipPosition().z;
        std::cout << "\r" << (int)z_abs << std::flush;
        
        if (z_abs > 0 )
        {
            
            if (init==true)
            {
                init = false;
                for (int i = 0; i<smooth_size; i ++)
                {
                    dx_smooth[i] = 0;
                    dy_smooth[i] = 0;
                }
            }
        }
        else
        {
            if (init == false)
            {
                old_x = (*fl).tipPosition().x;
                old_y = (*fl).tipPosition().y;
                init = true;
            }
            else
            {
                dx_current = - (old_x - (*fl).tipPosition().x) * 3;
                dy_current = (old_y - (*fl).tipPosition().y) * 3;
                
                old_x = (*fl).tipPosition().x;
                old_y = (*fl).tipPosition().y;
                
                if ( (dx_current != 0) || (dy_current != 0) ) //TODO check fo z change
                {
                    
                    for (int i = 0; i<smooth_size-1; i++)
                    {
                        dx_smooth[i] = dx_smooth[i+1];
                        dy_smooth[i] = dy_smooth[i+1];
                        dx += dx_smooth[i];
                        dy += dy_smooth[i];
                    }
                    
                    dx_smooth[smooth_size-1] = dx_current;
                    dy_smooth[smooth_size-1] = dy_current;
                    dx += dx_current;
                    dy += dy_current;
                    
                    dx = dx / smooth_size;
                    dy = dy / smooth_size;
                    
                    memset(&ev, 0, sizeof(struct input_event));
                    ev.type = EV_REL;
                    ev.code = REL_X;
                    ev.value = (int)dx;
                    if(write(fd, &ev, sizeof(struct input_event)) < 0)
                        die("error: write");

                    memset(&ev, 0, sizeof(struct input_event));
                    ev.type = EV_REL;
                    ev.code = REL_Y;
                    ev.value = (int)dy;
                    if(write(fd, &ev, sizeof(struct input_event)) < 0)
                        die("error: write");
                    
                    some_change = true;
                }
                
                if (z_abs != old_z)
                {
                    
                    old_z = z_abs;
                    
                    
                    if ((z_abs < -50) && (btn_left_clicked == false) )
                    {                           
                        
                        
                        if (btn_left_click == false)
                        {
                        
                            
                            btn_left_click = true;
                            
                            memset(&ev, 0, sizeof(struct input_event));

                            ev.type = EV_KEY;
                            ev.code = BTN_LEFT;
                            ev.value = 1;

                            if(write(fd, &ev, sizeof(struct input_event)) < 0)
                                die("error: write");
                        
                        } else {
                            btn_left_click = false;
                            btn_left_clicked = true;
                            
                            memset(&ev, 0, sizeof(struct input_event));

                            ev.type = EV_KEY;
                            ev.code = BTN_LEFT;
                            ev.value = 0;

                            if(write(fd, &ev, sizeof(struct input_event)) < 0)
                                die("error: write");
                            
                            some_change = true;
                        }
                        
                        some_change = true;
                                                
                    } 
                    else if ((z_abs > -40) && (btn_left_clicked == true))
                    {
                        btn_left_clicked = false;
                    }
                }
                
                if (some_change)
                {

                    memset(&ev, 0, sizeof(struct input_event));
                    ev.type = EV_SYN;
                    ev.code = 0;
                    ev.value = 0;
                    if(write(fd, &ev, sizeof(struct input_event)) < 0)
                        die("error: write");
                    
                }
            }
        }
    }
}

int main(int argc, char** argv) {
 
    SampleListener listener;    
    Controller controller;
    
    //connect to system before the listerner creates a new event.
    fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if(fd < 0) {
        die("error: open");
    }

    if(ioctl(fd, UI_SET_EVBIT, EV_KEY) < 0)
        die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, BTN_LEFT) < 0)
        die("error: ioctl");    
    
    if(ioctl(fd, UI_SET_EVBIT, EV_REL) < 0)
        die("error: ioctl");
    if(ioctl(fd, UI_SET_RELBIT, REL_X) < 0)
        die("error: ioctl");
    if(ioctl(fd, UI_SET_RELBIT, REL_Y) < 0)
        die("error: ioctl");
    
    struct uinput_user_dev uidev;

    memset(&uidev, 0, sizeof(uidev));

    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "uinput-LEAP");
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor  = 0x1234;
    uidev.id.product = 0xfedc;
    uidev.id.version = 1;    
    
    
    if(write(fd, &uidev, sizeof(uidev)) < 0)
        die("error: write");

    if(ioctl(fd, UI_DEV_CREATE) < 0)
        die("error: ioctl");

    
    controller.addListener(listener);

    // Keep this process running until Enter is pressed
    std::cout << "Press Enter to quit..." << std::endl;
    std::cin.get();
    
    controller.removeListener(listener);    
    
    if(ioctl(fd, UI_DEV_DESTROY) < 0)
        die("error: ioctl");

    return 0;
}
