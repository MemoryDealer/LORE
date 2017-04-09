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
#include <LORE2D/Resource/Color.h>
#include <LORE2D/Resource/Renderable/Renderable.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

    class Light final : public Renderable,
                        public Alloc<Light>
    {

        LORE_OBJECT_BODY()

    public:

        Light();

        virtual ~Light() override;

        //
        // Getters.

        inline Vec2 getPosition() const
        {
            return _position;
        }

        inline Color getColor() const
        {
            return _color;
        }

        inline real getRange() const
        {
            return _range;
        }

        inline real getConstant() const
        {
            return _constant;
        }

        inline real getLinear() const
        {
            return _linear;
        }

        inline real getQuadratic() const
        {
            return _quadratic;
        }

        inline real getIntensity() const
        {
            return _intensity;
        }

        //
        // Setters.

        inline void setColor( const Color& color )
        {
            _color = color;
        }

        inline void setAttenuation( const real range,
                                    const real constant,
                                    const real linear,
                                    const real quadratic )
        {
            _range = range;
            _constant = constant;
            _linear = linear;
            _quadratic = quadratic;
        }

        inline void setRange( const real range )
        {
            _range = range;
        }

        inline void setIntensity( const real intensity )
        {
            _intensity = intensity;
        }

    private:

        virtual void _reset() override;

        friend class Node;

    private:

        Vec2 _position;
        Color _color;

        // Attenuation.
        real _range;
        real _constant;
        real _linear;
        real _quadratic;
        real _intensity;

    };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
