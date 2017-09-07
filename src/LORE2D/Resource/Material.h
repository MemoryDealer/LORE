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

#include <LORE2D/Memory/Alloc.h>
#include <LORE2D/Renderer/FrameListener/FrameListenerController.h>
#include <LORE2D/Resource/Color.h>
#include <LORE2D/Shader/GPUProgram.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  ///
  /// \class Material
  /// \brief Contains data on how to render an object, including lighting, colors,
  ///     and shader effects.
  /// \details Can have N number of passes.
  class LORE_EXPORT Material final : public Alloc<Material>
  {

    LORE_OBJECT_BODY()

  public:

    enum class BlendFactor
    {
      None,
      Zero,
      One,
      //
      SrcAlpha,
      OneMinusSrcAlpha
    };

    ///
    /// \class Material::Pass
    /// \brief A single vertex and pixel shader pass.
    class LORE_EXPORT Pass final
    {

    public:

      bool lighting { true };
      bool blending { false };
      Color ambient { StockColor::White };
      Color diffuse { StockColor::White };
      TexturePtr texture { nullptr };
      GPUProgramPtr program { nullptr };
      
      struct
      {
        BlendFactor srcFactor { BlendFactor::None };
        BlendFactor dstFactor { BlendFactor::None };
      } AlphaBlend;

    public:

      Pass();

      ~Pass();

      //
      // Setters.

      void setTextureScrollSpeed( const Vec2& scroll );

      void setTextureSampleRegion( const Rect& region );

      void setTextureSampleRegion( const real x,
                                   const real y,
                                   const real w,
                                   const real h );

      //
      // Getters.

      inline Vec2 getTexCoordOffset() const
      {
        return _texCoordOffset;
      }

      inline Rect getTexSampleRegion() const
      {
        return _texSampleRegion;
      }

    private:

      Vec2 _texCoordScrollSpeed {};
      Vec2 _texCoordOffset {};
      Rect _texSampleRegion { 0.f, 0.f, 1.f, 1.f };
      FrameListenerController::FrameStartedCallback _texCoordCallback { nullptr };

    };

  public:

    Material();

    virtual ~Material() override;

    //
    // Getters.

    ///
    /// \brief Returns reference to Pass at specified index. All Materials have
    ///     at least one pass.
    Pass& getPass( const size_t idx = 0 )
    {
      assert( idx <= _passes.size() );
      return _passes[idx];
    }

    //
    // Operators.

    Material& operator = ( const Material& rhs )
    {
      _passes = rhs._passes;
      return *this;
    }

  private:

    using PassList = std::vector<Pass>;

  private:

    virtual void _reset() override;

  private:

    PassList _passes;

  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
