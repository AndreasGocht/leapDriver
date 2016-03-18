/*
 * Driver.h
 *
 *  Created on: 20.02.2016
 *      Author: andreas
 */

#ifndef DRIVER_H_
#define DRIVER_H_

#include "../LeapSDK/include/Leap.h"
#include "InputInterface.h"

namespace leapDriver
{

class Driver:public Leap::Listener {
public:
	Driver();
	virtual ~Driver();

    virtual void onConnect(const Leap::Controller& );
    virtual void onFrame(const Leap::Controller&);
    void mouse_movement(Leap::Finger);

private:
    InputInterface input;

    float mouse_move_multipyer = 3;
    int mouse_move_smooth_value = 5;
    float mouse_move_value = 10;
    float mouse_click_prepare_value = -40;
    float mouse_click_value = -50;
    float mouse_click_release_value = -40;


    //glaibal values
    float old_x = 0;
    float old_y = 0;
    float old_z = 0;
    float *dx_smooth;
    float *dy_smooth;
    bool init = false;
    bool btn_left_click = false;
    bool btn_left_clicked = false;


};
}

#endif /* DRIVER_H_ */
