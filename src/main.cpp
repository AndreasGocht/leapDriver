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

#include <iostream>
#include <Driver.h>
#include <unistd.h>
#include <string>
#include <log.h>
#include <memory>
#include <thread>

void print_help(int argc, char** argv)
{
	std::cout << std::string(argv[0]) << " [-c <config.json>] [-d debug level]\n";
	std::cout << "\t -c path to the configuration \n";
	std::cout << "\t -d debug level. One of: \n";
	std::cout << "\t\t DEBUG, INFO, WARN, ERROR \n";
	std::cout << "\t -s bool if service or not \n";
	std::cout << "\t -h this help \n";
	std::cout << std::endl;
	exit(0);
}

int main(int argc, char** argv) {

	std::string config_path;
	std::string debug_level;
	std::shared_ptr<leapDriver::Driver> driver;
	std::unique_ptr<Leap::Controller> controller;
	std::string service_str;
	bool service = false;
	int opt;

    while ((opt = getopt(argc, argv, "c:d:hs:")) != -1) {
        switch (opt) {
        case 'c':
        	config_path = std::string(optarg);
            break;
        case 'd':
        	debug_level = std::string(optarg);
            break;
        case 's':
        	service_str = std::string(optarg);
            break;
        case 'h':
        	print_help(argc, argv);
            break;
        default: /* '?' */
        	break;
        }
    }

    auto level = severity_from_string(debug_level,
            nitro::log::severity_level::info);
    leapDriver::log::set_min_severity_level(level);

    leapDriver::logging::info()<< "Starting program";

    if (service_str=="true")
    {
    	service = true;
    	leapDriver::logging::info()<< "Running as service";
    }

    leapDriver::logging::info()<< "Create driver";
    try{
    	driver = std::make_shared<leapDriver::Driver>(config_path,service);
    } catch(const std::exception& e)
    {
    	leapDriver::logging::fatal() << e.what();
    	leapDriver::logging::fatal() << "Finishing";

    	exit(-1);
    }

    if (!service)
    {
		leapDriver::logging::info()<< "Create leap controller";
		controller.reset(new Leap::Controller);

		leapDriver::logging::info()<< "Add listener";
		controller->addListener(*driver);
    }
    


    // Keep this process running until Enter is pressed
    std::string input;
    while (true)
    {
    	if(service)
    	{
        	driver->start();
    	}
    	else
    	{
			std::cout << "Insert \"q\" to quit or \"r\" to reload the config file..." << std::endl;
			std::getline(std::cin,input);

			if (input == "r")
			{
				driver->load_config();
			}
			if (input == "q")
			{
				break;
			}
    	}
    }

    if(!service)
    {
    	leapDriver::logging::info()<< "Remove listener";
    	controller->removeListener(*driver);
    }

    leapDriver::logging::info()<< "Delete driver";
    controller.reset(nullptr);

    leapDriver::logging::info()<< "ByBy";
    return 0;
}
