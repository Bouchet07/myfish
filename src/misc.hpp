#ifndef MISC_H
#define MISC_H

#include <chrono>
#include <unistd.h>
#include <iostream>

#include "types.hpp"

#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/select.h>
#endif


inline int get_time_ms() {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());

    return static_cast<int>(duration.count());
}

inline bool input_waiting() {
#ifdef _WIN32
    static bool init = false;
    static HANDLE inh;
    static bool pipe;

    if (!init) {
        init = true;
        inh = GetStdHandle(STD_INPUT_HANDLE);
        DWORD dw;
        pipe = !GetConsoleMode(inh, &dw);
        if (!pipe) {
            SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT));
            FlushConsoleInputBuffer(inh);
        }
    }

    DWORD dw;
    if (pipe) {
        return PeekNamedPipe(inh, nullptr, 0, nullptr, &dw, nullptr) != 0;
    } else {
        GetNumberOfConsoleInputEvents(inh, &dw);
        return dw > 1;
    }
#else
    fd_set readfds;
    struct timeval tv;
    FD_ZERO(&readfds);
    FD_SET(fileno(stdin), &readfds);
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    select(16, &readfds, nullptr, nullptr, &tv);
    return FD_ISSET(fileno(stdin), &readfds);
#endif
}

// read GUI/user input
inline void read_input(Time &time){
    // bytes to read holder
    int bytes;
    // GUI/user input
    char input[256] = "", *endc;
    // "listen" to STDIN
    if (input_waiting()){
        time.stopped = 1;// tell engine to stop calculating
        
        do{ // loop to read bytes from STDIN
            bytes=read(fileno(stdin), input, 256); // read bytes from STDIN
        }
        while (bytes < 0);// until bytes available
        
        endc = strchr(input,'\n'); // searches for the first occurrence of '\n'
        
        if (endc) *endc=0; // if found new line set value at pointer to 0
        
        if (strlen(input) > 0){ // if input is available
            if      (!strncmp(input, "quit", 4)) time.quit = 1; // tell engine to terminate exacution 
            else if (!strncmp(input, "stop", 4)) time.quit = 1; // tell engine to terminate exacution 
        }   
    }
}

// a bridge function to interact between search and GUI input
static void communicate(Time &time) {
	// if time is up break here
    if(time.timeset == 1 && get_time_ms() > time.stoptime) time.stopped = 1; // tell engine to stop calculating
	
	//read_input(time); // read GUI input
}

#endif