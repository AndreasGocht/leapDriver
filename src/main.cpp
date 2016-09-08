#include <iostream>
#include <Driver.h>


int main(int argc, char** argv) {

    leapDriver::Driver driver;

    Leap::Controller controller;
    


    
    controller.addListener(driver);

    // Keep this process running until Enter is pressed
    std::cout << "Press Enter to quit..." << std::endl;
    std::cin.get();
    
    controller.removeListener(driver);
    

    return 0;
}
