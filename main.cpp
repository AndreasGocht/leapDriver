#include <iostream>
#include "Driver.h"    leapDriver::DriverTouch driver;

#include "DriverTouch.h"


int main(int argc, char** argv) {

    leapDriver::Driver driver;
    //leapDriver::DriverTouch driver;

    Leap::Controller controller;
    


    
    controller.addListener(driver);

    // Keep this process running until Enter is pressed
    std::cout << "Press Enter to quit..." << std::endl;
    std::cin.get();
    
    controller.removeListener(driver);
    

    return 0;
}
