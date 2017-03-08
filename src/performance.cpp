#pragma once
#include "performance.h"

#ifdef _WIN32

#define time_struct LARGE_INTEGER
#define getTime(x) (QueryPerformanceCounter(x))

#elif __unix__

#define time_struct timespec
#define getTime(x) (clock_gettime(CLOCK_THREAD_CPUTIME_ID, x))

#endif

/*
	Store information about one timing entry.
*/
struct ENTRY {
	time_struct start;
	int_fast64_t elapsed;
	std::string name;
};

/*
	Keep track of all time intervals for a given iteration.
*/
std::vector<ENTRY> entries;

/*
	Keep track of which entry is next to be started.
	Also used as id for associationg starttime with endtime in entries.
*/
uint_fast32_t current = 0;

#ifdef _WIN32
/*
	Ticks per second as QueryPerformanceCounter() returns current amount of ticks
*/
LARGE_INTEGER frequency;
#endif

/*
	Whether the elapsed time should be written to the console
	when stopTimer() is called.
*/
bool printTime = false;

namespace performance {

	// TODO GPU TIMERS
	
	void initialize(bool logTime) {
#ifdef _WIN32
		QueryPerformanceFrequency(&frequency);
#endif
		printTime = logTime;
	}

	uint_fast32_t startTimer(std::string name) {
		ENTRY entry;
		getTime(&entry.start);
		entry.name = name;
		entries.push_back(entry);
		return current++;
	}

	void stopTimer(size_t id) {
		time_struct stop;
		getTime(&stop);

#ifdef _WIN32
		__int64 ticks = stop.QuadPart - entries[id].start.QuadPart;
		ticks *= 1000000;
		entries[id].elapsed = ticks / frequency.QuadPart;
#elif __unix__
		time_struct elapsed;
		timespec_diff(&entries[id].start, &stop, &elapsed);
		entries[id].elapsed = (elapsed.tv_sec / 1000000) + (elapsed.tv_nsec * 1000);
#endif
		if (printTime)
		{
			std::cout << entries[id].name << ":  " << entries[id].elapsed << " us" << std::endl;
		}
	}
	
	void gui() {
		// unimplemented

		current = 0;
		entries.clear();
	}

#ifdef __unix__
	void timespec_diff(struct timespec *start, struct timespec *stop, struct timespec *result) {
		if (stop->tv_nsec < start->tv_nsec) 
		{
			result->tv_sec = stop->tv_sec - start->tv_sec - 1;
			result->tv_nsec = stop->tv_nsec - start->tv_nsec + 1000000000;
		}
		else
		{
			result->tv_sec = stop->tv_sec - start->tv_sec;
			result->tv_nsec = stop->tv_nsec - start->tv_nsec;
		}
	}
#endif
}

