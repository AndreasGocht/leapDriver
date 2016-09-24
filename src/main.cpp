#include <iostream>
#include <Driver.h>
#include <unistd.h>
#include <string>
#include <log.h>
#include <memory>

int main(int argc, char** argv) {

	std::string config_path;
	std::string debug_level;
	std::unique_ptr<leapDriver::Driver> driver;
	std::unique_ptr<Leap::Controller> controller;
	int opt;

    while ((opt = getopt(argc, argv, "c:d:")) != -1) {
        switch (opt) {
        case 'c':
        	config_path = std::string(optarg);
            break;
        case 'd':
        	debug_level = std::string(optarg);
            break;
        default: /* '?' */
        	break;
        }
    }

    auto level = severity_from_string(debug_level,
            nitro::log::severity_level::info);
    leapDriver::log::set_min_severity_level(level);

    leapDriver::logging::info()<< "Starting program";

    leapDriver::logging::info()<< "Create driver";
    try{
    	driver.reset(new leapDriver::Driver(config_path)) ;
    } catch(const std::exception& e)
    {
    	leapDriver::logging::fatal() << e.what();
    	leapDriver::logging::fatal() << "Finishing";
    	driver.reset(nullptr);

    	exit(-1);
    }

    leapDriver::logging::info()<< "Create leap controller";
    controller.reset(new Leap::Controller);


    leapDriver::logging::info()<< "Add listener";
    controller->addListener(*driver);
    


    // Keep this process running until Enter is pressed
    std::string input;
    while (true)
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

    leapDriver::logging::info()<< "Remove listener";
    controller->removeListener(*driver);
    
    leapDriver::logging::info()<< "Delete driver";
    driver.reset(nullptr);
    controller.reset(nullptr);

    leapDriver::logging::info()<< "ByBy";
    return 0;
}
