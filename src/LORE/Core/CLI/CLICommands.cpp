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

#include "CLI.h"

#include <LORE/Core/Context.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace LocalNS {

  using Lore::CLI;
  using Lore::string;

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
  // Scene layout and aesthetic commands.
  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  struct SetNodePos : public CLI::Command
  {

    string execute( string& args ) override
    {
      if ( 2 == CLI::GetNumArgs( args ) ) {
        auto nodeName = CLI::ExtractNextArg( args );
        auto posStr = CLI::ExtractNextArg( args );
        auto pos = CLI::ToVec2( posStr );

        try {
          auto node = CLI::GetActiveScene()->getNode( nodeName );
          node->setPosition( pos );
        }
        catch ( const Lore::Exception& e ) {
          return e.getDescription();
        }

        return string("Set " + nodeName + " to position " + posStr);
      }
      return Command::execute( args );
    }

  };

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  struct TranslateNode : public CLI::Command
  {

    string execute( string& args ) override
    {
      if ( 2 == CLI::GetNumArgs( args ) ) {
        auto nodeName = CLI::ExtractNextArg( args );
        auto offsetStr = CLI::ExtractNextArg( args );
        auto offset = CLI::ToVec3( offsetStr );

        try {
          auto node = CLI::GetActiveScene()->getNode( nodeName );
          node->translate( offset );
        }
        catch ( const Lore::Exception& e ) {
          return e.getDescription();
        }

        return string( "Translated " + nodeName + " " + offsetStr );
      }
      return Command::execute( args );
    }

  };

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  struct SetLightColor : public CLI::Command
  {

    string execute( string& args ) override
    {
      if ( 2 == CLI::GetNumArgs( args ) ) {
        auto lightName = CLI::ExtractNextArg( args );
        auto lightColorStr = CLI::ExtractNextArg( args );
        auto lightColor = CLI::ToVec4( lightColorStr );

        try {
          auto light = CLI::GetActiveScene()->getLight( Lore::Light::Type::Point, lightName );
          light->setDiffuse( lightColor );
          light->setSpecular( lightColor );
        }
        catch ( const Lore::Exception& e ) {
          return e.getDescription();
        }

        return string( "Light " + lightName + "'s color set to " + lightColorStr );
      }
      return Command::execute( args );
    }

  };

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
  // Global commands.
  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  struct ReloadScene : public CLI::Command
  {

    string execute( string& args ) override
    {
      if ( CLI::GetActiveScene()->reload() ) {
        return string( "Scene reloaded." );
      }
      else {
        return string( "Scene reload failed." );
      }
    }

  };

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

}
using namespace LocalNS;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void CLI::Init()
{
  // Scene layout/aesthetic commands.
  CLI::RegisterCommand( new SetNodePos(), 2, "SetNodePos", "SetNodePosition");
  CLI::RegisterCommand( new TranslateNode(), 2, "TranslateNode", "MoveNode" );
  CLI::RegisterCommand( new SetLightColor(), 2, "SetLightColor", "slc" );

  // Global commands.
  CLI::RegisterCommand( new ReloadScene(), 2, "ReloadScene", "Reload" );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
