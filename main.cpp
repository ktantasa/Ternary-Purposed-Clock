//clock program
// Please see in the timezone.txt files for the time offsets

#include <iostream>
#include <iomanip>
#include <fstream>
#include <memory>
#include <future>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <string>
#include <cstring>
#include <map>
#include <chrono>
#include <ctime>
#include "clearScreen.hpp"

std::map<std::string,float> timeZones;

/*
 * Display the menu for the user interface.
 */
void showMenu(){
    std::cout << "Main Menu" << std::endl;
    std::cout << "1. Show Clock" << std::endl;
    std::cout << "2. Convert time" << std::endl;
    std::cout << "3. Stopwatch" << std::endl;
    std::cout << "4. Exit Program" << std::endl;
}

/*
 * Show the menu and return the user's selection.
 */
int getMenuChoice(){
    showMenu();
    std::string choice;
    getline(std::cin, choice);
    return stoi(choice);                        // String to interger conversion
}

/*
 * Load the timezone.txt file and scan the time zones into the map structure.
 */
void loadTimeZones(){
    std::ifstream file;
    file.open("timezone.txt", std::fstream::in); // Open the timezone.txt file in read mode.
    std::string input;
    while(getline(file, input)){                 // Loop over all the lines.
        char *name = strtok(&input[0], ":");
        char *value = strtok(nullptr, ":");     // If strtok is given a null pointer,
                                                // It cointinues to use the same string
        timeZones.insert(std::pair<std::string,float>(name, atof(value)));          // Place time zone and offset into the map.
    }
    file.close();
}

/*
 * The convert time zone function - Please see the timezone.txt file for the timezone acronym. Please only use the acronym of the timezone for your start timezone and and destination time zone.
 */
void convertTime(){
    std::cout<< "Time Converter" << std::endl;
    std::string StartTimeZone;
    while(true){                                                //guarding against invalid time zone.
        std::cout<< "Please input your current time zone: ";
        getline(std::cin, StartTimeZone);
        if(timeZones.count(StartTimeZone)>0){
            break;
        }
        std::cout<<"Invalid Time Zone!"<<std::endl;
    }
    std::cout<< "Please input the time to convert: ";
    std::string StartTime;
    getline(std::cin, StartTime);
    std::string DestinationTimeZone;
    while(true){
        std::cout<< "Please input your destination time zone: ";
        getline(std::cin, DestinationTimeZone);
        if(timeZones.count(DestinationTimeZone)>0){
            break;
        }
        std::cout<<"Invalid Time Zone!"<<std::endl;
    }
    size_t index = StartTime.find(":");
    float time = 0;
    if(index!=std::string::npos){
        char *hour = strtok(&StartTime[0], ":");            // String token function, returns all of the string up to the first instance of ":".
        char *minute = strtok(nullptr, ":");                // String token function, continues on previous string up to next instance of ":".
        time = atof(hour)+(atof(minute)/60);                // Address to float, converts char pointer to float value.
    }
    else{
        time = stof(StartTime);                             // String to float.
    }
    
    float startOffset = timeZones[StartTimeZone];
    float destinationOffset = timeZones[DestinationTimeZone];
    time+=destinationOffset-startOffset;
    int hours = (int)time;
    int minutes = (time-hours)*60;                          // Convert time into a decimal representation of minutes, which is then multiplied by 60.
    std::cout<<"Your destination time is "<< hours << ":" << std::setfill('0')<<std::setw(2)<<minutes <<std::endl;
}

/*
 * Show the clock and the dates.
 */
void showClock(){
    std::chrono::time_point<std::chrono::system_clock> currentTime = std::chrono::system_clock::now();      // Implemented Chrono from standard library
    std::time_t timeOBJ = std::chrono::system_clock::to_time_t(currentTime);
    std::cout<< "Current Time: " << std::ctime(&timeOBJ)<<std::endl;
}

/*
 * Stop Watch function
 */
void stopWatch(){
    std::mutex stop;                            // Mutex provides memory safety across threads.
    bool isRunning = true;
    std::thread waitForInput = std::thread([&]{     //creating threads for user input.
        std::string temp;
        while(isRunning && getline(std::cin, temp)){                                    // Run until Q is typed in.
            std::unique_lock<std::mutex> lock = std::unique_lock<std::mutex>(stop);     // unique locks activates mutex to prevent another
            if(temp == "q" || temp == "Q"){                                             // thread from accessing memory
                isRunning = false;
            }
            lock.unlock();                                                              // deactivates "mutex".
        }
        std::unique_lock<std::mutex> lock = std::unique_lock<std::mutex>(stop);
        isRunning = false;
        lock.unlock();
    });
    std::thread displayClock = std::thread([&]{                                        // display stopwatch thread.
        std::chrono::time_point<std::chrono::system_clock> currentTime = std::chrono::system_clock::now();
        std::chrono::time_point<std::chrono::system_clock> startTime = std::chrono::system_clock::now();
        std::chrono::duration<double> elaspeSecond = currentTime - startTime;
        bool currentRunning = true;
        while(currentRunning){
            clearScreen();
            currentTime = std::chrono::system_clock::now();
            elaspeSecond = currentTime - startTime;
            int seconds = elaspeSecond.count();                                        // converting seconds into stopwatch values.
            int milli = (elaspeSecond.count()-seconds)*100;
            int minutes = seconds/60;
            int hours = minutes/60;
            minutes = minutes%60;
            seconds = seconds%60;
            std::cout<< "Press Q then enter to stop: " << std::setfill('0')<<std::setw(2)<< hours << ":"    // Setfill and setw transforms value into
                     << std::setfill('0')<<std::setw(2) << minutes << ":"                                   // 2 digits with leading 0s.
                     << std::setfill('0')<<std::setw(2)<< seconds << ":"
                     << std::setfill('0')<<std::setw(2)<< milli <<std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));                                     // Sleep for 10 milliseconds for other threads
            std::unique_lock<std::mutex> lock = std::unique_lock<std::mutex>(stop);
            currentRunning = isRunning;
            lock.unlock();
        }
    });
    
    waitForInput.join();    // ".join" function waits for other functions to be completed.
    displayClock.join();
}

/*
 * Main function
 */
int main(int argc, const char * argv[]) {
    loadTimeZones();
    int menuChoice;
    do{
        menuChoice = getMenuChoice();
        clearScreen();
        switch (menuChoice) {
            case 1:
                showClock();
                break;
                
            case 2:
                convertTime();
                break;
            case 3:
                stopWatch();
                break;
        }
    } while(menuChoice!=4);
    return 0;
}
