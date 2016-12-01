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

#include "catch.hpp"
#include <LORE2D/Lore.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

TEST_CASE( "Context correctly created from render plugins", "[context]" )
{
    std::unique_ptr<Lore::Context> context;

    SECTION( "OpenGL render plugin" )
    {
        context = Lore::CreateContext( Lore::RenderPlugin::OpenGL );

        REQUIRE( context.get() != nullptr );
        REQUIRE( context->getRenderPluginName() == "OpenGL" );

        DestroyLoreContext( context );
    }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

TEST_CASE( "Context creation/destruction multiple times in a single run", "[context]" )
{
    std::unique_ptr<Lore::Context> context;

    SECTION( "Twice" )
    {
        context = Lore::CreateContext( Lore::RenderPlugin::OpenGL );
        REQUIRE( context.get() != nullptr );

        DestroyLoreContext( context );
        REQUIRE( context.get() == nullptr );

        context = Lore::CreateContext( Lore::RenderPlugin::OpenGL );
        REQUIRE( context.get() != nullptr );

        DestroyLoreContext( context );
        REQUIRE( context.get() == nullptr );
    }

    SECTION( "Context destruction/re-creation with window creation" )
    {
        context = Lore::CreateContext( Lore::RenderPlugin::OpenGL );

        Lore::WindowPtr window = context->createWindow( "UnitTest", 50, 50 );
        REQUIRE( nullptr != window );

        DestroyLoreContext( context );
        // The window object it points to should still be valid (though the render implementation resources should now be free).
        REQUIRE( nullptr != window );

        context = Lore::CreateContext( Lore::RenderPlugin::OpenGL );
        REQUIRE( nullptr != context );

        window = context->createWindow( "UnitTest2", 50, 50 );
        REQUIRE( nullptr != window );

        DestroyLoreContext( context );
        REQUIRE( nullptr != window );
    }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
