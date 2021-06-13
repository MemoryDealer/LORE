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

#include <LORE/Renderer/FrameListener/FrameListenerController.h>
#include <LORE/Resource/Color.h>
#include <LORE/Resource/IResource.h>
#include <LORE/Shader/GPUProgram.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  ///
  /// \class Material
  /// \brief Contains data on how to render an object, including lighting, colors,
  ///     and shader effects.
  /// \details Can have N number of passes.
  class LORE_EXPORT Material final : public Alloc<Material>, public IResource
  {

  public:

    enum class BlendFactor
    {
      Zero,
      One,
      SrcColor,
      OneMinusSrcColor,
      DstColor,
      OneMinusDstColor,
      SrcAlpha,
      OneMinusSrcAlpha,
      ConstantColor,
      OneMinusConstantColor,
      ConstantAlpha,
      OneMinusConstantAlpha
    };

  public:

    Material();

    virtual ~Material() override;

    MaterialPtr clone( const string& newName );

    //
    // Setters.

    void setTextureScrollSpeed( const glm::vec2& scroll );

    void setTextureSampleRegion( const Rect& region );

    void setTextureSampleRegion( const real x,
                                 const real y,
                                 const real w,
                                 const real h );

    //
    // Getters.

    inline glm::vec2 getTexCoordOffset() const
    {
      return _texCoordOffset;
    }

    inline Rect getTexSampleRegion() const
    {
      return _texSampleRegion;
    }

    Material& operator = ( const Material& rhs )
    {
      
      return *this;
    }

  public:

    bool lighting { true };
    Color ambient { StockColor::White };
    Color diffuse { StockColor::White };
    Color specular { StockColor::White };
    real shininess { 32.f };
    real opacity { 1.f };
    bool bloom { false };
    SpritePtr sprite { nullptr };
    GPUProgramPtr program { nullptr };
    
    glm::vec2 uvScale { 1.f, 1.f };

    struct
    {
      bool enabled { false };
      BlendFactor srcFactor { BlendFactor::SrcAlpha };
      BlendFactor dstFactor { BlendFactor::OneMinusSrcAlpha };
    } blendingMode;

  private:

    // TODO: Use linked list of materials for multi-pass rendering. The renderer
    // can pick these up and fill separate rendering queue groups for each pass.

    glm::vec2 _texCoordScrollSpeed { };
    glm::vec2 _texCoordOffset { };
    Rect _texSampleRegion { 0.f, 0.f, 1.f, 1.f };
    FrameListenerController::FrameStartedCallback _texCoordCallback { nullptr };

  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
