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

#include <Driver.h>
#include <cmath>
#include <json.hpp>
#include <fstream>
#include <log.h>


namespace leapDriver
{

using json = nlohmann::json;

Driver::Driver(bool service):
		Listener(),
		config_path(""),
		connected(false),
		service(service)
{
	load_config();
}

Driver::Driver(std::string config_path,bool service):
		Listener(),
		config_path(config_path),
		connected(false),
		service(service)
{
	load_config();
}

Driver::~Driver() {
	//simulate disconnect behavior.
	onDisconnect(controller);
}


/* This function controlls the leap connect event.
 *
 */
void Driver::onConnect(const Leap::Controller& controller) {

	/** Check if the mainthread is jonable. If not nothing is running
	 * and it is supposingly safe to create a new thread. I am not sure here.
	 *
	 */
    if(!mainThread.joinable())
    {
    	connected = true;
    	logging::info() << "[Driver] Connected" ;
    	mainThread = std::thread(&Driver::run,this);
    }
    else
    {
    	logging::error() << "[Driver] Old thread still running, doing nothing";
    }

}

/*this function sets connected to false
 *
 */
void Driver::onDisconnect(const Leap::Controller& controller)
{
	connected = false;
	//wait for the old process to finish
	if(mainThread.joinable())
		mainThread.join();

	logging::info() << "[Driver] Disconnected";
}

/*starts the class if run as service
 *
 */
void Driver::start()
{
	mainThread = std::thread(&Driver::run,this);
	mainThread.join();
}


/* This is the main running function.
 *
 * It is called by onConnect or by the start() function,
 * if this is a service.
 *
 * If this class is not run as a service, this function
 * runs as long as connected is true. connected is set by
 * onConnect and onDisconnect.
 *
 * The function gets the current timepoint, calls process, and
 * then sleeps timepoint + 20 ms
 *
 * If this function is run as a Service, it checks if the
 * controller is connected. If so, it behaves like the non service
 * variant. If it is not connectes, it sleeps for 1 second.
 * and checks again.
 *
 *
 */
void Driver::run()
{
	while(connected || service)
	{
		if(controller.isConnected())
		{
			auto get_next_frame = std::chrono::steady_clock::now() + std::chrono::milliseconds(20);
			process();
			std::this_thread::sleep_until(get_next_frame);
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}

	}
}

/* This function is called each time a frame shall be processed.
 *
 * It is the main processing function, and therefore cares for
 * thread safety.
 *
 * It distinguishes between four different cases:
 *
 * * one finger in the field: classical relative movement
 * * two finger : scrolling
 * * five fingers gestures
 * * zero fingers: re init
 *
 *
 */
void Driver::process() {

	/* try to lock the mutex
	 * if we still doning computation, skip this frame
	 */
	if (!processMutex.try_lock())
	{
		logging::debug()<< "[Driver] missed frame";
		return;
	}
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

    processMutex.unlock();

}

/**This function simulates classical mouse movement.
 *
 * @param finger a single finger responsible for the movements and
 * clicks
 *
 */
void Driver::mouse_movement(Leap::Finger finger)
{

    double dx_current = 0;
    double dy_current = 0;
    double z_abs = 0;
    double dx = 0;
    double dy = 0;
    bool some_change = false;

    z_abs = (finger).tipPosition().z;

    if (z_abs > mouse_move_threashold)
    {
    	reinit = true;
    	return;
    }


    /*
     * there where no fingers on some previous frames ... reinit erverything
     */
	if (reinit==true)
	{
		reinit = false;

		dx_smooth.clear();
		dy_smooth.clear();
		dx_smooth.resize(mouse_move_smooth_value,0);
		dy_smooth.resize(mouse_move_smooth_value,0);

		old_x = (finger).tipPosition().x;
		old_y = (finger).tipPosition().y;
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

/** Implements scrolling with two fingers
 *
 * @param fingers the fingers of the heand responsible for scrolling
 */
void Driver::mouse_scroll_movement(Leap::FingerList fingers)
{
    double dx_current = 0;
    double dy_current = 0;
    double z_abs = 0;
    double dx = 0;
    double dy = 0;
    double x;
    double y;
    bool some_change = false;

    z_abs = (fingers[0].tipPosition().z + fingers[0].tipPosition().z) / 2;

    if (z_abs > mouse_move_threashold)
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

		dx_smooth.clear();
		dy_smooth.clear();
		dx_smooth.resize(mouse_scroll_smooth_value,0);
		dy_smooth.resize(mouse_scroll_smooth_value,0);

		old_x = x;
		old_y = y;
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

				for (int i = 0; i<mouse_scroll_smooth_value-1; i++)
				{
					dx_smooth[i] = dx_smooth[i+1];
					dy_smooth[i] = dy_smooth[i+1];
					dx += dx_smooth[i];
					dy += dy_smooth[i];
				}

				dx_smooth[mouse_scroll_smooth_value-1] = dx_current;
				dy_smooth[mouse_scroll_smooth_value-1] = dy_current;
				dx += dx_current;
				dy += dy_current;

				dx = dx / mouse_scroll_smooth_value;
				dy = dy / mouse_scroll_smooth_value;

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

/** processing of gestures.
 *
 * Implements at the moment processing of rotating hands.
 * No actions jet.
 *
 * @param fingers fingers in the movement
 *
 */
void Driver::gesture(Leap::FingerList fingers)
{
	/*
	 * get the center of the fingers
	 */

	double x_center = 0;
	double y_center = 0;
	double z_center = 0;


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
//		std::cout << vol_up;
//		std::cout << std::endl;

		std::copy(finger_postitons.begin(),finger_postitons.end(),old_finger_postitons.begin());
	}

}

void Driver::load_config()
{
	std::ifstream file(config_path);
	json j;

	try{
		if (!file)
		{
			throw std::runtime_error(std::string("Can't open the config file \"") + config_path + std::string("\": ") + std::string(strerror(errno)));
		}
		j = json::parse(file);

		mouse_move_multipyer 		= j["mouse_move_multipyer"];
		mouse_scroll_multipyer 		= j["mouse_scroll_multipyer"];
		mouse_move_smooth_value 	= j["mouse_move_smooth_value"];
		mouse_scroll_smooth_value 	= j["mouse_scroll_smooth_value"];
		mouse_move_threashold 		= j["mouse_move_threashold"];
		mouse_click_prepare_value 	= j["mouse_click_prepare_value"];
		mouse_click_value 			= j["mouse_click_value"];
		mouse_click_release_value 	= j["mouse_click_release_value"];
		mouse_wheel_thresold 		= j["mouse_wheel_thresold"];
		vol_up_thr 					= j["vol_up_thr"];

		file.close();

	}
	catch (std::exception& e)
	{
		/* if anything wents wrong lets hope for an exception.
		 * Lets catch it and print an error (avoiding killing the whole
		 * program.
		 */
		logging::warn() << "[Driver] Something went wrong while loading the configuration";
		logging::warn() << "[Driver] " << e.what();
		logging::warn() << "[Driver] Loading default!.";

		mouse_move_multipyer = 3;
		mouse_scroll_multipyer = 1;
		mouse_move_smooth_value = 5;
		mouse_scroll_smooth_value = 3;
		mouse_move_threashold = 20;
		mouse_click_prepare_value = -40;
		mouse_click_value = -50;
		mouse_click_release_value = -40;
		mouse_wheel_thresold = -40;
		vol_up_thr = 30;

		return;
	}

}


}
