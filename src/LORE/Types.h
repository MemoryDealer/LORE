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

#include <LORE/LorePlatform.h>

#include <memory>
#include <string> // TODO: Figure out nicer way for clients to include dependencies.

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  //
  // Forward declarations.

  class AABB;
  class Skybox;
  class Box;
  class Camera;
  class Context;
  struct DirectionalLight;
  class Prefab;
  class Font;
  class GPUProgram;
  class InputController;
  class IResource;
  struct Light;
  class Material;
  class Mesh;
  class Model;
  class Node;
  struct PointLight;
  class PostProcessor;
  class Renderer;
  class RenderTarget;
  class ResourceController;
  class ResourceGroup;
  class Scene;
  class Shader;
  struct SpotLight;
  class Sprite;
  class SpriteAnimationSet;
  class SpriteController;
  class StockResourceController;
  class StockResourceFactory;
  class Textbox;
  class Texture;
  class UI;
  class Window;

  //
  // Pointer types.

  using AABBPtr = AABB*;
  using SkyboxPtr = Skybox*;
  using BoxPtr = Box*;
  using CameraPtr = Camera*;
  using ContextPtr = Context*;
  using DirectionalLightPtr = DirectionalLight*;
  using FontPtr = Font*;
  using PrefabPtr = Prefab*;
  using GPUProgramPtr = GPUProgram*;
  using InputControllerPtr = InputController*;
  using IResourcePtr = IResource*;
  using LightPtr = Light*;
  using MaterialPtr = Material*;
  using MeshPtr = Mesh*;
  using ModelPtr = Model*;
  using NodePtr = Node*;
  using PointLightPtr = PointLight*;
  using PostProcessorPtr = PostProcessor*;
  using RendererPtr = Renderer*;
  using RenderTargetPtr = RenderTarget*;
  using ResourceControllerPtr = ResourceController*;
  using ResourceGroupPtr = ResourceGroup*;
  using ScenePtr = Scene*;
  using ShaderPtr = Shader*;
  using SpotLightPtr = SpotLight*;
  using SpritePtr = Sprite*;
  using SpriteAnimationSetPtr = SpriteAnimationSet*;
  using SpriteControllerPtr = SpriteController*;
  using StockResourceControllerPtr = StockResourceController*;
  using StockResourceFactoryPtr = StockResourceFactory*;
  using TextboxPtr = Textbox*;
  using TexturePtr = Texture*;
  using UIPtr = std::shared_ptr<UI>;
  using WindowPtr = Window*;

  //
  // Types.

  using real = float;
  using uint = unsigned int;
  using u8 = uint8_t;
  using u16 = uint16_t;
  using u32 = uint32_t;
  using u64 = uint64_t;
  using i8 = int8_t;
  using i16 = int16_t;
  using i32 = int32_t;
  using i64 = int64_t;
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

  //
  // Structs.

  struct Dimensions
  {
    int width {};
    int height {};
  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
