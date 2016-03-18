/*
 * InputInterface.cpp
 *
 *  Created on: 20.02.2016
 *      Author: andreas
 */

#include "InputInterface.h"
#include <linux/input.h>
#include <linux/uinput.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>


namespace leapDriver
{

/**
 * Initialise the uinput interface, and registers different allowed keys and events
 */
InputInterface::InputInterface() {
    //connect to system before the listerner creates a new event.

    this->file = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if(this->file < 0) {
    	throw InputInterfaceError("error open",errno);
    }

    if(ioctl(this->file, UI_SET_EVBIT, EV_KEY) < 0)
    	throw InputInterfaceError("error ioctl",errno);
    if(ioctl(this->file, UI_SET_KEYBIT, BTN_LEFT) < 0)
    	throw InputInterfaceError("error ioctl",errno);
    if(ioctl(this->file, UI_SET_EVBIT, EV_REL) < 0)
    	throw InputInterfaceError("error ioctl",errno);
    if(ioctl(this->file, UI_SET_RELBIT, REL_X) < 0)
    	throw InputInterfaceError("error ioctl",errno);
    if(ioctl(this->file, UI_SET_RELBIT, REL_Y) < 0)
    	throw InputInterfaceError("error ioctl",errno);

    struct uinput_user_dev uidev;

    memset(&uidev, 0, sizeof(uidev));

    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "uinput-LEAP");
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor  = 0x1234;
    uidev.id.product = 0xfedc;
    uidev.id.version = 1;


    if(write(this->file, &uidev, sizeof(uidev)) < 0)
    	throw InputInterfaceError("error write", errno);

    if(ioctl(this->file, UI_DEV_CREATE) < 0)
    	throw InputInterfaceError("error ioctl", errno);
}

/*
 * closes the uinput interface
 */
InputInterface::~InputInterface() {
    if(ioctl(this->file, UI_DEV_DESTROY) < 0)
    	throw InputInterfaceError("error ioctl", errno);
}

void InputInterface::btn_left_click()
{
    struct input_event     ev;
	memset(&ev, 0, sizeof(struct input_event));

	ev.type = EV_KEY;
	ev.code = BTN_LEFT;
	ev.value = 1;

	if(write(this->file, &ev, sizeof(struct input_event)) < 0)
		throw InputInterfaceError("error write", errno);

}

void InputInterface::btn_left_release()
{
    struct input_event     ev;
    memset(&ev, 0, sizeof(struct input_event));

    ev.type = EV_KEY;
    ev.code = BTN_LEFT;
    ev.value = 0;

    if(write(this->file, &ev, sizeof(struct input_event)) < 0)
    	throw InputInterfaceError("error write", errno);

}

void InputInterface::move_rel(int dx, int dy)
{
	struct input_event     ev;
    memset(&ev, 0, sizeof(struct input_event));
    ev.type = EV_REL;
    ev.code = REL_X;
    ev.value = (int)dx;
    if(write(this->file, &ev, sizeof(struct input_event)) < 0)
        throw InputInterfaceError("error write", errno);

    memset(&ev, 0, sizeof(struct input_event));
    ev.type = EV_REL;
    ev.code = REL_Y;
    ev.value = (int)dy;
    if(write(this->file, &ev, sizeof(struct input_event)) < 0)
        throw InputInterfaceError("error write", errno);
}

void InputInterface::sync()
{
	struct input_event     ev;
    memset(&ev, 0, sizeof(struct input_event));
    ev.type = EV_SYN;
    ev.code = 0;
    ev.value = 0;
    if(write(this->file, &ev, sizeof(struct input_event)) < 0)
    	throw InputInterfaceError("error write", errno);

}

}
