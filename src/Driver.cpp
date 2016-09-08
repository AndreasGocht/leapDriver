/*
 * Driver.cpp
 *
 *  Created on: 20.02.2016
 *      Author: andreas
 */

#include "Driver.h"
#include <cmath>

namespace leapDriver
{

Driver::Driver():Listener() {
    dx_smooth = new float[mouse_move_smooth_value];
    dy_smooth = new float[mouse_move_smooth_value];
 }

Driver::~Driver() {
}


/*
 * called on leap Connection
 */
void Driver::onConnect(const Leap::Controller& controller) {
    std::cout << "Connected" << std::endl;
}

/*
 * called on new Frame from leap
 *
 * calls for every finger mouse_movement
 */
void Driver::onFrame(const Leap::Controller& controller) {

    const Leap::Frame frame = controller.frame();

    Leap::FingerList allTheFingers = frame.fingers().extended();

    if (allTheFingers.count() == 0)
    {
    	reinit = true;
    	reinit_scroll = true;
    	reinit_gesture = true;
    }
    else if(allTheFingers.count() == 1)
    {
    	reinit_scroll = true;
    	reinit_gesture = true;
		for(Leap::FingerList::const_iterator fl = allTheFingers.begin(); fl != allTheFingers.end(); fl++)
		{
			mouse_movement(*fl);
		}
    }else if(allTheFingers.count() == 2)
    {
    	reinit_gesture = true;
    	reinit = true;
    	mouse_scroll_movement(allTheFingers);
    }
    else if (allTheFingers.count() == 5)
    {
    	reinit_scroll = true;
    	reinit = true;
    	gesture(allTheFingers);
    }

}

void Driver::mouse_movement(Leap::Finger finger)
{

    float dx_current = 0;
    float dy_current = 0;
    float z_abs = 0;
    float dx = 0;
    float dy = 0;
    bool some_change = false;

    z_abs = (finger).tipPosition().z;
//    std::cout << "\r" << z_abs << std::flush;
//    std::cout << (int)z_abs << std::endl;


    if (z_abs > mouse_move_value)
    {
    	reinit = true;
    	return;
    }


    /*
     * there where no finges on some previos frames ... reinit erverything
     */
	if (reinit==true)
	{
		reinit = false;
		for (int i = 0; i<mouse_move_smooth_value; i ++)
		{
			dx_smooth[i] = 0;
			dy_smooth[i] = 0;
			old_x = (finger).tipPosition().x;
			old_y = (finger).tipPosition().y;
		}
	}

	//dealing with Z values
	// a click migth happen.
	if (z_abs < mouse_click_prepare_value)
	{
		if (z_abs != old_z)
		{

			old_z = z_abs;


			if ((z_abs < mouse_click_value) && (btn_left_clicked == false) )
			{
				if (btn_left_click == false)
				{

					btn_left_click = true;

					input.btn_left_click();

					some_change = true;

				}
			}

			if ((btn_left_click == true) && (btn_left_clicked == false))
			{
				btn_left_click = false;
				btn_left_clicked = true;

				input.btn_left_release();

				some_change = true;
			}
		}
	}

	if ((z_abs > mouse_click_release_value) && (btn_left_clicked == true))
	{
			btn_left_clicked = false;
	}

	/*
	 * just compute dx and dy if no click migth happen
	 */
	if (z_abs > mouse_click_prepare_value)
	{
		dx_current = - (old_x - (finger).tipPosition().x) * mouse_move_multipyer;
		dy_current = (old_y - (finger).tipPosition().y) * mouse_move_multipyer;

		old_x = (finger).tipPosition().x;
		old_y = (finger).tipPosition().y;

		if ( (dx_current != 0) || (dy_current != 0) ) //TODO check fo z change
		{

			for (int i = 0; i<mouse_move_smooth_value-1; i++)
			{
				dx_smooth[i] = dx_smooth[i+1];
				dy_smooth[i] = dy_smooth[i+1];
				dx += dx_smooth[i];
				dy += dy_smooth[i];
			}

			dx_smooth[mouse_move_smooth_value-1] = dx_current;
			dy_smooth[mouse_move_smooth_value-1] = dy_current;
			dx += dx_current;
			dy += dy_current;

			dx = dx / mouse_move_smooth_value;
			dy = dy / mouse_move_smooth_value;

			input.move_rel((int) dx, (int) dy);

			some_change = true;
		}
    }

    if (some_change)
    {
    	input.sync();
    }
}

/**
 * doesn't work ... take a look to the synaptics driver
 * http://lxr.free-electrons.com/source/drivers/input/mouse/synaptics.c#L1070
 *
 * http://linuxwacom.sourceforge.net/wiki/index.php/Kernel_Input_Event_Overview
 *
 * and figure out what to do
 *
 * TODO scroling works ... clean up!
 */
void Driver::mouse_scroll_movement(Leap::FingerList fingers)
{
    float dx_current = 0;
    float dy_current = 0;
    float z_abs = 0;
    float dx = 0;
    float dy = 0;
    float x;
    float y;
    bool some_change = false;

    z_abs = (fingers[0].tipPosition().z + fingers[0].tipPosition().z) / 2;

    if (z_abs > mouse_move_value)
    {
    	reinit_scroll = true;
    	return;
    }

    x = (fingers[0].tipPosition().x + fingers[1].tipPosition().x)/2;
    y = (fingers[0].tipPosition().y + fingers[1].tipPosition().y)/2;

    /*
     * there where no finges on some previos frames ... reinit erverything
     */
	if (reinit_scroll==true)
	{
		reinit_scroll = false;
		for (int i = 0; i<mouse_move_smooth_value; i ++)
		{
			dx_smooth[i] = 0;
			dy_smooth[i] = 0;
			old_x = x;
			old_y = y;
		}
	}

	if (z_abs != old_z)
	{
		old_z = z_abs;

		if ((z_abs < mouse_wheel_thresold))
		{
			dy_current = (old_y - y) * mouse_scroll_multipyer;
			dx_current = - (old_x - x) * mouse_scroll_multipyer;
			old_y = y;
			old_x = x;

			if ( (dy_current != 0) )
			{

				for (int i = 0; i<mouse_move_smooth_value-1; i++)
				{
					dx_smooth[i] = dx_smooth[i+1];
					dy_smooth[i] = dy_smooth[i+1];
					dx += dx_smooth[i];
					dy += dy_smooth[i];
				}

				dx_smooth[mouse_move_smooth_value-1] = dx_current;
				dy_smooth[mouse_move_smooth_value-1] = dy_current;
				dx += dx_current;
				dy += dy_current;

				dx = dx / mouse_move_smooth_value;
				dy = dy / mouse_move_smooth_value;

				input.move_rel_vert_wheel((int) std::round(dy));
				input.move_rel_hor_wheel((int) std::round(dx));

				some_change = true;
			}
		}

	}

    if (some_change)
    {
    	input.sync();
    }
}


void Driver::gesture(Leap::FingerList fingers)
{
	/*
	 * get the center of the fingers
	 */

	float x_center = 0;
	float y_center = 0;
	float z_center = 0;


	for (Leap::FingerList::const_iterator it = fingers.begin(); it != fingers.end(); it++ )
	{
		x_center = x_center + (*it).tipPosition().x;
		y_center = y_center + (*it).tipPosition().y;
		z_center = z_center + (*it).tipPosition().z;
	}

	x_center = x_center / 5;
	y_center = y_center / 5;
	z_center = z_center / 5;

	std::array<spherical,5> finger_postitons;


	for (Leap::FingerList::const_iterator it = fingers.begin(); it != fingers.end(); it++ )
	{
		int id = (*it).id()%5;
		finger_postitons[id].r = sqrt(
				pow((x_center - (*it).tipPosition().x),2) +
				pow((y_center - (*it).tipPosition().y),2) +
				pow((z_center - (*it).tipPosition().z),2)
				);

		finger_postitons[id].psi = atan2(sqrt(
				pow((x_center - (*it).tipPosition().x),2) +
				pow((y_center - (*it).tipPosition().y),2)
				),(z_center - (*it).tipPosition().z)
				) * 360 / (2*M_PI);

		finger_postitons[id].phi = atan2(
				(y_center - (*it).tipPosition().y),
				(x_center - (*it).tipPosition().x)
				) * 360 / (2*M_PI);
	}

	if (reinit_gesture)
	{
		std::copy(finger_postitons.begin(),finger_postitons.end(),old_finger_postitons.begin());
		incr_finger_postitons.fill(spherical(0,0,0));
		reinit_gesture = false;
	}
	else
	{
		std::array<spherical,5> d_finger_postitons;
		std::transform(finger_postitons.begin(),finger_postitons.end(),old_finger_postitons.begin(),
				d_finger_postitons.begin(), [](spherical s1, spherical s2){ return s2 - s1; });

		std::transform(d_finger_postitons.begin(),d_finger_postitons.end(),incr_finger_postitons.begin(),
				incr_finger_postitons.begin(), [](spherical s1, spherical s2){ return s1 + s2; });

		bool vol_up = true;
		for(spherical finger: incr_finger_postitons)
		{
			if ((finger.phi > this->vol_up_thr) && (vol_up))
			{
				vol_up = true;
			}
			else
			{
				vol_up = false;
			}
		}
		std::cout << vol_up;
		std::cout << std::endl;

		std::copy(finger_postitons.begin(),finger_postitons.end(),old_finger_postitons.begin());
	}




}

}
