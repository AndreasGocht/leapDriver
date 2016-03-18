/*
 * DriverTouch.h
 *
 *  Created on: 20.02.2016
 *      Author: andreas
 */

#ifndef DRIVERTOUCH_H_
#define DRIVERTOUCH_H_

#include "../LeapSDK/include/Leap.h"
#include "InputInterface.h"

namespace leapDriver
{

class DriverTouch:public Leap::Listener {
public:
	DriverTouch();
	virtual ~DriverTouch();

    virtual void onConnect(const Leap::Controller& );
    virtual void onFrame(const Leap::Controller&);
    void mouse_movement(Leap::Pointable, Leap::InteractionBox);

private:
    InputInterface input;

    float mouse_move_multipyer = 30;


    //glaibal values
    float old_x = 0;
    float old_y = 0;
    bool btn_left_click = false;


};
}

#endif /* DRIVERTOUCH_H_ */
