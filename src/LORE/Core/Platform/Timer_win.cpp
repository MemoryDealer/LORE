// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// The MIT License (MIT)
// This source file is part of LORE
// ( Lightweight Object-oriented Rendering Engine )
//
// Copyright (c) 2016-2017 Jordan Sparks
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files ( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

#include <LORE/Core/Timer.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

#if LORE_PLATFORM == LORE_WINDOWS

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Timer::Timer()
  : mSecondsPerCount( 0.0 )
  , mDeltaTime( -1.0 )
  , mBaseTime( 0 )
  , mPausedTime( 0 )
  , mPrevTime( 0 )
  , mCurrTime( 0 )
  , mStopped( false )
{
  // Get initial seconds per count.
  __int64 countsPerSec;
  QueryPerformanceFrequency(
    reinterpret_cast<LARGE_INTEGER*>( &countsPerSec ) );
  mSecondsPerCount = 1.0 / static_cast<double>( countsPerSec );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

float Timer::getDeltaTime( void ) const
{
  return static_cast<float>( mDeltaTime );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

float Timer::getTotalElapsedTime( void ) const
{
  if ( mStopped ) {
    return static_cast<float>( ( (
      mStopTime - mPausedTime ) - mBaseTime ) * mSecondsPerCount );
  }
  else {
    return static_cast<float>( ( (
      mCurrTime - mPausedTime ) - mBaseTime ) * mSecondsPerCount );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Timer::reset( void )
{
  __int64 currTime;
  QueryPerformanceCounter( reinterpret_cast<LARGE_INTEGER*>( &currTime ) );

  mBaseTime = currTime;
  mPrevTime = currTime;
  mStopTime = 0;
  mStopped = false;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Timer::start( void )
{
  if ( mStopped ) {
    __int64 startTime;
    QueryPerformanceCounter( reinterpret_cast<LARGE_INTEGER*>( &startTime ) );

    mPausedTime += ( startTime - mStopTime );
    mPrevTime = startTime;
    mStopTime = 0;
    mStopped = false;
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Timer::stop( void )
{
  if ( !mStopped ) {
    __int64 currTime;
    QueryPerformanceCounter( reinterpret_cast<LARGE_INTEGER*>( &currTime ) );

    mStopTime = currTime;
    mStopped = true;
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Timer::tick( void )
{
  if ( mStopped ) {
    mDeltaTime = 0.0;
    return;
  }

  __int64 currTime;
  QueryPerformanceCounter( reinterpret_cast<LARGE_INTEGER*>( &currTime ) );
  mCurrTime = currTime;

  mDeltaTime = ( mCurrTime - mPrevTime ) * mSecondsPerCount;

  mPrevTime = mCurrTime;

  // Negative time possible if CPU goes into power save mode.
  if ( mDeltaTime < 0.0 ) {
    mDeltaTime = 0.0;
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

#endif // LORE_WINDOWS

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
