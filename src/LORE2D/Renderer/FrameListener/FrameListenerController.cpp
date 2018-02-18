// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// The MIT License (MIT)
// This source file is part of LORE2D
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

#include "FrameListenerController.h"

#include <LORE2D/Core/Util.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void FrameListenerController::frameStarted()
{
  FrameListener::FrameEvent e;

  // Update frame listeners.
  for ( const auto& frameListener : _frameListeners ) {
    frameListener->frameStarted( e );
  }

  // Update bound functors.
  for ( const auto& callback : _frameStartedCallbacks ) {
    callback( e );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void FrameListenerController::frameEnded()
{
  FrameListener::FrameEvent e;

  // Update frame listeners.
  for ( const auto& frameListener : _frameListeners ) {
    frameListener->frameEnded( e );
  }

  // Update bound functors.
  for ( const auto& callback : _frameEndedCallbacks ) {
    callback( e );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void FrameListenerController::registerFrameListener( FrameListener* listener )
{
  _frameListeners.push_back( listener );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void FrameListenerController::registerFrameStartedCallback( FrameStartedCallback callback )
{
  _frameStartedCallbacks.push_back( callback );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void FrameListenerController::registerFrameEndedCallback( FrameEndedCallback callback )
{
  _frameEndedCallbacks.push_back( callback );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void FrameListenerController::unregisterFrameListener( FrameListener* listener )
{
  if ( _frameListeners.empty() ) {
    return;
  }

  for ( auto it = _frameListeners.begin(); it != _frameListeners.end(); ) {
    FrameListener* currentListener = *it;
    if ( currentListener == listener ) {
      _frameListeners.erase( it );
      break;
    }
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void FrameListenerController::unregisterFrameStartedCallback( FrameStartedCallback callback )
{
  if ( _frameStartedCallbacks.empty() ) {
    return;
  }

  for ( auto it = _frameStartedCallbacks.begin(); it != _frameStartedCallbacks.end(); ) {
    FrameStartedCallback currentCallback = *it;
    if ( Util::GetFPAddress(currentCallback) == Util::GetFPAddress(callback) ) {
      _frameStartedCallbacks.erase( it );
      break;
    }
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void FrameListenerController::unregisterFrameEndedCallback( FrameEndedCallback callback )
{
  if ( _frameEndedCallbacks.empty() ) {
    return;
  }

  for ( auto it = _frameEndedCallbacks.begin(); it != _frameEndedCallbacks.end(); ) {
    FrameEndedCallback currentCallback = *it;
    if ( Util::GetFPAddress( currentCallback ) == Util::GetFPAddress( callback ) ) {
      _frameEndedCallbacks.erase( it );
      break;
    }
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
