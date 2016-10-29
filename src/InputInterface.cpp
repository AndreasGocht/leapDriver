/*
 * leapDriver - a small userspace driver for the Leap Motion
 *
 * Copyright 2016 Andreas Gocht
 *
 * This file is part of leapDriver.
 *
 * leapDriver is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * leapDriver is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with leapDriver.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Contact:
 * Andreas Gocht: andreas.gocht@gmail.com
 */

#include <InputInterface.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <log.h>


namespace leapDriver
{

/** Initialise the uinput interface, and registers different allowed keys and events
 */
InputInterface::InputInterface() {
    //connect to system before the listerner creates a new event.

    this->file = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if(this->file < 0) {
    	throw InputInterfaceError("error opening /dev/uinput",errno);
    }

    if(ioctl(this->file, UI_SET_EVBIT, EV_KEY) < 0)
    	throw InputInterfaceError("error ioctl",errno);
    if(ioctl(this->file, UI_SET_KEYBIT, BTN_LEFT) < 0)
    	throw InputInterfaceError("error ioctl",errno);
    if(ioctl(this->file, UI_SET_KEYBIT, KEY_VOLUMEUP) < 0)
    	throw InputInterfaceError("error ioctl",errno);
    if(ioctl(this->file, UI_SET_KEYBIT, KEY_VOLUMEDOWN) < 0)
    	throw InputInterfaceError("error ioctl",errno);

    if(ioctl(this->file, UI_SET_EVBIT, EV_REL) < 0)
    	throw InputInterfaceError("error ioctl",errno);
    if(ioctl(this->file, UI_SET_RELBIT, REL_X) < 0)
    	throw InputInterfaceError("error ioctl",errno);
    if(ioctl(this->file, UI_SET_RELBIT, REL_Y) < 0)
    	throw InputInterfaceError("error ioctl",errno);
    if(ioctl(this->file, UI_SET_RELBIT, REL_WHEEL) < 0)
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

/** closes the uinput interface
 */
InputInterface::~InputInterface() {
    if(ioctl(this->file, UI_DEV_DESTROY) < 0)
    	logging::error() << "error ioctl: " << strerror(errno);
}

/** Sends a button left click
 *
 */
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

/** Sends a button left release
 *
 */
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

/** Sends a button double tap click
 *
 */
void InputInterface::btn_doubletap_click()
{
    struct input_event     ev;
	memset(&ev, 0, sizeof(struct input_event));

	ev.type = EV_KEY;
	ev.code = BTN_TOOL_DOUBLETAP;
	ev.value = 1;

	if(write(this->file, &ev, sizeof(struct input_event)) < 0)
		throw InputInterfaceError("error write", errno);

}

/** Sends a button double tap release
 *
 */
void InputInterface::btn_doubletap_release()
{
    struct input_event     ev;
    memset(&ev, 0, sizeof(struct input_event));

    ev.type = EV_KEY;
    ev.code = BTN_TOOL_DOUBLETAP;
    ev.value = 0;

    if(write(this->file, &ev, sizeof(struct input_event)) < 0)
    	throw InputInterfaceError("error write", errno);
}

/** Sends a button volume up click
 *
 */
void InputInterface::key_volume_up_click()
{
    struct input_event     ev;
	memset(&ev, 0, sizeof(struct input_event));

	ev.type = EV_KEY;
	ev.code = KEY_VOLUMEUP;
	ev.value = 1;

	if(write(this->file, &ev, sizeof(struct input_event)) < 0)
		throw InputInterfaceError("error write", errno);

}

/** Sends a button volume up release
 *
 */
void InputInterface::key_volume_up_release()
{
    struct input_event     ev;
	memset(&ev, 0, sizeof(struct input_event));

	ev.type = EV_KEY;
	ev.code = KEY_VOLUMEUP;
	ev.value = 0;

	if(write(this->file, &ev, sizeof(struct input_event)) < 0)
		throw InputInterfaceError("error write", errno);

}

/** Sends a button volume up click
 *
 */
void InputInterface::key_volume_down_click()
{
    struct input_event     ev;
	memset(&ev, 0, sizeof(struct input_event));

	ev.type = EV_KEY;
	ev.code = KEY_VOLUMEDOWN;
	ev.value = 1;

	if(write(this->file, &ev, sizeof(struct input_event)) < 0)
		throw InputInterfaceError("error write", errno);

}

/** Sends a button volume up release
 *
 */
void InputInterface::key_volume_down_release()
{
    struct input_event     ev;
	memset(&ev, 0, sizeof(struct input_event));

	ev.type = EV_KEY;
	ev.code = KEY_VOLUMEDOWN;
	ev.value = 0;

	if(write(this->file, &ev, sizeof(struct input_event)) < 0)
		throw InputInterfaceError("error write", errno);

}

/** vertical wheel movement
 *
 * @param: vertical movement
 */
void InputInterface::move_rel_vert_wheel(int dv)
{
	struct input_event     ev;
    memset(&ev, 0, sizeof(struct input_event));

    ev.type = EV_REL;
    ev.code = REL_WHEEL;
    ev.value = dv;

    if(write(this->file, &ev, sizeof(struct input_event)) < 0)
        throw InputInterfaceError("error write", errno);
}

/** horizontal wheel movement
 *
 * @param: vertical movement
 */
void InputInterface::move_rel_hor_wheel(int dh)
{
	struct input_event     ev;
    memset(&ev, 0, sizeof(struct input_event));

    ev.type = EV_REL;
    ev.code = REL_HWHEEL;
    ev.value = dh;

    if(write(this->file, &ev, sizeof(struct input_event)) < 0)
        throw InputInterfaceError("error write", errno);
}


/** relative muse movement,
 *
 * @param dx movement since last update in x
 * @param dy movement since last update in y
 *
 */
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


/** Synchronizes the different informations with the kernel.
 *
 */
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
