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

#include <LORE/LorePlatform.h>

#include <string> // TODO: Figure out nicer way for clients to include dependencies.

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  //
  // Forward declarations.

  class AABB;
  class Background;
  class Box;
  class Camera;
  class Context;
  class Entity;
  class Font;
  class GPUProgram;
  class InputController;
  class IResource;
  class Light;
  class Material;
  class Mesh;
  class Node;
  class PostProcessor;
  class Renderer;
  class RenderTarget;
  class ResourceController;
  class ResourceGroup;
  class Scene;
  class Shader;
  class Sprite;
  class SpriteAnimationSet;
  class SpriteController;
  class StockResourceController;
  class StockResourceFactory;
  class Textbox;
  class Texture;
  class UI;
  class UIPanel;
  class UIElement;
  class VertexBuffer;
  class Window;

  //
  // Pointer types.

  using AABBPtr = AABB*;
  using BackgroundPtr = Background*;
  using BoxPtr = Box*;
  using CameraPtr = Camera*;
  using ContextPtr = Context*;
  using FontPtr = Font*;
  using EntityPtr = Entity*;
  using GPUProgramPtr = GPUProgram*;
  using InputControllerPtr = InputController*;
  using IResourcePtr = IResource*;
  using LightPtr = Light*;
  using MaterialPtr = Material*;
  using MeshPtr = Mesh*;
  using NodePtr = Node*;
  using PostProcessorPtr = PostProcessor*;
  using RendererPtr = Renderer*;
  using RenderTargetPtr = RenderTarget*;
  using ResourceControllerPtr = ResourceController*;
  using ResourceGroupPtr = ResourceGroup*;
  using ScenePtr = Scene*;
  using ShaderPtr = Shader*;
  using SpritePtr = Sprite*;
  using SpriteAnimationSetPtr = SpriteAnimationSet*;
  using SpriteControllerPtr = SpriteController*;
  using StockResourceControllerPtr = StockResourceController*;
  using StockResourceFactoryPtr = StockResourceFactory*;
  using TextboxPtr = Textbox*;
  using TexturePtr = Texture*;
  using UIPtr = UI*;
  using UIPanelPtr = UIPanel*;
  using UIElementPtr = UIElement*;
  using VertexBufferPtr = VertexBuffer*;
  using WindowPtr = Window*;

  //
  // Types.

  using real = float;
  using uint = unsigned int;
  using string = std::string;

#if LORE_PLATFORM == LORE_WINDOWS

  #define LORE_VARIANT std::variant
  #define GET_VARIANT std::get
  #define HAS_VARIANT_TYPE std::holds_alternative
  #define MONOSTATE std::monostate

#elif LORE_PLATFORM == LORE_APPLE

  #define LORE_VARIANT mpark::variant
  #define GET_VARIANT mpark::get
  #define HAS_VARIANT_TYPE mpark::holds_alternative
  #define MONOSTATE mpark::monostate

#endif

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
