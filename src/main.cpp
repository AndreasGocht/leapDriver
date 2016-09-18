#include <iostream>
#include <Driver.h>
#include <unistd.h>
#include <string>

int main(int argc, char** argv) {

	std::string config_path;
	int opt;

    while ((opt = getopt(argc, argv, "c:")) != -1) {
        switch (opt) {
        case 'c':
        	config_path = std::string(optarg);
            break;
        default: /* '?' */
        	break;
        }
    }

	std::cout<< "Starting program" << std::endl;
    leapDriver::Driver driver(config_path);

    std::cout<< "driver init done"<< std::endl;

    Leap::Controller controller;
    std::cout<< "created leap controler"<< std::endl;

    
    controller.addListener(driver);
    std::cout<< "added listener" << std::endl;


    // Keep this process running until Enter is pressed
    std::string input;
    while (true)
    {
    	std::cout << "Insert \"q\" to quit or \"r\" to reload the config file..." << std::endl;
    	std::getline(std::cin,input);

    	if (input == "r")
    	{
    		driver.load_config();
    	}
    	if (input == "q")
    	{
    		break;
    	}
    }

    
    controller.removeListener(driver);
    std::cout<< "removed listener"<< std::endl;
    

    return 0;
}
