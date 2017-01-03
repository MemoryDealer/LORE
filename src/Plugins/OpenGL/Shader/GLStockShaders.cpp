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

#include "GLStockShaders.h"

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore::OpenGL;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

StockVertexShaders::StockVertexShaders()
: _shaders()
{

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

string& StockVertexShaders::get( const Type& t )
{
    auto lookup = _shaders.find( t );
    if ( _shaders.end() != lookup ) {
        return lookup->second;
    }

    throw Lore::Exception( "Stock shader not found" );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void StockVertexShaders::create( const Parameters& p )
{
    string s( "#version 430 core\n" );

    uint location = 0;
    if ( p.pos2D ) {
        s += "layout (location = " + std::to_string( location++ ) + ") in vec2 in_Pos;";
    }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
//
//const string StockVertexShaders::Generic =
//"#version 330 core\n"
//""
//"layout (location = 0) in vec2 in_Pos;"
//"layout (location = 1) in vec2 in_TexCoord;"
//""
//"uniform mat4 mvp;"
//"uniform mat4 model;"
//""
//"out vec2 FPos;"
//"out vec2 FTexCoord;"
//""
//"void main()"
//"{"
//"gl_Position = mvp * vec4(in_Pos, 1.0f);"
//"FPos = (model * vec4(in_Pos, 1.0f)).xy;"
//"FTexCoord = vec2(in_TexCoord.x, 1.0 - in_TexCoord.y);"
//"}"
//;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
