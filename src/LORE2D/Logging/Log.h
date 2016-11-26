#pragma once
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

#include "Types.h"

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

#pragma warning( disable: 4251 )

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

    enum class LogLevel {
        Critical,       // Program must terminate.
        Error,          // Something failed, which may or may not be fatal.
        Warning,        // A possible concern.
        Information,    // Similar to warning, however this reports general information.
        Debug,          // Extended information.
        Trace           // For tracing entry/exit of routines or blocks.
    };

    class LORE_EXPORT Logger final
    {

    private:

        LogLevel _level;

        struct Message {
            LogLevel lvl;
            string text;
        };

        std::queue<Message> _messageQueue;
        std::mutex _mutex;
        std::condition_variable _cv;
        std::atomic<bool> _active;

    private:

        void __logger();

    public:

        explicit Logger( const string& filename );

        ~Logger();

        void write( const string& text );

        void write( const LogLevel& lvl, const string& text );

        //
        // Setters.

        void setLevel( const LogLevel& lvl )
        {
            _level = lvl;
        }

    };

    class LORE_EXPORT Log final
    {

    public:

        static void Write( const string& text );

        static void Write( const LogLevel& lvl, const string& text );

        static void AllocateLogger();

    };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
