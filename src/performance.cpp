#pragma once
#include "performance.h"
#include "glad/glad.h"

#ifdef _WIN32

#define time_struct LARGE_INTEGER
#define getTime(x) (QueryPerformanceCounter(x))

#elif __unix__

#define time_struct timespec
#define getTime(x) (clock_gettime(CLOCK_THREAD_CPUTIME_ID, x))

#endif

/*
	Overload access such that out of bounds index clamp to last value
*/
struct gList : std::vector<uint_fast64_t>
{
	int operator[] (int idx) {
		if (this->size() > idx)
			return (*(this->begin() + idx));
		else
			return (*(this->end()-1));
	}
};

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

/*
	List for storing time data used to visualize performance
*/
std::vector< gList > graphData(90);


// **** GPU Timekeeping ********
//
//const int QUERY_COUNTERS = 90;
//
//GLuint gpuQueryIDStart[2][QUERY_COUNTERS];
//GLuint gpuQueryIDEnd[2][QUERY_COUNTERS];
//
//int g_gpu_timer_counter = 0;

// Keep track of which entry is next to be started for gpu timekeeping.
//uint_fast32_t gpuCurrent = 0;
//

GLint64 startTime, stopTime;
GLuint queryID[2];

/*
Keep track of all time intervals for a given iteration.
*/
std::vector<ENTRY> gpuEntries;

/*
List for storing time data used to visualize GPU performance
*/
gList gpuGraphData;

// **** END GPU Timekeeping ****

/*
	Maximum value of graphData, used to determine scaling of visualization graph	
*/
int_fast64_t max = 0;

/*
	List of colors used in graph TODO use ImCol
*/
std::vector<ImColor> colors = {
	ImColor(152, 35, 149),
	ImColor(0, 135, 203),
	ImColor(37, 9, 99),
	ImColor(213, 155, 17),
	ImColor(142, 8, 14),
	ImColor(7, 210, 26),
	ImColor(0.f,0.f,0.f,1.f),
	ImColor(0.f,0.f,0.f,1.f),
	ImColor(0.f,0.f,0.f,1.f),
	ImColor(0.f,0.f,0.f,1.f) };

namespace performance {

	void initialize(bool logTime) {
#ifdef _WIN32
		QueryPerformanceFrequency(&frequency);
#endif
        
        //// generate two queries
        glGenQueries(2, queryID);
        // dummy query to prevent OpenGL errors from popping out
        glQueryCounter(queryID[0], GL_TIMESTAMP);
        gpuGraphData.push_back(0.0f);


		printTime = logTime;
		for (uint32_t i = 0; i < graphData.size(); i++)
		{
			if (graphData[i].size() == 0)
				graphData[i].push_back(0.0f);
		}
	}

    // **** GPU TIMERS ****
    void gpuTimerStart()
    {
        ENTRY entry;
        getTime(&entry.start);
        entry.name = "GPU";
        gpuEntries.push_back(entry);

        glQueryCounter(queryID[0], GL_TIMESTAMP);
    }

    void gpuTimerStop()
    {
        time_struct stop;
        getTime(&stop);

        glQueryCounter(queryID[1], GL_TIMESTAMP);
        // wait until the results are available **** This halts the program until rendering is done. Might want to revise this.
        unsigned int stopTimerAvailable = 0;
        while (!stopTimerAvailable) {
            glGetQueryObjectiv(queryID[1],
                GL_QUERY_RESULT_AVAILABLE,
                (GLint *)&stopTimerAvailable);
        }

        // get query results
        glGetQueryObjectui64v(queryID[0], GL_QUERY_RESULT, (GLuint64 *)&startTime);
        glGetQueryObjectui64v(queryID[1], GL_QUERY_RESULT, (GLuint64 *)&stopTime);

#ifdef _WIN32
        GLint64 ticks = stopTime - startTime;
        ticks *= 1000000;
        gpuEntries[gpuEntries.size()-1].elapsed = ticks;// / frequency.QuadPart; // Will always be most recent timer (so far)
#elif __unix__
        time_struct elapsed;
        timespec_diff(&entries[id].start, &stop, &elapsed);
        entries[id].elapsed = (elapsed.tv_sec / 1000000) + (elapsed.tv_nsec * 1000);
#endif
        

        //printf("Time spent on the GPU: %f ms\n", (stopTime - startTime) / 1000000.0);
    }

    // **** END GPU TIMERS ****


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

	void next()
	{
		// Add current iteration times to our graphData
		static int offset = 0;
		offset = (offset + 1) % graphData.size();
		graphData[offset].clear();
		graphData[offset].push_back(0.0f);
		int_fast64_t accum = 0;
		for (auto entry : entries)
		{
			accum += entry.elapsed;
			graphData[offset].push_back(accum);
		}

		// Find maximum value so that we can scale our graph
		max = graphData[0][graphData[0].size()];
		for (int i = 1; i < graphData.size(); i++)
		{
			max = max > graphData[i][graphData[i].size()] ? max : graphData[i][graphData[i].size()];
		}

		current = 0;
		entries.clear();

        gpuGraphData.clear();
        gpuGraphData.push_back(0.0f);
        accum = 0;
        for (auto entry : gpuEntries)
        {
            accum += entry.elapsed;
            gpuGraphData.push_back(accum);
        }
	}

	void gui(bool* show) {

		if (!*show)
		{
			return;
		}

		ImGui::SetNextWindowSize(ImVec2(350, 560), ImGuiSetCond_FirstUseEver);
		if (!ImGui::Begin("Performance Diagnostics", show))
		{
			ImGui::End();
			return;
		}

		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		auto a = ImGui::GetIO();

		// Allow the amount of data points to change
		static int graphDataSize = graphData.size();
		static int last = graphDataSize;
		ImGui::SliderInt("Number of datapoints", &graphDataSize, 1, 500);
		if (graphDataSize != last)
		{
			graphData.resize(graphDataSize);
			last = graphDataSize;
			initialize(printTime);
			max = 0;
		}

		const ImVec2 p = ImGui::GetCursorScreenPos();
		float wWidth = ImGui::CalcItemWidth();
		ImVec2 spacing = ImVec2(2.0f, 20.0f);
		float graphHeight = (ImGui::GetContentRegionAvail().y / 2) - spacing.y;
		float dx = (wWidth) / graphData.size();
		float x = p.x + spacing.x;
		float y = p.y + spacing.y + graphHeight;
        float gy = y + graphHeight + spacing.y;
		float graphRightEdge = p.x + wWidth;
		float radius = 10.f;
		float middley = ImGui::GetContentRegionAvail().y / 2.f;
		float startx = graphRightEdge + (ImGui::GetContentRegionAvail().x - wWidth) / 4.f;
		float starty = middley - entries.size() * 20.f;
		float rowHeight = 40.f;
		float padding = 10.f;

		// Graph frame
		draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + (graphData.size() - 1) * dx, y - graphHeight), ImColor(164, 164, 164, 120));
        draw_list->AddRectFilled(ImVec2(x, gy), ImVec2(x + (graphData.size() - 1) * dx, gy - graphHeight), ImColor(164, 164, 164, 120)); // GPU

		// Draw graph
		for (uint32_t i = 0; i < graphData.size() - 1; i++)
		{
			for (uint32_t j = 0; j < graphData[i].size() - 1; j++)
			{
				draw_list->AddQuadFilled(
					ImVec2(x, y - (graphData[i][j] / (float)max)*graphHeight),
					ImVec2(x + dx, y - (graphData[i + 1][j] / (float)max)*graphHeight),
					ImVec2(x + dx, y - (graphData[i + 1][j + 1] / (float)max)*graphHeight),
					ImVec2(x, y - (graphData[i][j + 1] / (float)max)*graphHeight),
					colors[j]
				);
			}
			x += dx;
		}

        int_fast64_t gpuMax = gpuGraphData[gpuGraphData.size()];
       x = p.x + spacing.x;
       for (uint32_t i = 0; i < graphData.size() - 1; i++)
       {
           draw_list->AddQuadFilled(
               ImVec2(x, gy), //- (gpuGraphData[i] / (float)max)*graphHeight),
               ImVec2(x + dx, gy), // -(gpuGraphData[i + 1] / (float)max)*graphHeight),
               ImVec2(x + dx, gy - (gpuGraphData[i + 1] / (float)gpuMax)*graphHeight),
               ImVec2(x, gy - (gpuGraphData[i] / (float)gpuMax)*graphHeight),
               colors[3]
           );
            x += dx;
        }
		
		// Text information
		std::string yMax = "Max: " + std::to_string(max / 1000.0f).substr(0, 5) + " ms";
		draw_list->AddText(ImVec2(graphRightEdge + 5, p.y + spacing.y), ImColor(255, 255, 255), yMax.c_str());
		ImGui::Text("Application average: %.3f ms/frame: (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::SameLine();
		ImGui::Text("Last frame: %.3f ms", ImGui::GetIO().DeltaTime * 1000);

		// Create legend
		for (uint32_t i = 0; i < entries.size(); i++)
		{
			draw_list->AddCircleFilled(ImVec2(startx, p.y + spacing.y + starty), radius, colors[i]);
			draw_list->AddText(ImVec2(startx + radius + padding, p.y + spacing.y + starty - (radius / 2.f) - 2.f), ImColor(255, 255, 255), entries[i].name.c_str());
			starty += rowHeight;
		}

		ImGui::End();
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

