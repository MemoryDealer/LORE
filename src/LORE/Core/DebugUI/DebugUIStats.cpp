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

#include "DebugUIStats.h"

#include <LORE/Core/Context.h>
#include <LORE/Resource/Box.h>
#include <LORE/Resource/ResourceController.h>
#include <LORE/Resource/Textbox.h>
#include <LORE/Resource/StockResource.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

DebugUIStats::DebugUIStats()
{
  // Create the stats UI.
  _ui = Resource::CreateUI( "DebugUI_Stats" );
  _panel = _ui->createPanel( "core" );
  _frameDataElement = _panel->createElement( "FrameData" );
  _frameDataBoxElement = _panel->createElement( "FrameDataBox" );

  // Create textbox for frame data.
  _frameDataTextbox = Resource::CreateTextbox( "DebugUI_Stats.FrameData" );
  _frameDataElement->attachTextbox( _frameDataTextbox );

  // Create box to go around frame data.
  _frameDataBox = Resource::CreateBox( "DebugUI_Stats.FrameDataBox" );
  _frameDataBoxElement->attachBox( _frameDataBox );

  // Setup positions.
  _frameDataBoxElement->setPosition( -0.55f, .96f );
  _frameDataBoxElement->setDimensions( 4.5f, .45f );
  _frameDataElement->setPosition( -0.99f, 0.94f );

  // Setup appearance.
  _frameDataTextbox->setFont( StockResource::GetFont( "DebugUI" ) );
  _frameDataTextbox->setText( "Calculating..." );
  _frameDataTextbox->setTextColor( Color( 0.1f, 0.9f, 0.1f, 1.f ) );
  _frameDataBox->setFillColor( Color( 0.f, 0.f, 0.6f, 0.6f ) );
  _frameDataBox->setBorderColor( Color( 0.f, 0.f, 0.8f, 0.9f ) );

  // Start the timer.
  _timer.reset();

  Context::RegisterFrameListener( this );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

DebugUIStats::~DebugUIStats()
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void DebugUIStats::frameStarted( const FrameEvent& e )
{
  _timer.tick();

  ++_frameCount;

  // Get average over one second period.
  if ( ( _timer.getTotalElapsedTime() - _elapsed ) >= 1.f ) {
    const real fps = static_cast< real >( _frameCount );
    const real mspf = 1000.f / fps;

    std::ostringstream oss;
    oss.precision( 3 );
    oss << "FPS: " << fps << "    " << "Frame Time: " << mspf << " (ms)";

    // Update UI.
    _frameDataTextbox->setText( oss.str() );

    // Configure data for next run.
    _frameCount = 0;
    _elapsed += 1.f;
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
