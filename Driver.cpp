/*
 * Driver.cpp
 *
 *  Created on: 20.02.2016
 *      Author: andreas
 */

#include "Driver.h"

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
 * calles on new Frame from leap
 *
 * calls for every finger mouse_movement
 */
void Driver::onFrame(const Leap::Controller& controller) {

    const Leap::Frame frame = controller.frame();

    Leap::FingerList allTheFingers = frame.fingers().fingerType(Leap::Finger::TYPE_INDEX).extended();

    for(Leap::FingerList::const_iterator fl = allTheFingers.begin(); fl != allTheFingers.end(); fl++)
    {
        mouse_movement(*fl);
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
    std::cout << "\r" << (int)z_abs << std::flush;

    if (z_abs > mouse_move_value )
    {

        if (init==true)
        {
            init = false;
            for (int i = 0; i<mouse_move_smooth_value; i ++)
            {
                dx_smooth[i] = 0;
                dy_smooth[i] = 0;
                old_x = 0;
                old_y = 0;
            }
        }
    }
    else
    {
        //dealing with Z values
        // aclick migth happen.
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

        //freeze if a click migth happen
        if (z_abs > mouse_click_prepare_value)
        {
            if (init == false)
            {
                old_x = (finger).tipPosition().x;
                old_y = (finger).tipPosition().y;
                init = true;
            }
            else
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
        }
    }

    if (some_change)
    {
    	input.sync();
    }
}

}
