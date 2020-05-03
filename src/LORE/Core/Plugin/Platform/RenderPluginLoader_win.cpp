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

#include <LORE/Core/Plugin/RenderPluginLoader.h>

#include <LORE/Core/Context.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

#if LORE_PLATFORM == LORE_WINDOWS

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

RenderPluginLoader::RenderPluginLoader()
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
        LogWrite( Critical, "Unable to load render plugin %s - error %d", file.c_str(), GetLastError() );
        return false;
    }

    LogWrite( Info, "Render plugin %s successfully loaded", file.c_str() );

    return true;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

std::unique_ptr<Context> RenderPluginLoader::createContext()
{
    // Function pointer to create the context.
    using CreateContextPtr = Context*( *)( );

    // Get address of CreateContext function inside DLL.
    CreateContextPtr ccp = reinterpret_cast< CreateContextPtr >(
        GetProcAddress( _hModule, "CreateContext" ) );
    if ( nullptr == ccp ) {
        LogWrite( Critical, "Unable to get CreateContext function pointer from render plugin" );
        return nullptr;
    }

    // Call the DLL's CreateContext() - allocate the render plugin's context.
    Context* context = ccp();

    // Place Context object into unique_ptr to meet Lore's standard.
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
