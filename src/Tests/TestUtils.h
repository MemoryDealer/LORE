#pragma once
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

#include <LORE/Lore.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

// Platform define:

#define LORE_WINDOWS 0
#define LORE_LINUX 1
#define LORE_APPLE 2

#if defined( WIN32 ) || defined( _WIN32 )
#define LORE_PLATFORM LORE_WINDOWS
#elif defined( LINUX ) || defined( _LINUX )
#define LORE_PLATFORM LORE_LINUX
#elif defined( APPLE ) || defined( _APPLE )
#define LORE_PLATFORM LORE_APPLE
#endif

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

//
// Object creation/setup.

// Creates a Lore context with the OpenGL renderer.
#define TEST_CREATE_CONTEXT()\
std::unique_ptr<Lore::Context> context;\
context = Lore::CreateContext( Lore::RenderPlugin::OpenGL );\
REQUIRE( context.get() != nullptr );

#define TEST_DESTROY_CONTEXT() DestroyLoreContext( context )


// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
