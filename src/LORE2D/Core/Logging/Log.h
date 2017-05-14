#pragma once
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

#include "Types.h"

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

#pragma warning( disable: 4251 )

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  enum class LogLevel
  {
    Critical,       // Program must terminate.
    Error,          // Something failed, which may or may not be fatal.
    Warning,        // A possible concern.
    Information,    // Similar to warning, however this reports general information.
    Debug,          // Extended information.
    Trace           // For tracing entry/exit of routines or blocks.
  };

  ///
  /// \class Logger
  /// \brief Writes log messages to Lore.log file in same directory as executable.
  /// \details All logging should be done through the static methods of the helper
  ///     class Log.
  class Logger final
  {

  private:

    LogLevel _level;

    struct Message
    {
      LogLevel lvl;
      string text;
    };

    std::ofstream _stream;
    std::queue<Message> _messageQueue;

    //
    // Provide thread materials, as the actual output to the log file is done
    // on a separate thread, which must be woken up when log messages are available.

    std::thread _thread;
    std::mutex _mutex;
    std::condition_variable _cv;
    std::atomic<bool> _active;

  private:

    void __logger();

  public:

    explicit Logger( const string& filename );

    ~Logger();

    //
    // Logging functions.

    ///
    /// \brief Writes log message with timestamp at information level.
    void write( const string& text );

    ///
    /// \brief Writes log message with timestamp at specified level.
    void write( const LogLevel& lvl, const string& text );

    ///
    /// \brief Wakes up the logger thread and joins.
    void flush();

    //
    // Setters.

    inline void setLevel( const LogLevel& lvl )
    {
      _level=lvl;
    }

    inline void setActive( const bool active )
    {
      _active=active;
    }

    //
    // Getters.

    inline LogLevel getLevel() const
    {
      return _level;
    }

  };

  ///
  /// \class Log
  /// \brief Contains only static methods for logging
  class LORE_EXPORT Log final
  {

  public:

    ///
    /// \brief Writes message with timestamp to log file at information level.
    static void Write( const string& text );

    ///
    /// \brief Writes message with timestamp to log file at specified level.
    static void Write( const LogLevel& lvl, const string& text );

  private:

    // Provide Context access so only it can create a logger.
    friend class Context;

  private:

    static void AllocateLogger();

    static void DeleteLogger();

  };

  // Helper macros.
#define log_critical( text )    Log::Write( LogLevel::Critical, text )
#define log_error( text )       Log::Write( LogLevel::Error, text )
#define log_warning( text )     Log::Write( LogLevel::Warning, text )
#define log_information( text ) Log::Write( LogLevel::Information, text )
#define lore_log( text )        Log::Write( LogLevel::Information, text )
#define log_debug( text )       Log::Write( LogLevel::Debug, text )
#define log_trace( text )       Log::Write( LogLevel::Trace, text )
#define log_trace_entry         Log::Write( LogLevel::Trace, ( "Entry >>> " + __FILE__ + ": " + __FUNCTION__ + " - Line " + __LINE__ ) );
#define log_trace_exit          Log::Write( LogLevel::Trace, ( "Exit  >>> " + __FILE__ + ": " + __FUNCTION__ + " - Line " + __LINE__ ) );

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
