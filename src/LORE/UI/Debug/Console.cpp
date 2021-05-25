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

#ifdef LORE_DEBUG_UI

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

#include "Console.h"

#include <External/imgui/imgui.h>
#include <LORE/Core/CLI/CLI.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void DebugUI_Console::render()
{
  static bool resetKeyboardFocus = false;

  const real ConsoleHeight = 10.f;
  const real ConsoleYOffset = 32.f;
  ImGui::SetNextWindowSize( ImVec2( static_cast<float>( _windowDimensions.width ), ConsoleHeight ) );
  ImGui::SetNextWindowPos( ImVec2( 0.f, _windowDimensions.height - ConsoleYOffset ) );

  ImGui::Begin( "Console", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav );
  if ( resetKeyboardFocus ) {
    ImGui::SetKeyboardFocusHere();
    resetKeyboardFocus = false;
  }

  char buf[256] {};
  bool executed = ImGui::InputText( "Console", buf, IM_ARRAYSIZE( buf ), ImGuiInputTextFlags_EnterReturnsTrue );

  ImGui::End();

  // Display the command output.

  const real OutputHeight = 60.f;
  const real OutputYOffset = 92.f;
  ImGui::SetNextWindowSize( ImVec2( static_cast<float>( _windowDimensions.width ), OutputHeight ) );
  ImGui::SetNextWindowPos( ImVec2( 0.f, _windowDimensions.height - OutputYOffset ) );
  ImGui::Begin( "Console.Output", nullptr, ImGuiWindowFlags_NoDecoration );

  // Update CLI output if a command was executed.
  if ( executed ) {
    string input( buf );
    if ( !input.empty() ) {
      _cliOutput = CLI::Execute( buf );
      resetKeyboardFocus = true; // Keep focus in textbox after hitting enter.
    }
  }

  ImGui::TextWrapped( _cliOutput.c_str() );
  ImGui::End();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

#endif

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
