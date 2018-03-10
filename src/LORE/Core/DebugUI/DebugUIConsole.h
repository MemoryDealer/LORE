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

#include "DebugUIComponent.h"

#include <LORE/Core/Timer.h>
#include <LORE/Renderer/FrameListener/FrameListener.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  using Clock = std::chrono::high_resolution_clock;

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  class DebugUIConsole final : public DebugUIComponent,
                               public FrameListener
  {

  public:

    DebugUIConsole();
    virtual ~DebugUIConsole() override;

    void setCommandStr( const string& cmd );
    void appendChar( const char c );
    void backspace();
    void onDelete();
    void cursorLeft();
    void cursorRight();
    void cursorHome();
    void cursorEnd();
    void execute();
    void clear();

    virtual void frameStarted( const FrameEvent& e ) override;

  private:

    UIPanelPtr _panel { nullptr };
    UIElementPtr _consoleElement { nullptr };
    TextboxPtr _consoleTextbox { nullptr };
    BoxPtr _consoleBox { nullptr };
    UIElementPtr _consoleBoxElement { nullptr };
    UIElementPtr _consoleHistoryElement { nullptr };
    TextboxPtr _consoleHistoryTextbox { nullptr };
    UIElementPtr _backgroundElement { nullptr };
    EntityPtr _backgroundEntity { nullptr };

    UIElementPtr _cursorElement { nullptr };
    EntityPtr _cursorEntity { nullptr };

    Clock::time_point _time;

    string _command {};
    uint32_t _cursorIdx { 0 };

  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //