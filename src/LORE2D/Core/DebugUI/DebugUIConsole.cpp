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

#include <LORE2D/Core/Context.h>
#include <LORE2D/Core/CLI/CLI.h>
#include <LORE2D/Core/DebugUI/DebugUI.h>
#include <LORE2D/Resource/Box.h>
#include <LORE2D/Resource/Entity.h>
#include <LORE2D/Resource/ResourceController.h>
#include <LORE2D/Resource/StockResource.h>
#include <LORE2D/Resource/Textbox.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace LocalNS {

  static DebugUIConsole* ConsoleInstance = nullptr;
  static constexpr const auto CursorDefaultX = -.96f;

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  static void OnKeyChanged( const Keycode key, const bool pressed )
  {
    static bool initialized = false;

    if ( pressed ) {
      switch ( key ) {
      default:
        break;

      case Keycode::Backspace:
        ConsoleInstance->backspace();
        break;

      case Keycode::Delete:
        ConsoleInstance->onDelete();
        break;

      case Keycode::Enter:
        ConsoleInstance->execute();
        break;

      case Keycode::Left:
        ConsoleInstance->cursorLeft();
        break;

      case Keycode::Right:
        ConsoleInstance->cursorRight();
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
        {
          auto str = CLI::GetNextCommand();
          if ( !str.empty() ) {
            ConsoleInstance->setCommandStr( str );
          }
        }
        break;

      case Keycode::Home:
        ConsoleInstance->cursorHome();
        break;

      case Keycode::End:
        ConsoleInstance->cursorEnd();
        break;
      }
    }
    else {
      switch ( key ) {
      default:
        break;

      case Keycode::GraveAccent:
      case Keycode::Escape:
        if ( initialized ) {
          ConsoleInstance->clear();
          CLI::ResetCommandHistoryIndex();
          DebugUI::HideConsole();
          initialized = false;
        }
        else {
          // Detect the initial key up for GraveAccent so we don't immediately exit.
          initialized = true;
        }
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
  _panel = _ui->createPanel( "core" );
  _consoleElement = _panel->createElement( "Console" );
  _consoleBoxElement = _panel->createElement( "ConsoleBox" );
  _consoleHistoryElement = _panel->createElement( "ConsoleHistory" );

  // Create textbox for console.
  _consoleTextbox = Resource::CreateTextbox( "DebugUI_Console" );
  _consoleElement->attachTextbox( _consoleTextbox );
  _consoleHistoryTextbox = Resource::CreateTextbox( "DebugUI_ConsoleHistory" );
  _consoleHistoryElement->attachTextbox( _consoleHistoryTextbox );
  _consoleBox = Resource::CreateBox( "DebugUI_Console" );
  _consoleBoxElement->attachBox( _consoleBox );

  // Create background for console.
  _backgroundElement = _panel->createElement( "background" );
  _backgroundEntity = Resource::CreateEntity( "DebugUI_ConsoleBackground", VertexBuffer::Type::Quad );
  _backgroundElement->attachEntity( _backgroundEntity );

  // Create blinking cursor.
  _cursorElement = _panel->createElement( "DebugUI_Cursor" );
  _cursorEntity = Resource::CreateEntity( "DebugUI_Cursor", VertexBuffer::Type::Quad );
  _cursorEntity->setMaterial( StockResource::GetMaterial( "UnlitStandard" ) ); // TODO: Add parameter or overload to CreateEntity to avoid cloning default material.
  _cursorElement->attachEntity( _cursorEntity );
  _cursorElement->setDimensions( .16f, .4f );
  _cursorElement->setPosition( CursorDefaultX, -.91f );
  _cursorElement->setDepth( -1.f );

  // Setup positional data.
  _consoleElement->setPosition( -.98f, -.94f );
  _consoleBoxElement->setPosition( 0.f, -.91f );
  _consoleBoxElement->setDimensions( 10.f, .8f );
  _consoleHistoryElement->setPosition( -.98f, -.80f );
  _backgroundElement->setPosition( 0.f, -1.2f );
  _backgroundElement->setDimensions( 15.f, 5.f );
  _backgroundElement->setDepth( 1.f ); // Behind console text.

  // Setup colors.
  auto backgroundMat = _backgroundEntity->getMaterial();
  backgroundMat->blendingMode.enabled = true;
  backgroundMat->diffuse.a = 0.5f;
  backgroundMat->ambient = Color( 0.f, 0.f, 0.f, 0.0f );
  _consoleBox->setFillColor( Color( 0.f, 0.f, 0.f, 0.6f ) );
  _consoleBox->setBorderColor( Color( .5f, .5f, .5f, .9f ) );

  // Setup input hooks.
  _hooks.keyCallback = OnKeyChanged;
  _hooks.charCallback = OnChar;

  ConsoleInstance = this;

  // TODO: Move these static functions out of Context.
  Context::RegisterFrameListener( this );
  _time = Clock::now();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

DebugUIConsole::~DebugUIConsole()
{
  //Context::UnregisterFrameListener( this ); // too late, context null.
  // destroy ui
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void DebugUIConsole::setCommandStr( const string& cmd )
{
  _command = cmd;
  _consoleTextbox->setText( _command );
  _cursorElement->setPosition( CursorDefaultX, -.91f );
  _cursorIdx = 0;
  for ( const char c : _command ) {
    const auto width = _consoleTextbox->getFont()->getWidth( c );
    _cursorElement->translate( width / Context::GetActiveWindow()->getAspectRatio(), 0.f );
    ++_cursorIdx;
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void DebugUIConsole::appendChar( const char c )
{
  string s;
  s += c;
  _command.insert( _cursorIdx, s );
  _consoleTextbox->setText( _command );

  // Move blinking cursor.
  const auto width = _consoleTextbox->getFont()->getWidth( c );
  // TODO: Dividing by aspect ratio is major hack to account for adjustment in GenericRenderer.
  //   This indicates a wider problem with UI rendering.
  _cursorElement->translate( width / Context::GetActiveWindow()->getAspectRatio(), 0.f );
  ++_cursorIdx;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void DebugUIConsole::backspace()
{
  if ( !_command.empty() && _cursorIdx > 0 ) {
    --_cursorIdx;
    const char c = _command[_cursorIdx];
    _command.erase( _cursorIdx, 1 );
    _consoleTextbox->setText( _command );

    const auto width = _consoleTextbox->getFont()->getWidth( c );
    _cursorElement->translate( -width / Context::GetActiveWindow()->getAspectRatio(), 0.f );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void DebugUIConsole::onDelete()
{
  if ( !_command.empty() && _cursorIdx < _command.size() ) {
    _command.erase( _cursorIdx, 1 );
    _consoleTextbox->setText( _command );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void DebugUIConsole::cursorLeft()
{
  if ( _cursorIdx > 0 ) {
    --_cursorIdx;
    const char c = _command[_cursorIdx];
    const auto width = _consoleTextbox->getFont()->getWidth( c );
    _cursorElement->translate( -width / Context::GetActiveWindow()->getAspectRatio(), 0.f );
  }
  _cursorElement->setVisible( true );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void DebugUIConsole::cursorRight()
{
  if ( _command.size() > _cursorIdx ) {
    const char c = _command[_cursorIdx];
    ++_cursorIdx;
    const auto width = _consoleTextbox->getFont()->getWidth( c );
    _cursorElement->translate( width / Context::GetActiveWindow()->getAspectRatio(), 0.f );
  }
  _cursorElement->setVisible( true );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void DebugUIConsole::cursorHome()
{
  while ( _cursorIdx > 0 ) {
    cursorLeft();
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void DebugUIConsole::cursorEnd()
{
  while ( _cursorIdx < _command.size() ) {
    cursorRight();
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void DebugUIConsole::execute()
{
  if ( !_command.empty() ) {
    auto output = CLI::Execute( _command );
    clear();
    _consoleHistoryTextbox->setText( output );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void DebugUIConsole::clear()
{
  _command.clear();
  _consoleTextbox->setText( "" );
  _consoleHistoryTextbox->setText( "" );
  _cursorElement->setPosition( CursorDefaultX, _cursorElement->getPosition().y );
  _cursorIdx = 0;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void DebugUIConsole::frameStarted( const FrameEvent& e )
{
  static bool cursorVisible = false;

  auto now = Clock::now();
  if ( std::chrono::duration_cast<std::chrono::milliseconds>( now - _time ).count() > 500 ) {
    cursorVisible = !cursorVisible;
    _cursorElement->setVisible( cursorVisible );
    _time = now;
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
