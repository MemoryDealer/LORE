#pragma once
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

#include <LORE2D/Core/Util.h>
#include <LORE2D/Resource/Material.h>
#include <LORE2D/Resource/Mesh.h>
#include <LORE2D/Resource/Registry.h>
#include <LORE2D/Shader/GPUProgram.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

    struct UberProgramParameters
    {
        unsigned int maxLights { 4 };
        unsigned int numTextures { 1 };
        bool texYCoordinateFlipped { true };
        bool colorMod { true }; // Modulate final output by color, regardless of scene lighting.
    };

    struct BackgroundProgramParameters
    {
      bool scrolling { true };
      bool fitViewport { false };
    };

    class LORE_EXPORT StockResourceController
    {

    public:

        enum class GPUProgramType
        {
            StandardTexturedQuad,
            StandardTexturedTriangle
        };

        enum class MaterialType
        {
            BaseWhiteNoLighting
        };

    public:

        StockResourceController();

        virtual ~StockResourceController();

        virtual void createStockResources();

        virtual GPUProgramPtr createUberProgram( const string& name, const UberProgramParameters& params ) = 0;

        virtual GPUProgramPtr createBackgroundProgram( const string& name, const BackgroundProgramParameters& params ) = 0;

        //
        // Retrieval functions for each type of stock resource.

        GPUProgramPtr getGPUProgram( const string& name );

        MaterialPtr getMaterial( const string& name );

        MeshPtr getMesh( const MeshType& type );

        TexturePtr getTexture( const string& name );

        VertexBufferPtr getVertexBuffer( const string& name );

        //
        // Cloning functions.

        MaterialPtr cloneMaterial( const string& name, const string& cloneName );

    protected:

        using MeshTable = Util::HashTable<MeshType, MeshPtr>;

    protected:

        std::unique_ptr<ResourceController> _controller;

        MeshTable _meshTable;

    };

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    class LORE_EXPORT StockResource final
    {

    public:

        static GPUProgramPtr GetGPUProgram( const string& name );

        static MaterialPtr GetMaterial( const string& name );

        static MeshPtr GetMesh( const MeshType& type );

        static TexturePtr GetTexture( const string& name );

        static VertexBufferPtr GetVertexBuffer( const string& name );

        //
        // Cloning.

        static MaterialPtr CloneMaterial( const string& name, const string& newName );

    private:

        friend class Context;

    private:

        static void AssignContext( ContextPtr context );

    };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
