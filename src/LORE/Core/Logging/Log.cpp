// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// The MIT License (MIT)
// This source file is part of LORE
// ( Lightweight Object-oriented Rendering Engine )
//
// Copyright (c) 2017-2021 Jordan Sparks
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

#include "Log.h"

#include <iostream>

#include <LORE/Core/Logging/Timestamp.h>
#include <LORE/Util/FileUtils.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace LocalNS {

  std::unique_ptr<Logger> __logger;

}
using namespace LocalNS;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Logger::__logger()
{
  const std::unordered_map<Level, string> logLevelStrings = {
    { Level::Critical, "Critical" },
    { Level::Error, "Error" },
    { Level::Warning, "Warning" },
    { Level::Info, "Info" },
    { Level::Trace, "Trace" },
  };

  while ( _active ) {

    // Wait for incoming messages.
    std::unique_lock<std::mutex> lock( _mutex );
    _cv.wait( lock );

    auto timestamp = GenerateTimestamp();

    while ( !_messageQueue.empty() ) {
      Message msg=_messageQueue.front();

      // Build the log string.
      string out;
      out.append( "[" + timestamp + "] " );
      out.append( msg.file + ":" + msg.line + " (" + msg.function + ")" );
      out.append( "[" + logLevelStrings.at( msg.lvl ) + "] " );
      out.append( msg.text );

      // Output the string to both the console and log file.

      ///
      /// Ever since upgrading to VS2019, something is severely wrong with printf and it hangs indefinitely at times.
      /// They claim to have fixed it but they might be lying.
      /// https://developercommunity.visualstudio.com/t/printf-vprintf-blocknever-return/1314072
      /// 
      //printf( "%s\n", out.c_str() );
      std::cout << out << std::endl;
      _stream << out << std::endl;

      _messageQueue.pop();
    }

    _stream.flush();

  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Logger::Logger( const string& filename )
  : _level( Level::Info )
  , _stream()
  , _messageQueue()
  , _thread()
  , _mutex()
  , _cv()
  , _active( true )
{
  // Open log file.
  _stream.open( filename.c_str(), std::ofstream::out );
  if ( !_stream.is_open() ) {
    throw Lore::Exception( "Unable to create log file " + filename );
  }

  _stream << "..::| LORE Log Started |::.." << std::endl;

  _thread = std::thread( &Logger::__logger, this );

  CreateTimestamper();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Logger::~Logger()
{
  flush();
  if ( _stream.is_open() ) {
    _stream.close();
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Logger::log( const Message& msg )
{
  // Enqueue message and wake up logger thread.
  std::unique_lock<std::mutex> lock( _mutex );
  _messageQueue.push( msg );
  _cv.notify_one();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Logger::flush()
{
  _active = false;
  if ( _thread.joinable() ) {
    _cv.notify_one();
    _thread.join();
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

bool Logger::levelEnabled( const Level lvl )
{
  return ( lvl <= _level );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Logger::setLevel( const Level lvl )
{
  _level = lvl;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Log::WriteLog( const bool internal,
                    const Logger::Level lvl,
                    const char* file,
                    const int line,
                    const char* function,
                    const char* format,
                    ... )
{
  if ( !format ) {
    return;
  }

  Logger::Message msg;
  msg.lvl = lvl;

  // Parse the message parameters.
  if ( 0 == strncmp( "%s", format, sizeof( format ) ) ) {
    va_list args;
    va_start( args, format );
    msg.text = va_arg( args, const char* );
    va_end( args );
  }
  else {
    constexpr auto BufferSize = 512;
    char buf[BufferSize];
    va_list args;
    va_start( args, format );
    const auto sz = vsnprintf( buf, BufferSize, format, args );
    va_end( args );

    if ( sz >= 0 && sz < BufferSize ) {
      msg.text = buf;
    }
    else {
      // Not a valid buffer.
      return;
    }
  }

  msg.file = FileUtil::GetFileName( file, true );
  msg.line = std::to_string( line );
  msg.function = function;

  __logger->log( msg );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

bool Log::LevelEnabled( const Logger::Level lvl )
{
  return __logger->levelEnabled( lvl );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Log::Alloc()
{
  __logger = std::make_unique<Logger>( "LORE.log" );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Log::Delete()
{
  __logger->flush();
  __logger.reset();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
