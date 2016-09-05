/*
 * DriverTouch.cpp
 *
 *  Created on: 20.02.2016
 *      Author: andreas
 */

#include "DriverTouch.h"

namespace leapDriver
{

DriverTouch::DriverTouch():Listener() {
}

DriverTouch::~DriverTouch() {
}


/*
 * called on leap Connection
 */
void DriverTouch::onConnect(const Leap::Controller& controller) {
    std::cout << "Connected" << std::endl;
}

/*
 * calles on new Frame from leap
 *
 * calls for every finger mouse_movement
 */
void DriverTouch::onFrame(const Leap::Controller& controller) {

    const Leap::Frame frame = controller.frame();

    Leap::PointableList pointables = frame.pointables();
    Leap::InteractionBox iBox = frame.interactionBox();

    for(Leap::PointableList::const_iterator fl = pointables.begin(); fl != pointables.end(); fl++)
    {
        mouse_movement(*fl,iBox);
    }

}

void DriverTouch::mouse_movement(Leap::Pointable pointable, Leap::InteractionBox iBox)
{

    float dx = 0;
    float dy = 0;
    bool some_change = false;
    Leap::Pointable::Zone zone = pointable.touchZone();
    Leap::Vector normalizedPosition = iBox.normalizePoint(pointable.stabilizedTipPosition());


    //moving ?
	if (zone == Leap::Pointable::ZONE_NONE)
	{
	}


    // aclick migth happen.
	if (zone == Leap::Pointable::ZONE_HOVERING)
	{
        if ((btn_left_click == true))
        {
            btn_left_click = false;

// 	           input.btn_left_release();

            some_change = true;
        }
	}

	if (zone == Leap::Pointable::ZONE_TOUCHING)
	{
        if (btn_left_click == false)
        {

            btn_left_click = true;

//            input.btn_left_click();

            some_change = true;

        }
	}

	dx = (old_x - normalizedPosition.x)*mouse_move_multipyer;
	dy = (old_y - normalizedPosition.y)*mouse_move_multipyer;

	old_x = normalizedPosition.x;
	old_y = normalizedPosition.y;

	std::cout << dx << ";" << dy << ";" << old_x << ";" << old_y << std::endl;

	input.move_rel((int) dx, (int) dy);

    some_change = true;

    if (some_change)
    {
    	input.sync();
    }
}

}
