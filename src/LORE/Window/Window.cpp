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

#include "Window.h"

#include <LORE/Resource/ResourceController.h>
#include <LORE/Resource/StockResource.h>
#include <LORE/UI/Debug/DebugUI.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Window::Window()
{
#ifdef LORE_DEBUG_UI
  // Allocate built-in UIs.
  _debugUI = std::make_shared<DebugUI>();
#endif

  Input::AddKeyListener( this );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Window::~Window()
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Window::addRenderView( const RenderView& renderView )
{
  // Verify this render view does not already exist.
  for ( const auto& rv : _renderViews ) {
    if ( rv == renderView ) {
      throw Lore::Exception( "RenderView " + renderView.name +
                             " already exists in Window " + _title );
    }
  }

  LogWrite( Info, "Adding RenderView %s to Window %s", renderView.name.c_str(), _title.c_str() );
  _renderViews.push_back( renderView );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Window::removeRenderView( const RenderView& renderView )
{
  removeRenderView( renderView.name );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Window::removeRenderView( const string& name )
{
  for ( auto it = _renderViews.begin(); it != _renderViews.end(); ) {
    const RenderView& rv = ( *it );
    if ( rv.name == name ) {
      LogWrite( Info, "Removing RenderView %s from Window %s", name.c_str(), _title.c_str() );
      it = _renderViews.erase( it );
      break;
    }
    else {
      ++it;
    }
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

const RenderView& Window::getRenderView( const string& name ) const
{
  for ( const auto& rv : _renderViews ) {
    if ( name == rv.name ) {
      return rv;
    }
  }

  throw Lore::Exception( "RenderView " + name + " does not exist in Window " + _title );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Window::resetRenderViews()
{
  _renderViews.clear();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Window::setTitle( const string& title )
{
  _title = title;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Window::setDimensions( const int width, const int height )
{
  _dimensions.width = width;
  _dimensions.height = height;

#ifdef LORE_DEBUG_UI
  _debugUI->setWindowDimensions( _dimensions );
#endif
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Window::setMode( const Mode& mode )
{
  _mode = mode;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

const RenderView& Window::getRenderView( const int32_t idx ) const
{
  if ( _renderViews.size() < idx ) {
    throw Lore::Exception( "Render view with index " + std::to_string( idx ) + " does not exist in window " + _title );
  }
  return _renderViews[idx];
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

ResourceControllerPtr Window::getResourceController() const
{
  return _controller.get();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

StockResourceControllerPtr Window::getStockResourceController() const
{
  return _stockController.get();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Window::onKeyDown( const Keycode code )
{
#ifdef LORE_DEBUG_UI
  switch ( code ) {
  default:
    break;

  case Keycode::GraveAccent:
    {
      auto debugUI = std::static_pointer_cast<DebugUI>( _debugUI );

      if ( Input::GetKeyState( Keycode::LeftShift ) || Input::GetKeyState( Keycode::RightShift ) ) {
        if ( debugUI->getEnabled() && DebugUI::Panel::PerformanceStats == debugUI->getActivePanel() ) {
          // Do nothing--keep debug UI open.
        }
        else {
          debugUI->setEnabled( true );
        }

        debugUI->setActivePanel( DebugUI::Panel::Console );
        Input::OverrideHooks( debugUI->getInputHooks() );
        Input::SetCursorEnabled( true );
      }
      else {
        debugUI->setActivePanel( DebugUI::Panel::PerformanceStats );
        debugUI->setEnabled( !debugUI->getEnabled() );
      }
    }
    break;
  }
#endif
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
