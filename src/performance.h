#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <stdint.h>
#include <utility>
#include "imgui/imgui.h"

#ifdef _WIN32
#include <Windows.h>
#elif __unix__
#include <time.h>
// Cant test linux things on windows, which of these are needed (none?)
#include <ctime>
#include <sys/time.h> 
#include <sys/timeb.h>
#endif

namespace performance {

	/*
		Initializes the performance module
	*/
	void initialize(bool logTime = false);

	/*
		Start a timer and returns the corresponding id number
		Should this method be atomic?
	*/
	uint_fast32_t startTimer(std::string name);

	/*
		Stops the given timer.
		Should this method be atomic?
	*/
	void stopTimer(size_t id);

	/*
		Reset counters and store timing information for visualization
	*/
	void next();

	/*
		Draws the performance statistics
	*/
	void gui(bool* show);

#ifdef __unix__
	/*  
		Calculates the time difference between start and stop and returns the result in result
		https://gist.github.com/diabloneo/9619917
	    https://www.gnu.org/software/libc/manual/html_node/Elapsed-Time.html
		https://www.guyrutenberg.com/2007/09/22/profiling-code-using-clock_gettime/
	*/
	void timespec_diff(struct timespec *start, struct timespec *stop, struct timespec *result);
#endif
}

