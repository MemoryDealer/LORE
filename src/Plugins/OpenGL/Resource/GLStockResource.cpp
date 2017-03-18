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

#include "GLStockResource.h"

#include <LORE2D/Core/APIVersion.h>

#include <Plugins/OpenGL/Resource/GLResourceController.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore::OpenGL;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

StockResourceController::StockResourceController()
: Lore::StockResourceController()
{
    _controller = std::make_unique<ResourceController>();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

StockResourceController::~StockResourceController()
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void StockResourceController::createStockResources()
{
    Lore::StockResourceController::createStockResources();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::GPUProgramPtr StockResourceController::createUberShader( const string& name, const Lore::UberShaderParameters& params )
{
    const bool textured = ( params.numTextures > 0 );
    const string header = "#version " +
        std::to_string( APIVersion::GetMajor() ) + std::to_string( APIVersion::GetMinor() ) + "0" +
        " core\n";

    //
    // Vertex shader.

    string src = header;

    //
    // Layout.

    src += "layout (location = 0) in vec2 vertex;";

    if ( textured ) {
        src += "layout (location = 1) in vec2 texCoord;";
    }

    //
    // Uniforms and outs.

    src += "uniform mat4 transform;";

    if ( textured ) {
        src += "out vec2 TexCoord;";
    }

    //
    // main function.

    src += "void main(){";

    src += "gl_Position = transform * vec4(vertex, 0.0f, 1.0f);";
    if ( textured ) {
        if ( params.texYCoordinateFlipped ) {
            src += "TexCoord = vec2(texCoord.x, 1.0 - texCoord.y);";
        }
        else {
            src += "TexCoord = texCoord;";
        }
    }

    src += "}";

    // Compile vertex shader.
    //auto vs = std::make_unique<Shader>( name + "_VS", Shader::Type::Vertex );
    auto vsptr = _controller->createVertexShader( name + "_VS" );
    if ( !vsptr->loadFromSource( src ) ) {
        throw Lore::Exception( "Failed to compile uber vertex shader for " + name );
    }

    // ::::::::::::::::::::::::::::::::: //

    //
    // Fragment shader.

    src.clear();
    src = header;

    //
    // Uniforms and ins.

    if ( textured ) {
        src += "uniform sampler2D tex;";
    }

    // ...

    if ( textured ) {
        src += "in vec2 TexCoord;";
    }

    // Final pixel output color.
    src += "out vec4 pixel;";

    //
    // main function.

    src += "void main(){";

    if ( textured ) {
        src += "pixel = texture(tex, TexCoord);";
    }

    src += "}";
    auto fsptr = _controller->createFragmentShader( name + "_FS" );
    if ( !fsptr->loadFromSource( src ) ) {
        throw Lore::Exception( "Failed to compile uber fragment shader for " + name );
    }

    // ::::::::::::::::::::::::::::::::: //

    // Attach a vertex buffer.
    auto vbptr = _controller->createVertexBuffer( name + "_VB", params.vbType );
    vbptr->build();

    //
    // GPU program.

    auto program = _controller->createGPUProgram( name );
    program->setVertexBuffer( vbptr );
    program->attachShader( vsptr );
    program->attachShader( fsptr );

    if ( !program->link() ) {
        throw Lore::Exception( "Failed to link GPUProgram " + name );
    }

    program->addTransformVar( "transform" );

    return program;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
