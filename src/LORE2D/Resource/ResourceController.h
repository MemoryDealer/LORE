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
  struct LORE_EXPORT ResourceGroup
  {

    struct IndexedResource
    {
      SerializableResource type;
      string file;
      bool loaded;
    };

    using ShaderRegistry = Registry<std::unordered_map, Shader>;
    using ResourceIndex = std::multimap<string, IndexedResource>;

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    string name {};
    ResourceIndex index {};

    Registry<std::unordered_map, Texture> textures {};
    ShaderRegistry vertexShaders {};
    ShaderRegistry fragmentShaders {};
    Registry<std::unordered_map, GPUProgram> programs {};
    Registry<std::unordered_map, VertexBuffer> vertexBuffers {};
    Registry<std::unordered_map, Material> materials {};
    Registry<std::unordered_map, Mesh> meshes {};
    Registry<std::unordered_map, Entity> entities {};
    Registry<std::unordered_map, Font> fonts {};
    Registry<std::unordered_map, Box> boxes {};
    Registry<std::unordered_map, Textbox> textboxes { };
    Registry<std::unordered_map, Sprite> sprites {};
    Registry<std::unordered_map, SpriteAnimationSet> animationSets {};
    Registry<std::unordered_map, RenderTarget> renderTargets {};
    Registry<std::unordered_map, UI> uis {};
    Registry<std::unordered_map, Camera> cameras { };

  };

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  class LORE_EXPORT ResourceController
  {

  public:

    using ResourceGroupMap = std::unordered_map<std::string, std::shared_ptr<ResourceGroup>>; // TODO: Use unique_ptr (shared_ptr for now to avoid very difficult compilation error).
    using VertexBufferTable = Util::HashTable<MeshType, VertexBufferPtr>;

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

    EntityPtr createEntity( const string& name,
                            const MeshType& meshType,
                            const string& groupName = DefaultGroupName );
    SpritePtr createSprite( const string& name,
                            const string& groupName = DefaultGroupName );
    SpriteAnimationSetPtr createAnimationSet( const string& name,
                                              const string& groupName = DefaultGroupName );
    MaterialPtr createMaterial( const string& name,
                                const string& groupName = DefaultGroupName );
    CameraPtr createCamera( const string& name,
                            const string& groupName = DefaultGroupName );
    BoxPtr createBox( const string& name,
                      const string& groupName = DefaultGroupName );
    TextboxPtr createTextbox( const string& name,
                              const string& groupName = DefaultGroupName );
    UIPtr createUI( const string& name,
                    const string& groupName = DefaultGroupName );
    MeshPtr createMesh( const string& name,
                        const MeshType& meshType,
                        const string& groupName = DefaultGroupName );

    //
    // Cloning functions (TODO: Move cloning functions to objects with prototype pattern).

    MaterialPtr cloneMaterial( const string& name,
                               const string& cloneName );

    //
    // Existence checkers.

    bool textureExists( const string& name,
                        const string& groupName = DefaultGroupName );

    //
    // Getters.

    GPUProgramPtr getGPUProgram( const string& name,
                                 const string& groupName = DefaultGroupName );
    SpritePtr getSprite( const string& name,
                         const string& groupName = DefaultGroupName );
    SpriteAnimationSetPtr getAnimationSet( const string& name,
                                           const string& groupName = DefaultGroupName );
    MaterialPtr getMaterial( const string& name,
                             const string& groupName = DefaultGroupName );
    TexturePtr getTexture( const string& name,
                           const string& groupName = DefaultGroupName );
    VertexBufferPtr getVertexBuffer( const string& name,
                                     const string& groupName = DefaultGroupName );
    FontPtr getFont( const string& name,
                     const string& groupName = DefaultGroupName );

    //
    // Destruction methods (non-virtual).

    void destroyMesh( MeshPtr mesh );
    void destroyEntity( EntityPtr entity );
    void destroyBox( BoxPtr box );
    void destroyTextbox( TextboxPtr textbox );
    void destroySprite( SpritePtr sprite );
    void destroyAnimationSet( SpriteAnimationSetPtr animationSet );
    void destroyUI( UIPtr ui );

    //
    // Functions for loading resources from the storage.

    virtual TexturePtr loadTexture( const string& name,
                                    const string& file,
                                    const string& groupName = DefaultGroupName ) = 0;
    virtual FontPtr loadFont( const string& name,
                              const string& file,
                              const uint32_t size,
                              const string& groupName = ResourceController::DefaultGroupName ) = 0;

    //
    // Factory functions for creation of resources (implemented by render plugin).

    virtual GPUProgramPtr createGPUProgram( const string& name,
                                            const string& groupName = DefaultGroupName ) = 0;
    virtual ShaderPtr createVertexShader( const string& name,
                                          const string& groupName = DefaultGroupName ) = 0;
    virtual ShaderPtr createFragmentShader( const string& name,
                                            const string& groupName = DefaultGroupName ) = 0;
    virtual VertexBufferPtr createVertexBuffer( const string& name,
                                                const MeshType& type,
                                                const string& groupName = DefaultGroupName ) = 0;
    virtual TexturePtr createTexture( const string& name,
                                      const uint32_t width,
                                      const uint32_t height,
                                      const string& groupName = DefaultGroupName ) = 0;
    virtual TexturePtr createTexture( const string& name,
                                      const uint32_t width,
                                      const uint32_t height,
                                      const Color& color,
                                      const string& groupName = DefaultGroupName ) = 0;
    virtual RenderTargetPtr createRenderTarget( const string& name,
                                                const uint32_t width,
                                                const uint32_t height,
                                                const string& groupName = DefaultGroupName ) = 0;

    //
    // Destruction methods (implemented by render plugin).

    virtual void destroyTexture( TexturePtr texture ) = 0;
    virtual void destroyTexture( const string& name,
                                 const string& groupName = DefaultGroupName ) = 0;
    virtual void destroyFont( FontPtr font ) = 0;
    virtual void destroyVertexShader( ShaderPtr vertexShader ) = 0;
    virtual void destroyFragmentShader( ShaderPtr fragmentShader ) = 0;
    virtual void destroyGPUProgram( GPUProgramPtr gpuProgram ) = 0;
    virtual void destroyVertexBuffer( VertexBufferPtr vertexBuffer ) = 0;
    virtual void destroyRenderTarget( RenderTargetPtr renderTarget ) = 0;

  protected:

    ResourceGroupPtr _getGroup( const string& groupName );

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    ResourceGroupMap _groups {};
    ResourceGroupPtr _defaultGroup { nullptr };

    VertexBufferTable _vertexBufferTable {}; // HACK

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

    static TexturePtr LoadTexture( const string& name,
                                   const string& file,
                                   const string& groupName = ResourceController::DefaultGroupName );
    static FontPtr LoadFont( const string& name,
                             const string& file,
                             const uint32_t size,
                             const string& groupName = ResourceController::DefaultGroupName );

    //
    // Factory functions for creation of resources.

    static EntityPtr CreateEntity( const string& name,
                                   const MeshType& meshType,
                                   const string& groupName = ResourceController::DefaultGroupName );
    static GPUProgramPtr CreateGPUProgram( const string& name,
                                           const string& groupName = ResourceController::DefaultGroupName );
    static ShaderPtr CreateVertexShader( const string& name,
                                         const string& groupName = ResourceController::DefaultGroupName );
    static ShaderPtr CreateFragmentShader( const string& name,
                                           const string& groupName = ResourceController::DefaultGroupName );
    static VertexBufferPtr CreateVertexBuffer( const string& name,
                                               const MeshType& type,
                                               const string& groupName = ResourceController::DefaultGroupName );
    static SpritePtr CreateSprite( const string& name,
                                   const string& groupName = ResourceController::DefaultGroupName );
    static MaterialPtr CreateMaterial( const string& name,
                                       const string& groupName = ResourceController::DefaultGroupName );
    static MeshPtr CreateMesh( const string& name,
                               const MeshType& meshType,
                               const string& groupName = ResourceController::DefaultGroupName );
    static TexturePtr CreateTexture( const string& name,
                                     const uint32_t width,
                                     const uint32_t height,
                                     const string& groupName = ResourceController::DefaultGroupName );
    static RenderTargetPtr CreateRenderTarget( const string& name,
                                               const uint32_t width,
                                               const uint32_t height,
                                               const string& groupName = ResourceController::DefaultGroupName );
    static CameraPtr CreateCamera( const string& name,
                                   const string& groupName = ResourceController::DefaultGroupName );
    static BoxPtr CreateBox( const string& name,
                             const string& groupname = ResourceController::DefaultGroupName );
    static TextboxPtr CreateTextbox( const string& name,
                                     const string& groupname = ResourceController::DefaultGroupName );
    static UIPtr CreateUI( const string& name,
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
    static void DestroyTexture( const string& name,
                                const string& groupName = ResourceController::DefaultGroupName );

  private:

    friend class Context;

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    static void AssignContext( ContextPtr context );

  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
