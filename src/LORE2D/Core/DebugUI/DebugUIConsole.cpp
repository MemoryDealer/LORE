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

#include "DebugUIConsole.h"

#include <LORE2D/Core/CLI/CLI.h>
#include <LORE2D/Core/DebugUI/DebugUI.h>
#include <LORE2D/Resource/Entity.h>
#include <LORE2D/Resource/Renderable/Textbox.h>
#include <LORE2D/Resource/ResourceController.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace LocalNS {

  static DebugUIConsole* ConsoleInstance = nullptr;

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  static void OnKeyChanged( const Keycode key, const bool pressed )
  {
    if ( pressed ) {
      switch ( key ) {
      default:
        break;

      case Keycode::Backspace:
        ConsoleInstance->popBack();
        break;

      case Keycode::Enter:
        ConsoleInstance->execute();
        break;

      case Keycode::Up:
        {
          auto str = CLI::GetPreviousCommand();
          if ( !str.empty() ) {
            ConsoleInstance->setCommandStr( str );
          }
        }
        break;

      case Keycode::Down:
        ConsoleInstance->setCommandStr( CLI::GetNextCommand() );
        break;

      case Keycode::GraveAccent:
      case Keycode::Escape:
        ConsoleInstance->clear();
        DebugUI::HideConsole();
        break;
      }
    }
  }

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  static void OnChar( const char c )
  {
    if ( c != '`' ) {
      ConsoleInstance->appendChar( c );
    }
  }

}
using namespace LocalNS;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

DebugUIConsole::DebugUIConsole()
{
  // Create the console UI.
  _ui = Resource::CreateUI( "DebugUI_Console" ); // TODO: Different resource group for debug UI.
  _panel = _ui->createPanel( "default" );
  _consoleElement = _panel->createElement( "Console" );
  _consoleHistoryElement = _panel->createElement( "ConsoleHistory" );

  // Create textbox for console.
  _consoleTextbox = Resource::CreateTextbox( "DebugUI_Console" );
  _consoleElement->setTextbox( _consoleTextbox );
  _consoleHistoryTextbox = Resource::CreateTextbox( "DebugUI_ConsoleHistory" );
  _consoleHistoryElement->setTextbox( _consoleHistoryTextbox );
  

  // Create background for console.
  _backgroundElement = _panel->createElement( "background" );
  _backgroundEntity = Resource::CreateEntity( "DebugUI_ConsoleBackground", MeshType::Quad );
  auto backgroundMat = _backgroundEntity->getMaterial();
  backgroundMat->blendingMode.enabled = true;
  backgroundMat->diffuse = Color( 0.f, 0.f, 0.f, 0.5f );
  _backgroundElement->setEntity( _backgroundEntity );

  // Setup positional data.
  _consoleElement->setPosition( -.98f, -.94f );
  _consoleHistoryElement->setPosition( -.98f, -.82f );
  _backgroundElement->setPosition( 0.f, -0.5f );
  _backgroundElement->setDimensions( 15.f, 5.f );
  _backgroundElement->setDepth( 1.f ); // Behind console text.

  // Setup input hooks.
  _hooks.keyCallback = OnKeyChanged;
  _hooks.charCallback = OnChar;

  ConsoleInstance = this;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

DebugUIConsole::~DebugUIConsole()
{
  // destroy ui
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void DebugUIConsole::setCommandStr( const string& cmd )
{
  _command = cmd;
  _consoleTextbox->setText( _command );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void DebugUIConsole::appendChar( const char c )
{
  _command += c;
  _consoleTextbox->setText( _command );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void DebugUIConsole::popBack()
{
  if ( !_command.empty() ) {
    _command.pop_back();
    _consoleTextbox->setText( _command );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void DebugUIConsole::execute()
{
  auto output = CLI::Execute( _command );
  clear();
  _consoleHistoryTextbox->setText( output );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void DebugUIConsole::clear()
{
  _command.clear();
  _consoleTextbox->setText( "" );
  _consoleHistoryTextbox->setText( "" );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
