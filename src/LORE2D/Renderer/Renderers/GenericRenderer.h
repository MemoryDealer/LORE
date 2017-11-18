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

#include <LORE2D/Renderer/Renderer.h>

#include <LORE2D/Resource/Material.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  class IRenderAPI;

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  ///
  /// \class GenericRenderer
  /// \brief Renders a scene normally, without any special behavior.
  class GenericRenderer : public Lore::Renderer
  {

  public:

    const size_t DefaultRenderQueueCount = 100;

  public:

    GenericRenderer();

    virtual ~GenericRenderer() override;

    virtual void addRenderData( Lore::EntityPtr e,
                                Lore::NodePtr node ) override;

    virtual void addBox( Lore::BoxPtr box,
                         const Lore::Matrix4& transform ) override;

    virtual void addTextbox( Lore::TextboxPtr textbox,
                             const Lore::Matrix4& transform ) override;

    virtual void present( const Lore::RenderView& rv,
                          const WindowPtr window ) override;

  private:

    virtual void _clearRenderQueues() override;

    void activateQueue( const uint id,
                        Lore::RenderQueue& rq );

    void renderBackground( const Lore::RenderView& rv,
                            const real aspectRatio,
                            const Matrix4& proj );

    void renderMaterialMap( const Lore::ScenePtr scene,
                            RenderQueue::EntityDataMap& mm,
                            const Matrix4& viewProjection ) const;

    void renderTransparents( const Lore::ScenePtr scene,
                              RenderQueue::TransparentDataMap& tm,
                              const Matrix4& viewProjection ) const;

    void renderBoxes( RenderQueue::BoxList& boxes,
                      const Matrix4& viewProjection ) const;

    void renderTextboxes( RenderQueue::TextboxList& textboxes,
                          const Matrix4& viewProjection ) const;

    void renderUI( const Lore::RenderView& rv,
                   const real aspectRatio,
                   const Matrix4& proj ) const;

  private:

    using RenderQueueList = std::vector<RenderQueue>;
    using ActiveRenderQueueList = std::map<uint, RenderQueue&>;

  private:

    RenderQueueList _queues { };
    ActiveRenderQueueList _activeQueues { };

  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
