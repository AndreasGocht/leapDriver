/*
 * InputInterface.h
 *
 *  Created on: 20.02.2016
 *      Author: andreas
 */

#ifndef INPUTINTERFACE_H_
#define INPUTINTERFACE_H_

#include <string>
#include <stdexcept>
#include <cstring>

namespace leapDriver{

	class InputInterfaceError : public std::runtime_error
	{
	public:
		InputInterfaceError(std::string what_arg, int err)
		: std::runtime_error(what_arg + std::string(": ") +  std::string(": ") + std::string(strerror(err)))
		{
		}
	};

	class InputInterface {
	public:
		InputInterface();
		virtual ~InputInterface();

		void btn_left_click();
		void btn_left_release();
		void btn_doubletap_click();
		void btn_doubletap_release();
		void move_rel_vert_wheel(int dv);
		void move_rel_hor_wheel(int dh);
		void move_rel(int dx, int dy);
		void sync();

	private:
		int file;
	};

}
#endif /* INPUTINTERFACE_H_ */
