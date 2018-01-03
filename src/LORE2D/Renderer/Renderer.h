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

#include <LORE2D/Window/RenderView.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  class IRenderAPI;

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  ///
  /// \struct RenderQueue
  /// \brief Holds a list of Renderables for rendering. RenderQueues are organized
  ///     and handled by the IRenderer implementation.
  struct RenderQueue
  {

    // Lore supports 100 render queues, rendered in order from 0-99.
    static const uint32_t Background = 0;
    static const uint32_t General = 50;
    static const uint32_t Foreground = 99;

    // :::::: //

    // An instance of a type of Renderable attached to a node (e.g., Texture).
    struct RenderData
    {
      Matrix4 model {};
      bool xFlipped { false };
      bool yFlipped { false };
    };
    using RenderDataList = std::vector<RenderData>;

    struct EntityData
    {
      MaterialPtr material { nullptr };
      VertexBufferPtr vertexBuffer { nullptr };
      size_t spriteFrame { 0 };

      bool operator < ( const EntityData& r ) const;
    };

    struct Transparent // TODO: This is inconsistent with EntityData, clean these up.
    {
      MaterialPtr material { nullptr };
      VertexBufferPtr vertexBuffer { nullptr };
      size_t spriteFrame { 0 };
      Matrix4 model {};
      bool xFlipped { false };
      bool yFlipped { false };
    };

    struct BoxData
    {
      BoxPtr box { nullptr };
      Matrix4 model {};
    };

    struct TextboxData
    {
      TextboxPtr textbox { nullptr };
      Matrix4 model {};
    };

    struct LightData
    {
      LightPtr light { nullptr };
      Vec2 pos {};
    };

    // Every Material maps to a list of RenderData.
    using EntityDataMap = std::map<EntityData, RenderDataList>;
    using TransparentDataMap = std::multimap<real, Transparent>;
    using BoxList = std::vector<BoxData>;
    using TextboxList = std::vector<TextboxData>;
    using LightList = std::vector<LightData>;

    EntityDataMap solids {};
    TransparentDataMap transparents {};
    BoxList boxes {};
    TextboxList textboxes {};
    LightList lights {};

  };

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  ///
  /// \class IRenderer
  /// \brief Interface for Renderers - the object that knows how to interpret
  ///     a RenderView and a Scene's scene graph and present an image to the 
  ///     window. Render plugins shall define these implementations.
  class Renderer
  {

  public:

    virtual ~Renderer() { }

    ///
    /// \brief Registers a Renderable object for rendering. This should be
    ///     called when a Renderable is attached to a Node.
    virtual void addRenderData( Lore::EntityPtr e,
                                Lore::NodePtr node ) = 0;

    virtual void addBox( Lore::BoxPtr box,
                         const Lore::Matrix4& transform ) = 0;

    virtual void addTextbox( Lore::TextboxPtr textbox,
                             const Lore::Matrix4& transform ) = 0;

    virtual void addLight( Lore::LightPtr light,
                           const Lore::NodePtr node ) = 0;

    ///
    /// \brief Uses internal Renderable lists to create a frame buffer using
    ///     the provided RenderView.
    virtual void present( const RenderView& rv,
                          const WindowPtr window ) = 0;

    void setRenderAPI( IRenderAPI* api )
    {
      _api = api;
    }

  protected:

    virtual void _clearRenderQueues() = 0;

  protected:

    IRenderAPI* _api { nullptr };

  };

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  // All built-in LORE renderer types.
  enum class RendererType
  {
    Generic
  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
