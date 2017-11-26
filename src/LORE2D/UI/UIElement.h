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

#include <LORE2D/Math/Math.h>
#include <LORE2D/Memory/Alloc.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  ///
  /// \class UIElement
  /// \brief Contains an entity/textbox to render on a UI panel.
  class LORE_EXPORT UIElement : public Alloc<UIElement>
  {

  public:

    UIElement() = default;
    ~UIElement() override = default;

    //
    // Getters.

    inline Vec2 getPosition() const
    {
      return _pos;
    }

    inline Vec2 getDimensions() const
    {
      return _dimensions;
    }

    inline real getDepth() const
    {
      return _depth;
    }

    inline EntityPtr getEntity() const
    {
      return _entity;
    }

    inline BoxPtr getBox() const
    {
      return _box;
    }

    inline TextboxPtr getTextbox() const
    {
      return _textbox;
    }

    //
    // Setters.

    inline void setPosition( const real x, const real y )
    {
      _pos = Vec2( x, y );
    }

    inline void setDimensions( const real w, const real h )
    {
      _dimensions = Vec2( w, h );
    }

    inline void setDepth( const real depth )
    {
      assert( depth <= 100.f && depth >= -100.f );
      _depth = depth;
    }

    inline void attachEntity( const EntityPtr entity )
    {
      _entity = entity;
    }

    inline void attachBox( const BoxPtr box )
    {
      _box = box;
    }

    inline void attachTextbox( const TextboxPtr textbox )
    {
      _textbox = textbox;
    }

  protected:

    virtual void _reset() override { }

  private:

    Vec2 _pos {};
    Vec2 _dimensions { 0.1f, 0.1f };
    real _depth { 0.f };
    EntityPtr _entity { nullptr };
    BoxPtr _box { nullptr };
    TextboxPtr _textbox { nullptr };

  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
