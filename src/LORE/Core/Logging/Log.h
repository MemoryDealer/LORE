#pragma once
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

#include <LORE/Types.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

#pragma warning( disable: 4251 )

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {



  ///
  /// \class Logger
  /// \brief Writes log messages to Lore.log file in same directory as executable.
  /// \details All logging should be done through the static methods of the helper
  ///     class Log.
  class Logger final
  {

  public:

    enum class Level
    {
      Critical,       // Program must terminate.
      Error,          // Something failed, which may or may not be fatal.
      Warning,        // A possible concern.
      Info,           // Similar to warning, however this reports general information.
      Trace           // For tracing entry/exit of routines or blocks.
    };

    struct Message
    {
      Level lvl;
      string file;
      string line;
      string function;
      string text;
    };

  public:

    explicit Logger( const string& filename );

    ~Logger();

    //
    // Logging functions.

    void log( const Message& msg );

    ///
    /// \brief Wakes up the logger thread and joins.
    void flush();

    bool levelEnabled( const Level lvl );

    //
    // Setters.

    void setLevel( const Level lvl );

  private:

    void __logger();

  private:

    Level _level;

    std::ofstream _stream;
    std::queue<Message> _messageQueue;

    //
    // Provide thread materials, as the actual output to the log file is done
    // on a separate thread, which must be woken up when log messages are available.

    std::thread _thread;
    std::mutex _mutex;
    std::condition_variable _cv;
    std::atomic<bool> _active;

  };

  class LORE_EXPORT Log final
  {

  public:

    static void WriteLog( const bool internal,
                          const Logger::Level lvl,
                          const char* file,
                          const int line,
                          const char* function,
                          const char* format,
                          ... );

    static bool LevelEnabled( const Logger::Level lvl );

    static void Alloc();
    static void Delete();

  };

  //
  // Helper macros.

#define LogWrite(lvl, format, ...)\
  do {\
    if ( Log::LevelEnabled( Logger::Level::lvl ) ) {\
      Log::WriteLog( true, Logger::Level::lvl, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__);\
    }\
  } while ( false )

#define TO_LLU(value) static_cast<unsigned long long>( value )

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
