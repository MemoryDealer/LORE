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

#include <LORE/Util/Util.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace LocalNS {

  static Lore::ContextPtr ActiveContext = nullptr;
  static std::unordered_multimap<Lore::string, Lore::CLI::CommandPtr> CommandMap {};
  static std::vector<Lore::string> CommandHistory {};
  static int32_t CommandHistoryIdx= -1;

}
using namespace LocalNS;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

ScenePtr CLI::ActiveScene = nullptr;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

ContextPtr CLI::GetContext()
{
  return ActiveContext;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

string CLI::Execute( const string& command )
{
  // Extract command name from string.
  size_t end = command.find_first_of( ' ' );
  string commandName;
  string args;
  if ( string::npos == end ) {
    end = command.size(); // This command has no arguments.
    commandName = command;
  }
  else {
    commandName = command.substr( 0, end );
    args = command.substr( end + 1, command.size() - end );
  }

  // Add command to history. Don't add if the previous command is identical.
  bool dup = false;
  if ( !CommandHistory.empty() ) {
    if ( CommandHistory[0].compare( command ) == 0 ) {
      dup = true;
    }
  }
  if ( !dup ) {
    CommandHistory.insert( CommandHistory.begin(), command );
  }
  CommandHistoryIdx = -1;

  StringUtil::ToLower( commandName );
  auto lookup = CommandMap.find( commandName );
  if ( CommandMap.end() != lookup ) {
    CommandPtr p = lookup->second;

    // Shrink blocks of multiple spaces to a single space.
    auto newEnd = std::unique( args.begin(), args.end(),
                               [] ( const char lhs, const char rhs ) { return ( lhs == rhs ) && ( lhs == ' ' ); } );
    args.erase( newEnd, args.end() );

    return p->execute( args );
  }

  return string( "Unknown command." );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

bool CLI::RegisterCommand( CommandPtr command, const uint32_t count, ... )
{
  va_list v;
  va_start( v, count );

  for ( uint32_t n = 0; n < count; ++n ) {
    auto c = va_arg( v, const char* );
    string commandName( c );
    StringUtil::ToLower( commandName );
    std::pair<string, CommandPtr> pair( commandName, command );
    CommandMap.insert( pair );
  }

  va_end( v );
  return true;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

string CLI::GetPreviousCommand()
{
  if ( CommandHistory.empty() ) {
    return string();
  }

  if ( CommandHistoryIdx + 1 != CommandHistory.size() ) {
    ++CommandHistoryIdx;
  }

  return CommandHistory[CommandHistoryIdx];
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

string CLI::GetNextCommand()
{
  if ( CommandHistory.empty() ) {
    return string();
  }

  if ( 0 < CommandHistoryIdx ) {
    --CommandHistoryIdx;
  }
  else if ( -1 == CommandHistoryIdx ) {
    return string();
  }

  return CommandHistory[CommandHistoryIdx];
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void CLI::ResetCommandHistoryIndex()
{
  CommandHistoryIdx = -1;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

string CLI::ExtractNextArg( string& str )
{
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

  // Remove whitespace.
  arg.erase( std::remove_if( arg.begin(), arg.end(),
             [] ( unsigned char c ) { return std::isspace( c ); } ),
             arg.end() );

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

glm::vec2 CLI::ToVec2( const string& str )
{
  std::istringstream tokenizer( str );
  string x, y;
  std::getline( tokenizer, x, ',' );
  std::getline( tokenizer, y );

  if ( x.empty() ) {
    x = "0";
  }
  if ( y.empty() ) {
    y = "0";
  }

  return glm::vec2( std::stof( x ), std::stof( y ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

glm::vec3 CLI::ToVec3( const string& str )
{
  std::istringstream tokenizer( str );
  string x, y, z;
  std::getline( tokenizer, x, ',' );
  std::getline( tokenizer, y, ',' );
  std::getline( tokenizer, z );

  if ( x.empty() ) {
    x = "0";
  }
  if ( y.empty() ) {
    y = "0";
  }
  if ( z.empty() ) {
    z = "0";
  }

  return glm::vec3( std::stof( x ), std::stof( y ), std::stof( z ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

glm::vec4 CLI::ToVec4( const string& str )
{
  std::istringstream tokenizer( str );
  string x, y, z, w;
  std::getline( tokenizer, x, ',' );
  std::getline( tokenizer, y, ',' );
  std::getline( tokenizer, z, ',' );
  std::getline( tokenizer, w );

  if ( x.empty() ) {
    x = "0";
  }
  if ( y.empty() ) {
    y = "0";
  }
  if ( z.empty() ) {
    z = "0";
  }
  if ( w.empty() ) {
    w = "0";
  }

  return glm::vec4( std::stof( x ), std::stof( y ), std::stof( z ), std::stof( w ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void CLI::AssignContext( ContextPtr context )
{
  ActiveContext = context;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
