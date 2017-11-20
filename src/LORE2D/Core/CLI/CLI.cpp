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

#include "CLI.h"

#include <LORE2D/Core/Util.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace LocalNS {

  static std::unordered_map<Lore::string, std::unique_ptr<Lore::CLI::Command>> CommandMap;

}
using namespace LocalNS;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

string CLI::Execute( const string& command )
{
  // Extract command name from string.
  size_t end = command.find_first_of( ' ' );
  string commandName;
  string args;
  if ( std::string::npos == end ) {
    end = command.size(); // This command has no arguments.
    commandName = command;
  }
  else {
    commandName = command.substr( 0, end );
    args = command.substr( end + 1, command.size() - end );
  }

  Util::ToLower( commandName );
  auto lookup = CommandMap.find( commandName );
  if ( CommandMap.end() != lookup ) {
    std::unique_ptr<Command>& p = lookup->second;
    return p->execute( args );
  }

  return string( "Unknown command." );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

bool CLI::RegisterCommand( const string& commandName, CommandPtr command )
{
  std::unique_ptr<Command> p( command );
  Util::ToLower( const_cast<string&>( commandName ) );
  auto pair = CommandMap.emplace( commandName, std::move( p ) );
  return pair.second;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

string CLI::GetNextArg( string& str )
{
  // TODO: Remove multiple whitespace .
  string arg;
  const size_t nextArgPos = str.find_first_of( ' ' );
  if ( string::npos != nextArgPos ) {
    arg = str.substr( 0, nextArgPos );

    // Remove argument from original string.
    str = str.substr( nextArgPos + 1 );
  }
  else {
    arg = str.substr( 0, str.size() - nextArgPos );

    // Last argument, so clear string.
    str.clear();
  }

  return arg;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

uint32_t CLI::GetNumArgs( const string& str )
{
  std::istringstream tokenizer( str );
  string arg;
  uint32_t count = 0;
  while ( std::getline( tokenizer, arg, ' ' ) ) {
    ++count;
  }

  return count;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
