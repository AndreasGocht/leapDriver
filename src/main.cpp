#include <iostream>
#include <Driver.h>


int main(int argc, char** argv) {

	std::cout<< "Starting program" << std::endl;
    leapDriver::Driver driver;

    std::cout<< "driver init done"<< std::endl;

    Leap::Controller controller;
    std::cout<< "created leap controler"<< std::endl;

    


    
    controller.addListener(driver);
    std::cout<< "added listener" << std::endl;

    // Keep this process running until Enter is pressed
    std::cout << "Press Enter to quit..." << std::endl;
    std::cin.get();
    
    controller.removeListener(driver);
    std::cout<< "removed listener"<< std::endl;
    

    return 0;
}
