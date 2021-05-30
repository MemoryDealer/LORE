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

#include <LORE/Resource/IResource.h>

#include <LORE/Renderer/IRenderAPI.h>
#include <LORE/Renderer/Renderer.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  class LORE_EXPORT Prefab final : public Alloc<Prefab>, public IResource
  {

  public:

    Prefab() = default;
    ~Prefab() override;

    PrefabPtr clone( const string& name );

    //
    // Special instancing functions.

    ModelPtr getInstancedModel() const;
    size_t getInstanceCount() const;

    ///
    /// \brief All nodes that use this Prefab after a call to this function will be
    /// rendered with instancing. This greatly improves performance for many Nodes
    /// using the same Prefab.
    /// Note: This should be called before attaching this Prefab to all nodes desired
    /// to use instancing.
    void enableInstancing( const size_t max );

    ///
    /// \brief Restores original rendering mode. The internal instanced data will be
    /// destroyed.
    void disableInstancing();

    ///
    /// \brief This node's properties will be used for rendering in instancing mode
    /// (e.g., material, sprite controller settings. etc.).
    void setInstanceControllerNode( const NodePtr node );

    ///
    /// \brief Updates the matrix entry in the instanced buffer for the specified
    /// instance ID.
    void updateInstancedMatrix( const size_t idx, const glm::mat4& matrix );

    //
    // Helper functions.

    void setSprite( SpritePtr sprite );

    //
    // Modifiers.

    void setMaterial( MaterialPtr material ); 
    void setModel( ModelPtr buffer );

    //
    // Accessors.

    MaterialPtr getMaterial() const;
    ModelPtr getModel() const;
    uint getRenderQueue() const;
    bool isInstanced() const;
    NodePtr getInstanceControllerNode() const;

    //

    friend class Node;

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    void _notifyAttached( const NodePtr node );

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    MaterialPtr _material { nullptr };
    ModelPtr _model { nullptr };

    // Only used if Prefab is instanced.
    ModelPtr _instancedModel { nullptr };
    size_t _instanceCount { 0 };
    NodePtr _instanceControllerNode { nullptr };

    uint _renderQueue { RenderQueue::General };

    IRenderAPI::CullingMode cullingMode = IRenderAPI::CullingMode::Back;

  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
