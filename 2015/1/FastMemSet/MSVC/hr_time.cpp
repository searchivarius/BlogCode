/* 
 * This solution is borrowed from
 * http://cplus.about.com/od/howtodothingsi2/a/timing.htm
 */

#include <windows.h>

#include "hr_time.h"

double CStopWatch::LIToSecs( LARGE_INTEGER & L) {
	return ((double)L.QuadPart /(double)frequency.QuadPart);
}

CStopWatch::CStopWatch(){
	timer.start.QuadPart=0;
	timer.stop.QuadPart=0;	
	QueryPerformanceFrequency( &frequency );
}

void CStopWatch::startTimer( ) {
    QueryPerformanceCounter(&timer.start);
}

void CStopWatch::stopTimer( ) {
    QueryPerformanceCounter(&timer.stop);
}


double CStopWatch::getElapsedTime() {
	LARGE_INTEGER time;
	time.QuadPart = timer.stop.QuadPart - timer.start.QuadPart;
    return LIToSecs( time) ;
}
