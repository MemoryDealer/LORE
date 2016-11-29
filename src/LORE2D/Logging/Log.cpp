// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// The MIT License (MIT)
// This source file is part of LORE2D
// ( Lightweight Object-oriented Rendering Engine )
//
// Copyright (c) 2016 Jordan Sparks
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

#include <LORE2D/Core/Timestamp.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Local {

    std::unique_ptr<Logger> __log;

}
using namespace Local;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Logger::__logger()
{
    std::unordered_map<LogLevel, string> logLevelStrings;
    logLevelStrings[LogLevel::Critical] = " Critical";
    logLevelStrings[LogLevel::Error] = " Error";
    logLevelStrings[LogLevel::Warning] = " Warning";
    logLevelStrings[LogLevel::Information] = "";
    logLevelStrings[LogLevel::Debug] = " Debug";
    logLevelStrings[LogLevel::Trace] = " Trace";

    while ( _active ) {

        // Wait for incoming messages.
        std::unique_lock<std::mutex> lock( _mutex );
        _cv.wait( lock );

        auto timestamp = GenerateTimestamp();

        while ( !_messageQueue.empty() ) {
            Message msg = _messageQueue.front();

            // Build the log string.
            string out = "[" + timestamp + "]";
            out.append( logLevelStrings[msg.lvl] + ": " );
            out.append( msg.text );

            // Output the string to both the console and log file.
            printf( "%s\n", out.c_str() );
            _stream << out << std::endl;

            _messageQueue.pop();
        }

        _stream.flush();

    }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Logger::Logger( const string& filename )
: _level( LogLevel::Information )
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

    _stream << "..::| LORE2D Log Started |::.." << std::endl;

    _thread = std::thread( &Logger::__logger, this );
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

void Logger::write( const string& text )
{
    write( LogLevel::Information, text );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Logger::write( const LogLevel& lvl, const string& text )
{
    Message msg;
    msg.lvl = lvl;
    msg.text = text;

    // Enqueue message and wake up logger thread.
    std::unique_lock<std::mutex> lock( _mutex );
    _messageQueue.push( msg );
    _cv.notify_one();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Logger::flush()
{
    if ( _thread.joinable() ) {
        _cv.notify_one();
        _thread.join();
    }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Log::Write( const string& msg )
{
    if ( LogLevel::Information <= __log->getLevel() ) {
        __log->write( msg );
    }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Log::Write( const LogLevel& lvl, const string& msg )
{
    if ( lvl <= __log->getLevel() ) {
        __log->write( lvl, msg );
    }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Log::AllocateLogger()
{
    if ( !__log.get() ) {
        __log = std::make_unique<Logger>( "Lore.log" );
    }
    CreateTimestamper();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Log::DeleteLogger()
{
    if ( __log.get() ) {
        __log->setActive( false );
        __log->flush();
        __log.reset();
    }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
