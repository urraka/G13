#include <System/System.h>

#if defined(WIN32)
	#include <windows.h>
#elif defined(APPLE)
	#include <mach/mach_time.h>
#elif defined(UNIX)
	#include <time.h>
#endif

#if defined(WIN32)
namespace
{
	LARGE_INTEGER getFrequency()
	{
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);
		return frequency;
	}
}
#endif

Time Clock::time()
{
	#if defined(WIN32)
		HANDLE currentThread = GetCurrentThread();
		DWORD_PTR previousMask = SetThreadAffinityMask(currentThread, 1);

		static LARGE_INTEGER frequency = getFrequency();

		LARGE_INTEGER time;
		QueryPerformanceCounter(&time);

		SetThreadAffinityMask(currentThread, previousMask);

		return 1000000 * time.QuadPart / frequency.QuadPart;
	#elif defined(APPLE)
		static mach_timebase_info_data_t frequency = {0, 0};

		if (frequency.denom == 0)
			mach_timebase_info(&frequency);

		Time nanoseconds = mach_absolute_time() * frequency.numer / frequency.denom;

		return nanoseconds / 1000;
	#elif defined(UNIX)
		timespec time;
		clock_gettime(CLOCK_MONOTONIC, &time);
		return static_cast<Time>(time.tv_sec) * 1000000 + time.tv_nsec / 1000;
	#endif
}
