#pragma once
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// The MIT License (MIT)
// This source file is part of LORE
// ( Lightweight Object-oriented Rendering Engine )
//
// Copyright (c) 2017-2021 Jordan Sparks
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

#include <LORE/Renderer/Renderer.h>

#include <LORE/Resource/Material.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  using RenderQueueList = std::vector<RenderQueue>;
  using ActiveRenderQueueList = std::map<uint, RenderQueue&>;

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  ///
  /// \class Forward3DRenderer
  /// \brief A basic 3D forward renderer.
  class Forward3DRenderer : public Lore::Renderer
  {

    void _presentPostProcessing( const RenderView& rv,
      const WindowPtr window );

    void _clearRenderQueues() override;

    void _activateQueue( const uint id,
      RenderQueue& rq );

    void _renderShadowMaps( const RenderView& rv,
      const RenderQueue& queue );

    void _renderSkybox( const RenderView& rv,
      const glm::mat4& viewProjection ) const;

    void _renderSolids( const RenderView& rv,
      const RenderQueue& queue,
      const glm::mat4& viewProjection ) const;

    void _renderTransparents( const RenderView& rv,
      const RenderQueue& queue,
      const glm::mat4& viewProjection ) const;

    void _renderBoxes( const RenderQueue& queue,
      const glm::mat4& viewProjection ) const;

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    RenderQueueList _queues { };
    ActiveRenderQueueList _activeQueues { };

    CameraPtr _camera { nullptr };

  public:

    Forward3DRenderer();
    ~Forward3DRenderer() override = default;

    void addRenderData( PrefabPtr e,
                        NodePtr node ) override;

    void addBox( BoxPtr box,
                 const glm::mat4& transform ) override;

    void addTextbox( TextboxPtr textbox,
                     const glm::mat4& transform ) override;

    void addLight( LightPtr light,
                   const NodePtr node ) override;

    void present( const RenderView& rv,
                  const WindowPtr window ) override;

  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
