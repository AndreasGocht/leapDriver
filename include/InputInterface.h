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
		: std::runtime_error(std::string("InputInterfaceError: ") + what_arg + std::string(", ") + std::string(strerror(err)))
		{
		}
	};

	class InputInterface {
	public:
		InputInterface();
		~InputInterface();

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
