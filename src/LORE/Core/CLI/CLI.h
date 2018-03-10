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

#include <LORE/Math/Math.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  class LORE_EXPORT CLI
  {

  public:

    struct Command
    {

      virtual string execute( string& args )
      {
        return string( "Invalid arguments: " + args );
      }
      virtual void undo() { }

    };
    using CommandPtr = Command*;

  public:

    static void Init();

    static ContextPtr GetContext();

    static string Execute( const string& command );

    static bool RegisterCommand( CommandPtr command, const uint32_t count, ... );

    static string GetPreviousCommand();

    static string GetNextCommand();

    static void ResetCommandHistoryIndex();

    static ScenePtr GetActiveScene() { return ActiveScene; }

    static void SetActiveScene( ScenePtr scene ) { ActiveScene = scene; }

    // String processing.

    // Returns next isolated string, and removes it from str.
    static string ExtractNextArg( string& str );

    static uint32_t GetNumArgs( const string& str );

    // Conversion.

    static Vec2 ToVec2( const string& str );

    static Vec4 ToVec4( const string& str );

  private:

    friend class Context;
    static void AssignContext( ContextPtr context );

  private:

    static ScenePtr ActiveScene;

  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //