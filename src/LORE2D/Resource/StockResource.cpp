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

#include "StockResource.h"

#include <LORE2D/Core/Context.h>
#include <LORE2D/Resource/Material.h>
#include <LORE2D/Resource/ResourceController.h>
#include <LORE2D/Resource/StockResource.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace LocalNS {

    static ContextPtr ActiveContext = nullptr;

}
using namespace LocalNS;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

StockResourceController::StockResourceController()
: _controller( nullptr )
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

StockResourceController::~StockResourceController()
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void StockResourceController::createStockResources()
{
    //
    // Load stock programs.

    // Lit programs.
    {
        UberShaderParameters params;

        createUberShader( "StandardTexturedQuad", params );

        params.numTextures = 0;
        params.vbType = VertexBuffer::Type::Quad;
        createUberShader( "StandardQuad", params );
    }
    
    // Unlit programs.
    {
        UberShaderParameters params;
        params.maxLights = 0;

        createUberShader( "UnlitTexturedQuad", params );

        params.numTextures = 0;
        params.vbType = VertexBuffer::Type::Quad;
        createUberShader( "UnlitStandardQuad", params );
    }

    //
    // Load stock materials.

    // Lit materials.
    {
        auto material = _controller->createMaterial( "StandardTexturedQuad" );
        Material::Pass& pass = material->getPass();
        pass.lighting = true;
        pass.program = _controller->getGPUProgram( "StandardTexturedQuad" );
    }

    {
        auto material = _controller->createMaterial( "StandardQuad" );
        Material::Pass& pass = material->getPass();
        pass.lighting = true;
        pass.program = _controller->getGPUProgram( "StandardQuad" );
    }

    // Unlit materials.
    {
        auto material = _controller->createMaterial( "UnlitTexturedQuad" );
        Material::Pass& pass = material->getPass();
        pass.lighting = false;
        pass.program = _controller->getGPUProgram( "UnlitTexturedQuad" );
    }

    {
        auto material = _controller->createMaterial( "UnlitStandardQuad" );
        Material::Pass& pass = material->getPass();
        pass.lighting = false;
        pass.program = _controller->getGPUProgram( "UnlitStandardQuad" );
    }

    //
    // Create stock textures.

    // Lit textures.
    {
        auto texture = _controller->createTexture( "White" );
        texture->setMaterial( getMaterial( "StandardQuad" ) );
    }

    // Unlit textures.
    {
        auto texture = _controller->createTexture( "UnlitWhite" );
        texture->setMaterial( getMaterial( "UnlitStandardQuad" ) );
    }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GPUProgramPtr StockResourceController::getGPUProgram( const string& name )
{
    return _controller->getGPUProgram( name );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

MaterialPtr StockResourceController::getMaterial( const string& name )
{
    return _controller->getMaterial( name );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

TexturePtr StockResourceController::getTexture( const string& name )
{
    return _controller->getTexture( name );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GPUProgramPtr StockResource::GetGPUProgram( const string& name )
{
    return ActiveContext->getStockResourceController()->getGPUProgram( name );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

MaterialPtr StockResource::GetMaterial( const string& name )
{
    return ActiveContext->getStockResourceController()->getMaterial( name );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

TexturePtr StockResource::GetTexture( const string& name )
{
    return ActiveContext->getStockResourceController()->getTexture( name );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void StockResource::AssignContext( ContextPtr context )
{
    ActiveContext = context;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

