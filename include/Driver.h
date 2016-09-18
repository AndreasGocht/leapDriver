/*
 * Driver.h
 *
 *  Created on: 20.02.2016
 *      Author: andreas
 */

#ifndef DRIVER_H_
#define DRIVER_H_

#include <Leap.h>
#include <InputInterface.h>
#include <array>
#include <mutex>
#include <atomic>
#include <thread>

namespace leapDriver
{

struct spherical{
	spherical()
	{}
	spherical(float r,float phi, float psi):
		r(r),phi(phi),psi(psi)
	{}

	float r = 0;
	float phi = 0;
	float psi = 0;

	spherical operator-(spherical s2)
	{
		spherical result;
		result.r = this->r - s2.r;
		result.phi = circle360(this->phi - s2.phi);
		result.psi = circle360(this->psi - s2.psi);

		return result;
	}

	spherical operator+(spherical s2)
	{
		spherical result;
		result.r = this->r + s2.r;
		result.phi = circle360(this->phi + s2.phi);
		result.psi = circle360(this->psi + s2.psi);

		return result;
	}

	private:
	float circle360(float val)
	{
		if (val < 0)
			return val + 360;
		if (val >=360)
			return val - 360;
		return val;
	}

};

class Driver:public Leap::Listener {
public:
	Driver();
	virtual ~Driver();

    virtual void onConnect(const Leap::Controller& );
    virtual void onDisconnect(const Leap::Controller& );

    void run();

    void process();
    void mouse_movement(Leap::Finger);
    void mouse_scroll_movement(Leap::FingerList);
    void gesture(Leap::FingerList);

private:
    InputInterface input;
    Leap::Controller controller;
    std::thread mainThread;

    /** const values */
    /** values for mouse movements*/
    const float mouse_move_multipyer = 3;
    const float mouse_scroll_multipyer = 1;
    const int mouse_move_smooth_value = 5;
    const float mouse_move_value = 20;
    const float mouse_click_prepare_value = -40;
    const float mouse_click_value = -50;
    const float mouse_click_release_value = -40;

    /** values for doubletap*/
    const float mouse_wheel_thresold = -40;

    /** values for gestures*/
    const float vol_up_thr = 30; /** rotate about vol_up_thr degrees to trigger vol_up*/


    std::mutex processMutex;
    std::mutex connectionMutex;
    std::atomic_bool connected;

    /**changing values */
    /** common moving values for mouse and doubletap */
    float old_x = 0;
    float old_y = 0;
    float old_z = 0;
    float *dx_smooth;
    float *dy_smooth;

    /** values for mouse movement*/
    bool reinit = false;
    bool btn_left_click = false;
    bool btn_left_clicked = false;

    /** values for doubletap */
    bool reinit_scroll = false;
    bool btn_doubletap_click = false;
    bool btn_doubletap_clicked = false;

    /** vlaues for gestures*/
    bool reinit_gesture = false;
    std::array<spherical,5> old_finger_postitons;
    std::array<spherical,5> incr_finger_postitons;


};
}

#endif /* DRIVER_H_ */
