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

#include "Timestamp.h"

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Local {

    std::unique_ptr<ITimestamper> __timestamper;

}
using namespace Local;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Lore::CreateTimestamper()
{
    if ( __timestamper.get() ) {
        __timestamper.reset();
    }

    __timestamper = std::make_unique<Timestamper>();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

string Lore::GenerateTimestamp()
{
    return __timestamper->generate();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

#if LORE_PLATFORM == LORE_WINDOWS

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

string Timestamper::generate() const 
{
    SYSTEMTIME st { 0 };

    GetLocalTime( &st );
    string timestamp;

    // Trash "algorithm" to generate a timestamp string.
    auto month = std::to_string( st.wMonth );
    if ( st.wMonth < 10 ) month.insert( 0, "0" );
    auto day = std::to_string( st.wDay );
    if ( st.wDay < 10 ) day.insert( 0, "0" );
    auto year = std::to_string( st.wYear );
    auto hour = std::to_string( st.wHour );
    if ( st.wHour < 10 ) hour.insert( 0, "0" );
    auto minute = std::to_string( st.wMinute );
    if ( st.wMinute < 10 ) minute.insert( 0, "0" );
    auto second = std::to_string( st.wSecond );
    if ( st.wSecond < 10 ) second.insert( 0, "0" );

    timestamp.append( month + "/" + day + "/" + year + " " );
    timestamp.append( hour + ":" + minute + ":" + second );
    return timestamp;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

#elif LORE_PLATFORM == LORE_APPLE

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

string Timestamper::generate() const
{
    string timestamp("??:??:??");
    return timestamp;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

#endif

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //


