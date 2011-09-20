/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#include "cinder/Timer.h"

#if defined( CINDER_MSW )
	#include <windows.h>
#endif

namespace cinder {

Timer::Timer()
	: mIsStopped( true )
{
#if defined( CINDER_COCOA )
	mEndTime = mStartTime = -1;
#elif defined( CINDER_MSW )
	::LARGE_INTEGER nativeFreq;
	::QueryPerformanceFrequency( &nativeFreq );
	mInvNativeFreq = 1.0 / nativeFreq.QuadPart;
	mStartTime = mEndTime = -1;
#elif defined( CINDER_LINUX )
	memset(&mStartTime, 0, sizeof(struct timespec));
	memset(&mEndTime, 0, sizeof(struct timespec));
#endif
}

Timer::Timer( bool startOnConstruction )
	: mIsStopped( true )
{
#if defined( CINDER_COCOA )
	mEndTime = mStartTime = -1;
#elif defined( CINDER_MSW )
	::LARGE_INTEGER nativeFreq;
	::QueryPerformanceFrequency( &nativeFreq );
	mInvNativeFreq = 1.0 / nativeFreq.QuadPart;
	mStartTime = mEndTime = -1;
#elif defined( CINDER_LINUX )
	memset(&mStartTime, 0, sizeof(struct timespec));
	memset(&mEndTime, 0, sizeof(struct timespec));
#endif
	if( startOnConstruction ) {
		start();
	}
}

void Timer::start()
{	
#if defined( CINDER_COCOA )
	mStartTime = ::CFAbsoluteTimeGetCurrent();
#elif defined( CINDER_MSW )
	::LARGE_INTEGER rawTime;
	::QueryPerformanceCounter( &rawTime );
	mStartTime = rawTime.QuadPart * mInvNativeFreq;
#elif defined( CINDER_LINUX )
	clock_gettime(CLOCK_MONOTONIC, &mStartTime);
#endif

	mIsStopped = false;
}

double Timer::getSeconds() const
{
#if defined( CINDER_LINUX )
	if (mIsStopped) {
		return ((mEndTime.tv_sec + mEndTime.tv_nsec / double(1e9)) - 
				(mStartTime.tv_sec + mStartTime.tv_nsec / double(1e9)));
	}
	else {
		struct timespec mCurrentTime;
		clock_gettime(CLOCK_MONOTONIC, &mCurrentTime);
		return ((mCurrentTime.tv_sec + mCurrentTime.tv_nsec / double(1e9)) - 
				(mStartTime.tv_sec + mStartTime.tv_nsec / double(1e9)));
	}
#else
	if( mIsStopped )
		return mEndTime - mStartTime;
	else {
#if defined( CINDER_COCOA )
		return ::CFAbsoluteTimeGetCurrent() - mStartTime;
#elif defined( CINDER_MSW )
	::LARGE_INTEGER rawTime;
	::QueryPerformanceCounter( &rawTime );
	return (rawTime.QuadPart * mInvNativeFreq) - mStartTime;
#endif
	}
#endif
}

void Timer::stop()
{
	if( ! mIsStopped ) {
#if defined( CINDER_COCOA )
		mEndTime = ::CFAbsoluteTimeGetCurrent();
#elif defined( CINDER_MSW )
		::LARGE_INTEGER rawTime;
		::QueryPerformanceCounter( &rawTime );
		mEndTime = rawTime.QuadPart * mInvNativeFreq;
#elif defined( CINDER_LINUX )
		clock_gettime(CLOCK_MONOTONIC, &mEndTime);
#endif
		mIsStopped = true;
	}
}

} // namespace cinder
