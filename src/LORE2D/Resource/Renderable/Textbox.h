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
#include <LORE2D/Renderer/Renderer.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  class LORE_EXPORT Textbox final : public Alloc<Textbox>
  {

    LORE_OBJECT_BODY()

  public:

    Textbox() = default;
    ~Textbox() = default;

    // TODO: Add "typed" text effect where each letter appears over time at certain rate.
    //   This can be an external class such as TextboxTypist. 
    // Add solid/blended box behind text and border.

    //
    // Setters.

    void setText( const string& text )
    {
      _text = text;
    }

    void setFont( FontPtr font )
    {
      _font = font;
    }

    void setTextColor( const Color& color )
    {
      _textColor = color;
    }

    //
    // Getters.

    string getText() const
    {
      return _text;
    }

    FontPtr getFont() const
    {
      return _font;
    }

    Color getTextColor() const
    {
      return _textColor;
    }

    uint32_t getRenderQueue() const
    {
      return _renderQueue;
    }

  private:

    virtual void _reset() override
    { }

  private:

    FontPtr _font { nullptr };
    string _text {};
    Color _textColor { StockColor::White };
    uint32_t _renderQueue { RenderQueue::General };

  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
