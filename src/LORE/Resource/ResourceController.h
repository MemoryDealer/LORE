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

#include <LORE/Core/Util.h>
#include <LORE/Memory/Alloc.h>
#include <LORE/Resource/Font.h>
#include <LORE/Resource/Material.h>
#include <LORE/Resource/Mesh.h>
#include <LORE/Resource/IO/ResourceIndexer.h>
#include <LORE/Resource/IO/SerializableResource.h>
#include <LORE/Resource/IResource.h>
#include <LORE/Resource/Registry.h>
#include <LORE/Resource/Texture.h>
#include <LORE/Scene/Camera.h>
#include <LORE/Shader/GPUProgram.h>
#include <LORE/Shader/Shader.h>
#include <LORE/Shader/VertexBuffer.h>
#include <LORE/Window/RenderTarget.h>
#include <LORE/UI/UI.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  ///
  /// \class ResourceGroup
  /// \brief A collection of resources used for rendering and various effects.
  /// \details The default resource group is "Core", where resources that do not
  /// specify a resource group will go. Resource groups can be used to organize
  /// different stages in a game, for instance. Having the resources needed for
  /// all stages (character, UI, common items, etc.) in the core group, and
  /// resources for specific stages in their own separate groups, which can be
  /// loaded and unloaded as the player progresses through the game.
  class LORE_EXPORT ResourceGroup final
  {

  public:

    struct IndexedResource
    {
      SerializableResource type;
      string file;
      bool loaded;
    };

    using ResourceIndex = std::multimap<string, IndexedResource>;
    using ResourceRegistry = Registry<std::unordered_map, IResource>;
    using ResourceRegistryMap = std::unordered_map<std::type_index, ResourceRegistry>;

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    ResourceGroup( const string& name );
    ~ResourceGroup() = default;

    template<typename T>
    void insertResource( T* resource );

    template<typename T>
    T* getResource( const string& id );

    template<typename T>
    void removeResource( const string& id );

    //
    // Modifiers.

    void addIndexedResource( const string& name, const IndexedResource& ir );

    //
    // Accessors.

    const string& getName() const;

  private:

    friend class ResourceController;

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    template<typename T>
    void _addResourceType()
    {
      _resources[std::type_index( typeid( T ) )] = ResourceRegistry();
    }

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    string _name {};
    ResourceIndex _index {};
    ResourceRegistryMap _resources {};

  };

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  ///
  /// \class ResourceController
  /// \brief The prime entity which controls all resource lifetime.
  /// \details Holds all resource groups, provides creation, accessor, and destruction generic functions
  /// which are necessary to use a resource group. Must be overriden by render plugins to provide functors
  /// for resources that require render plugin APIs (e.g., textures, render targets, etc.).
  class LORE_EXPORT ResourceController
  {

  public:

    using ResourceGroupMap = std::unordered_map<string, std::shared_ptr<ResourceGroup>>; // TODO: Use unique_ptr (shared_ptr for now to avoid very difficult compilation error).
    using PluginCreationFunctor = std::function<IResourcePtr()>;
    using PluginDestructionFunctor = std::function<void( IResourcePtr )>;
    using PluginCreationFunctorMap = std::unordered_map<std::type_index, PluginCreationFunctor>;
    using PluginDestructionFunctorMap = std::unordered_map<std::type_index, PluginDestructionFunctor>;

    static const string DefaultGroupName;

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    ResourceController();
    virtual ~ResourceController();

    //
    // Resource groups, resource configuration and indexing.

    void createGroup( const string& groupName );
    void destroyGroup( const string& groupName );
    void loadGroup( const string& groupName );
    void unloadGroup( const string& groupName );
    void reloadGroup( const string& groupName );
    void loadResourceConfiguration( const string& file );
    void indexResourceLocation( const string& directory,
                                const string& groupName = DefaultGroupName,
                                const bool recursive = true );
    void indexResourceFile( const string& file, const string& groupName = DefaultGroupName );

    //
    // Resource management.

    // Creates resources that are implemented directly in LORE library.
    template<typename ResourceType>
    typename std::enable_if<std::is_base_of<Alloc<ResourceType>, ResourceType>::value, ResourceType*>::type
      create( const string& name, const string& groupName = DefaultGroupName );

    // Creates resources that are implemented by a render plugin, using functors the plugin provides.
    template<typename ResourceType>
    typename std::enable_if<!std::is_base_of<Alloc<ResourceType>, ResourceType>::value, ResourceType*>::type
      create( const string& name, const string& groupName = DefaultGroupName );

    // Returns true if specified resource exists.
    template<typename ResourceType>
    bool resourceExists( const string& name, const string& groupName = DefaultGroupName );

    // Returns pointer to specified resource. Throws ItemIdentityException if not found.
    template<typename ResourceType>
    ResourceType* get( const string& name, const string& groupName = DefaultGroupName );

    // Destroys resources that are implemented directly in LORE library.
    template<typename ResourceType>
    typename std::enable_if<std::is_base_of<Alloc<ResourceType>, ResourceType>::value, void>::type
      destroy( ResourceType* resource );

    // Destroys resources that are implemented by a render plugin, using functors the plugin provides.
    template<typename ResourceType>
    typename std::enable_if<!std::is_base_of<Alloc<ResourceType>, ResourceType>::value, void>::type
      destroy( ResourceType* resource );

    // Destroys all resources of specified type in specified group.
    template<typename ResourceType>
    void destroyAllInGroup( const string& groupName );

    //
    // Plugin functors.

    template<typename T>
    void addCreationFunctor( PluginCreationFunctor functor );

    template<typename T>
    void addDestructionFunctor( PluginDestructionFunctor functor );

    template<typename T>
    PluginCreationFunctor getCreationFunctor();

    template<typename T>
    PluginDestructionFunctor getDestructionFunctor();

  private:

    ResourceGroupPtr _getGroup( const string& groupName );

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    ResourceGroupMap _groups {};
    ResourceGroupPtr _defaultGroup { nullptr };

    PluginCreationFunctorMap _creationFunctors {};
    PluginDestructionFunctorMap _destructionFunctors {};

  };

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  ///
  /// \class Resource
  /// \brief A helper class with only static functions that communicate to the active ResourceController,
  /// which is owned by the active Context.
  class LORE_EXPORT Resource final
  {

  public:

    //
    // Resource groups, resource configuration and indexing.

    static void CreateGroup( const string& groupName );
    static void LoadGroup( const string& groupName );
    static void UnloadGroup( const string& groupName );
    static void ReloadGroup( const string& groupName );
    static void LoadResourceConfiguration( const string& file );
    static void IndexResourceLocation( const string& directory,
                                       const string& groupName = ResourceController::DefaultGroupName,
                                       const bool recursive = true );

    //
    // Functions for loading resources from the storage.

    static FontPtr LoadFont( const string& name,
                             const string& file,
                             const uint32_t size,
                             const string& groupName = ResourceController::DefaultGroupName );
    static TexturePtr LoadTexture( const string& name,
                                   const string& file,
                                   const string& groupName = ResourceController::DefaultGroupName );

    //
    // Factory functions for creation of resources.

    static BoxPtr CreateBox( const string& name,
                             const string& groupname = ResourceController::DefaultGroupName );
    static EntityPtr CreateEntity( const string& name,
                                   const VertexBuffer::Type& vbType,
                                   const string& groupName = ResourceController::DefaultGroupName );
    static GPUProgramPtr CreateGPUProgram( const string& name,
                                           const string& groupName = ResourceController::DefaultGroupName );
    static MaterialPtr CreateMaterial( const string& name,
                                       const string& groupName = ResourceController::DefaultGroupName );
    static MeshPtr CreateMesh( const string& name,
                               const VertexBuffer::Type& vbType,
                               const string& groupName = ResourceController::DefaultGroupName );
    static RenderTargetPtr CreateRenderTarget( const string& name,
                                               const uint32_t width,
                                               const uint32_t height,
                                               const string& groupName = ResourceController::DefaultGroupName );
    static ShaderPtr CreateShader( const string& name,
                                   const Shader::Type type,
                                   const string& groupName = ResourceController::DefaultGroupName );
    static SpritePtr CreateSprite( const string& name,
                                   const string& groupName = ResourceController::DefaultGroupName );
    static SpriteAnimationSetPtr CreateSpriteAnimationSet( const string& name,
                                                           const string& groupName = ResourceController::DefaultGroupName );
    static TexturePtr CreateTexture( const string& name,
                                     const uint32_t width,
                                     const uint32_t height,
                                     const Color& color,
                                     const string& groupName = ResourceController::DefaultGroupName );
    static TextboxPtr CreateTextbox( const string& name,
                                     const string& groupname = ResourceController::DefaultGroupName );
    static UIPtr CreateUI( const string& name,
                           const string& groupName = ResourceController::DefaultGroupName );
    static VertexBufferPtr CreateVertexBuffer( const string& name,
                                               const VertexBuffer::Type& type,
                                               const string& groupName = ResourceController::DefaultGroupName );

    //
    // Getters.

    static BoxPtr GetBox( const string& name,
                          const string& groupName = ResourceController::DefaultGroupName );
    static EntityPtr GetEntity( const string& name,
                                const string& groupName = ResourceController::DefaultGroupName );
    static FontPtr GetFont( const string& name,
                            const string& groupName = ResourceController::DefaultGroupName );
    static GPUProgramPtr GetGPUProgram( const string& name,
                                        const string& groupName = ResourceController::DefaultGroupName );
    static MaterialPtr GetMaterial( const string& name,
                                    const string& groupName = ResourceController::DefaultGroupName );
    static MeshPtr GetMesh( const string& name,
                            const string& groupName = ResourceController::DefaultGroupName );
    static RenderTargetPtr GetRenderTarget( const string& name,
                                            const string& groupName = ResourceController::DefaultGroupName );
    static ShaderPtr GetShader( const string& name,
                                const string& groupName = ResourceController::DefaultGroupName );
    static SpritePtr GetSprite( const string& name,
                                const string& groupName = ResourceController::DefaultGroupName );
    static SpriteAnimationSetPtr GetSpriteAnimationSet( const string& name,
                                                        const string& groupName = ResourceController::DefaultGroupName );
    static TexturePtr GetTexture( const string& name,
                                  const string& groupName = ResourceController::DefaultGroupName );
    static TextboxPtr GetTextbox( const string& name,
                                  const string& groupName = ResourceController::DefaultGroupName );
    static UIPtr GetUI( const string& name,
                        const string& groupName = ResourceController::DefaultGroupName );
    static VertexBufferPtr GetVertexBuffer( const string& name,
                                            const string& groupName = ResourceController::DefaultGroupName );

    //
    // Destruction methods.

    static void DestroyBox( BoxPtr box );
    static void DestroyEntity( EntityPtr entity );
    static void DestroyFont( FontPtr font );
    static void DestroyGPUProgram( GPUProgramPtr program );
    static void DestroyMaterial( MaterialPtr material );
    static void DestroyMesh( MeshPtr mesh );
    static void DestroyRenderTarget( RenderTargetPtr rt );
    static void DestroyShader( ShaderPtr shader );
    static void DestroySprite( SpritePtr sprite );
    static void DestroySpriteAnimationSet( SpriteAnimationSetPtr sas );
    static void DestroyTexture( TexturePtr texture );
    static void DestroyTextbox( TextboxPtr textbox );
    static void DestroyUI( UIPtr ui );
    static void DestroyVertexBuffer( VertexBufferPtr vb );

    //
    // Accessors.

    ///
    /// \brief Returns resource controller of active context. It is recommended to 
    /// use the static functions provided in the Resource class instead.
    static ResourceControllerPtr GetResourceController();

  private:

    friend class Context;

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    static void AssignContext( ContextPtr context );

  };

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  // Implement template functions.
  #include "ResourceController.inl"

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
