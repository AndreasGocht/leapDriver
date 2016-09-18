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
	spherical(double r,double phi, double psi):
		r(r),phi(phi),psi(psi)
	{}

	double r = 0;
	double phi = 0;
	double psi = 0;

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
	double circle360(double val)
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
	Driver(std::string config_path);
	virtual ~Driver();

    virtual void onConnect(const Leap::Controller& );
    virtual void onDisconnect(const Leap::Controller& );

    void run();

    void process();
    void mouse_movement(Leap::Finger);
    void mouse_scroll_movement(Leap::FingerList);
    void gesture(Leap::FingerList);

    void load_config();

private:
    InputInterface input;
    Leap::Controller controller;
    std::thread mainThread;

    std::string config_path;

    /** const values */
    /** values for mouse movements*/
    double mouse_move_multipyer;
    double mouse_scroll_multipyer;
    int mouse_move_smooth_value;
    int mouse_scroll_smooth_value;
    double mouse_move_threashold;
    double mouse_click_prepare_value;
    double mouse_click_value;
    double mouse_click_release_value;

    double mouse_wheel_thresold = -40;

    /** values for gestures*/
    double vol_up_thr = 30; /** rotate about vol_up_thr degrees to trigger vol_up*/


    std::mutex processMutex;
    std::mutex connectionMutex;
    std::atomic_bool connected;

    /**changing values */
    /** common moving values for mouse and doubletap */
    double old_x = 0;
    double old_y = 0;
    double old_z = 0;
    std::vector<double> dx_smooth;
    std::vector<double> dy_smooth;

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
