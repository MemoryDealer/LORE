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

#include "RenderPluginLoader.h"

#include <LORE2D/Core/Context.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

#if defined( _WIN32 ) || defined( _WIN64 )

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

constexpr
RenderPluginLoader::RenderPluginLoader()
: _hModule( nullptr )
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

RenderPluginLoader::~RenderPluginLoader()
{
    free();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

bool RenderPluginLoader::load( const string& file )
{
    free(); 

    _hModule = LoadLibrary( file.c_str() );
    if ( nullptr == _hModule ) {
        log_critical( "Unable to load render plugin " + file );
        return false;
    }

    log_debug( "Render plugin " + file + " successfully loaded" );

    return true;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

std::unique_ptr<Context> RenderPluginLoader::createContext()
{
    using CreateContextPtr = Context*( *)( );

    CreateContextPtr ccp = reinterpret_cast< CreateContextPtr >(
        GetProcAddress( _hModule, "CreateContext" ) );
    if ( nullptr == ccp ) {
        log_critical( "Unable to get CreateContext function pointer from render plugin" );
        return nullptr;
    }

    // Allocate the render plugin's context.
    Context* context = ccp();

    std::unique_ptr<Context> p( context );
    return std::move( p );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void RenderPluginLoader::free()
{
    if ( _hModule ) {
        FreeLibrary( _hModule );
    }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

#endif

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
