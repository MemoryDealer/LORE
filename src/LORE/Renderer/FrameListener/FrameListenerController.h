#pragma once
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

#include <LORE/Renderer/FrameListener/FrameListener.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  ///
  /// \class FrameListenerController
  /// \brief Handles per-frame callbacks for FrameListeners and registered individual callback
  ///   functors.
  class LORE_EXPORT FrameListenerController final
  {

  public:

    using FrameStartedCallback = std::function<void( const FrameListener::FrameEvent& e )>;
    using FrameEndedCallback = std::function<void( const FrameListener::FrameEvent& e )>;

  private:

    using FrameListenerList = std::vector<FrameListener*>;
    using FrameStartedCallbackList = std::vector<FrameStartedCallback>;
    using FrameEndedCallbackList = std::vector<FrameEndedCallback>;

  private:

    FrameListenerList _frameListeners {};
    FrameStartedCallbackList _frameStartedCallbacks {};
    FrameEndedCallbackList _frameEndedCallbacks {};

  public:

    FrameListenerController() = default;

    ~FrameListenerController() = default;

    void frameStarted();

    void frameEnded();

    void registerFrameListener( FrameListener* listener );

    void registerFrameStartedCallback( FrameStartedCallback callback );

    void registerFrameEndedCallback( FrameEndedCallback callback );

    void unregisterFrameListener( FrameListener* listener );

    void unregisterFrameStartedCallback( FrameStartedCallback callback );

    void unregisterFrameEndedCallback( FrameEndedCallback callback );

  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
