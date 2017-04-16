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
#include <LORE2D/Memory/Alloc.h>
#include <LORE2D/Resource/Material.h>
#include <LORE2D/Resource/Mesh.h>
#include <LORE2D/Resource/Registry.h>
#include <LORE2D/Resource/Renderable/Texture.h>
#include <LORE2D/Resource/ResourceIndexer.h>
#include <LORE2D/Scene/Camera.h>
#include <LORE2D/Shader/GPUProgram.h>
#include <LORE2D/Shader/Shader.h>
#include <LORE2D/Shader/VertexBuffer.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

    struct LORE_EXPORT ResourceGroup
    {

        enum class ResourceType
        {
            Texture,
            Shader,
            GPUProgram
        };

        struct IndexedResource
        {
            ResourceType type;
            string file;
            bool loaded;
        };

        // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

        using ShaderRegistry = Registry<std::unordered_map, Shader>;
        using ResourceIndex = std::map<string, IndexedResource>;

        // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

        inline explicit ResourceGroup( const string& name_ )
        : name( name_ )
        , index()
        , textures()
        , programs()
        , vertexShaders()
        , fragmentShaders()
        , vertexBuffers()
        , materials()
        , entities()
        , cameras()
        { }

        // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

        string name;

        ResourceIndex index;

        Registry<std::unordered_map, Texture> textures;

        // Shaders.

        Registry<std::unordered_map, GPUProgram> programs;

        ShaderRegistry vertexShaders;
        ShaderRegistry fragmentShaders;

        Registry<std::unordered_map, VertexBuffer> vertexBuffers;

        // Materials.

        Registry<std::unordered_map, Material> materials;

        // Meshes.

        Registry<std::unordered_map, Mesh> meshes;

        // Entities.

        Registry<std::unordered_map, Entity> entities;

        // Scene.

        Registry<std::unordered_map, Camera> cameras;

    };

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    class LORE_EXPORT ResourceController
    {

    public:

        static const string DefaultGroupName;

    public:

        ResourceController();

        virtual ~ResourceController();

        //
        // Groups.

        void createGroup( const string& name );

        void destroyGroup( const string& name );

        void addResourceLocation( const string& directory, const bool recursive = false, const string& groupName = DefaultGroupName );

        void loadGroup( const string& name );

        void unloadGroup( const string& name );

        //
        // Loading.

        virtual TexturePtr loadTexture( const string& name, const string& file, const string& groupName = DefaultGroupName ) = 0;

        //
        // Factory functions.

        virtual GPUProgramPtr createGPUProgram( const string& name, const string& groupName = DefaultGroupName ) = 0;

        virtual ShaderPtr createVertexShader( const string& name, const string& groupName = DefaultGroupName ) = 0;

        virtual ShaderPtr createFragmentShader( const string& name, const string& groupName = DefaultGroupName ) = 0;

        virtual VertexBufferPtr createVertexBuffer( const string& name, const MeshType& type, const string& groupName = DefaultGroupName ) = 0;

        virtual MaterialPtr createMaterial( const string& name, const string& groupName = DefaultGroupName ) = 0;

        virtual TexturePtr createTexture( const string& name, const string& groupName = DefaultGroupName ) = 0;

        virtual CameraPtr createCamera( const string& name, const string& groupName = DefaultGroupName ) = 0;

        //
        // Entity.

        EntityPtr createEntity( const string& name, const MeshType& meshType, const string& groupName = DefaultGroupName );

        EntityPtr createEntity( const string& name, const string& groupName = DefaultGroupName );

        MeshPtr createMesh( const string& name, const MeshType& meshType, const string& groupName = DefaultGroupName );

        //
        // Getters.

        GPUProgramPtr getGPUProgram( const string& name, const string& groupName = DefaultGroupName );

        MaterialPtr getMaterial( const string& name, const string& groupName = DefaultGroupName );

        TexturePtr getTexture( const string& name, const string& groupName = DefaultGroupName );

        //
        // Destruction.

        virtual void destroyTexture( TexturePtr texture ) = 0;

        virtual void destroyTexture( const string& name, const string& groupName = DefaultGroupName ) = 0;

    protected:

        using ResourceGroupMap = std::unordered_map<std::string, std::unique_ptr<ResourceGroup>>;
        using VertexBufferTable = Util::HashTable<MeshType, VertexBufferPtr>;

    protected:

        ResourceGroupPtr _getGroup( const string& groupName );

    protected:

        ResourceGroupMap _groups;
        ResourceGroupPtr _defaultGroup;
        std::unique_ptr<ResourceIndexer> _indexer;

        VertexBufferTable _vertexBufferTable;

    };

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    class LORE_EXPORT Resource final
    {

    public:

        //
        // Loading.

        static TexturePtr LoadTexture( const string& name, const string& file, const string& groupName = ResourceController::DefaultGroupName );

        //
        // Factory functions.

        static GPUProgramPtr CreateGPUProgram( const string& name, const string& groupName = ResourceController::DefaultGroupName );

        static ShaderPtr CreateVertexShader( const string& name, const string& groupName = ResourceController::DefaultGroupName );

        static ShaderPtr CreateFragmentShader( const string& name, const string& groupName = ResourceController::DefaultGroupName );

        static MaterialPtr CreateMaterial( const string& name, const string& groupName = ResourceController::DefaultGroupName );

        static MeshPtr CreateMesh( const string& name, const MeshType& meshType, const string& groupName = ResourceController::DefaultGroupName );

        static TexturePtr CreateTexture( const string& name, const string& groupName = ResourceController::DefaultGroupName );

        static CameraPtr CreateCamera( const string& name, const string& groupName = ResourceController::DefaultGroupName );

        //
        // Entity.

        static EntityPtr CreateEntity( const string& name, const MeshType& meshType, const string& groupName = ResourceController::DefaultGroupName );

        static EntityPtr CreateEntity( const string& name, const string& groupName = ResourceController::DefaultGroupName );

        //
        // Getters.

        static GPUProgramPtr GetGPUProgram( const string& name, const string& groupName = ResourceController::DefaultGroupName );

        static MaterialPtr GetMaterial( const string& name, const string& groupName = ResourceController::DefaultGroupName );

        static TexturePtr GetTexture( const string& name, const string& groupName = ResourceController::DefaultGroupName );

        //
        // Destruction.

        static void DestroyTexture( TexturePtr texture );

        static void DestroyTexture( const string& name, const string& groupName = ResourceController::DefaultGroupName );

    private:

        friend class Context;

    private:

        static void AssignContext( ContextPtr context );

    };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
