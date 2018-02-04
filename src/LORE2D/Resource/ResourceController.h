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
#include <LORE2D/Resource/Font.h>
#include <LORE2D/Resource/Material.h>
#include <LORE2D/Resource/Mesh.h>
#include <LORE2D/Resource/Registry.h>
#include <LORE2D/Resource/IResource.h>
#include <LORE2D/Resource/ResourceIndexer.h>
#include <LORE2D/Resource/SerializableResource.h>
#include <LORE2D/Resource/Texture.h>
#include <LORE2D/Scene/Camera.h>
#include <LORE2D/Shader/GPUProgram.h>
#include <LORE2D/Shader/Shader.h>
#include <LORE2D/Shader/VertexBuffer.h>
#include <LORE2D/Window/RenderTarget.h>
#include <LORE2D/UI/UI.h>

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
  class LORE_EXPORT ResourceGroup
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

    template<typename T>
    void insertResource( T* resource )
    {
      ResourceRegistry& registry = _resources[std::type_index( typeid( T ) )];
      registry.insert( resource->getName(), resource );
    }

    template<typename T>
    T* getResource( const string& id )
    {
      ResourceRegistry& registry = _resources[std::type_index( typeid( T ) )];
      return static_cast< T* >( registry.get( id ) );
    }

    template<typename T>
    void removeResource( const string& id )
    {
      ResourceRegistry& registry = _resources[std::type_index( typeid( T ) )];
      registry.remove( id );
    }

    //
    // Modifiers.

    void addIndexedResource( const string& name, const IndexedResource& ir );

    //
    // Accessors.

    const string& getName() const;

    ResourceIndex& getResourceIndex();

  private:

    template<typename T>
    void addResourceType()
    {
      _resources[std::type_index( typeid( T ) )] = ResourceRegistry();
    }

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    string _name {};
    ResourceIndex _index {};
    ResourceRegistryMap _resources {};

  };

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  class LORE_EXPORT ResourceController
  {

  public:

    using ResourceGroupMap = std::unordered_map<std::string, std::shared_ptr<ResourceGroup>>; // TODO: Use unique_ptr (shared_ptr for now to avoid very difficult compilation error).
    using PluginFunctor = std::function<IResourcePtr()>;
    using PluginFunctorMap = std::unordered_map<std::type_index, PluginFunctor>;

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
    // Factory functions for creation of resources (non-virtual).

    template<typename ResourceType>
    typename std::enable_if<std::is_base_of<Alloc<ResourceType>, ResourceType>::value, ResourceType*>::type
      create( const string& name, const string& groupName = DefaultGroupName )
    {
      static_assert( std::is_base_of<IResource, ResourceType>::value, "ResourceType must derived from IResource" );
      ResourceType* resource = nullptr;

      // Allocate object directly in LORE2D lib.
      resource = MemoryAccess::GetPrimaryPoolCluster()->create<ResourceType>();
      resource->setName( name );
      resource->setResourceGroupName( groupName );
      _getGroup( groupName )->insertResource<ResourceType>( resource );
      return resource;
    }

    // For types created in the render plugin.
    template<typename ResourceType>
    typename std::enable_if<!std::is_base_of<Alloc<ResourceType>, ResourceType>::value, ResourceType*>::type
      create( const string& name, const string& groupName = DefaultGroupName )
    {
      static_assert( std::is_base_of<IResource, ResourceType>::value, "ResourceType must derived from IResource" );
      ResourceType* resource = nullptr;

      // This is a derived resource that is defined in the render plugin. Call the functor bound
      // for this type to get the allocated result.
      auto functor = getCreationFunctor<ResourceType>();
      if ( functor ) {
        resource = static_cast< ResourceType* >( functor() );
        resource->setName( name );
        resource->setResourceGroupName( groupName );
        _getGroup( groupName )->insertResource<ResourceType>( resource );
      }
      return resource;
    }

    template<typename ResourceType>
    bool resourceExists( const string& name, const string& groupName = DefaultGroupName )
    {
      return ( _getGroup( groupName )->getResource( name ) != nullptr );
    }

    template<typename ResourceType>
    ResourceType* get( const string& name, const string& groupName = DefaultGroupName )
    {
      return _getGroup( groupName )->getResource<ResourceType>( name );
    }

    template<typename ResourceType>
    typename std::enable_if<std::is_base_of<Alloc<ResourceType>, ResourceType>::value, void>::type
      destroy( ResourceType* resource )
    {
      auto groupName = resource->getResourceGroupName();
      _getGroup( groupName )->removeResource<ResourceType>( resource->getName() );
      MemoryAccess::GetPrimaryPoolCluster()->destroy<ResourceType>( resource );
    }

    template<typename ResourceType>
    typename std::enable_if<!std::is_base_of<Alloc<ResourceType>, ResourceType>::value, void>::type
      destroy( ResourceType* resource )
    {
      auto groupName = resource->getResourceGroupName();
      _getGroup( groupName )->removeResource<ResourceType>( resource->getName() );
      // Get functor.
    }

    //
    // Cloning functions (TODO: Move cloning functions to objects with prototype pattern).

    MaterialPtr cloneMaterial( const string& name,
                               const string& cloneName );

    //
    // Plugin functors.

    template<typename T>
    void addCreationFunctor( PluginFunctor functor )
    {
      const auto t = std::type_index( typeid( T ) );
      _creationFunctors[t] = functor;
    }

    template<typename T>
    PluginFunctor getCreationFunctor()
    {
      const auto t = std::type_index( typeid( T ) );
      auto lookup = _creationFunctors.find( t );
      if ( _creationFunctors.end() != lookup ) {
        return lookup->second;
      }
      return nullptr;
    }

  private:

    ResourceGroupPtr _getGroup( const string& groupName );

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    ResourceGroupMap _groups {};
    ResourceGroupPtr _defaultGroup { nullptr };

    PluginFunctorMap _creationFunctors {};
    PluginFunctorMap _destructionFunctors {};

  };

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

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
    static CameraPtr CreateCamera( const string& name,
                                   const string& groupName = ResourceController::DefaultGroupName );
    static EntityPtr CreateEntity( const string& name,
                                   const MeshType& meshType,
                                   const string& groupName = ResourceController::DefaultGroupName );
    static GPUProgramPtr CreateGPUProgram( const string& name,
                                           const string& groupName = ResourceController::DefaultGroupName );
    static MaterialPtr CreateMaterial( const string& name,
                                       const string& groupName = ResourceController::DefaultGroupName );
    static MeshPtr CreateMesh( const string& name,
                               const MeshType& meshType,
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
                                               const MeshType& type,
                                               const string& groupName = ResourceController::DefaultGroupName );


    //
    // Cloning functions.

    ///
    /// \brief Clones specified material and registers new material.
    /// \param name Material to clone.
    /// \param cloneName New name of the cloned material.
    static MaterialPtr CloneMaterial( const string& name,
                                      const string& cloneName );

    //
    // Getters.

    static GPUProgramPtr GetGPUProgram( const string& name,
                                        const string& groupName = ResourceController::DefaultGroupName );
    static SpritePtr GetSprite( const string& name,
                                const string& groupName = ResourceController::DefaultGroupName );
    static SpriteAnimationSetPtr GetAnimationSet( const string& name,
                                                  const string& groupName = ResourceController::DefaultGroupName );
    static MaterialPtr GetMaterial( const string& name,
                                    const string& groupName = ResourceController::DefaultGroupName );
    static TexturePtr GetTexture( const string& name,
                                  const string& groupName = ResourceController::DefaultGroupName );
    static FontPtr GetFont( const string& name,
                            const string& groupName = ResourceController::DefaultGroupName );

    //
    // Destruction methods.

    static void DestroyTexture( TexturePtr texture );

  private:

    friend class Context;

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    static void AssignContext( ContextPtr context );

  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
