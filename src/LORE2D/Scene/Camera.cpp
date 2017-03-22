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

#include "Camera.h"

#include <LORE2D/Math/Math.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Camera::Camera( const string& name )
: _name( name )
, _position()
, _scale( 1.f, 1.f )
, _view()
, _dirty( true )
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Camera::~Camera()
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera::setPosition( const Vec2& pos )
{
    _position = pos;
    dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera::setPosition( const real x, const real y )
{
    _position.x = x;
    _position.y = y;
    dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera::translate( const Vec2& offset )
{
    _position += offset;
    dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera::translate( const real xOffset, const real yOffset )
{
    _position.x += xOffset;
    _position.y += yOffset;
    dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera::zoom( const real amount )
{
    _scale.x += amount;
    _scale.y += amount;
    dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera::setZoom( const real amount )
{
    _scale.x = _scale.y = amount;
    dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera::dirty()
{
    _dirty = true;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera::_reset()
{
    _position = Vec2();
    _scale = Vec2( 1.f, 1.f );
    _view = Matrix4();
    _dirty = true;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Matrix4 Camera::getViewMatrix()
{
    if ( _dirty ) {
        _updateViewMatrix();
    }

    return _view;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera::_updateViewMatrix()
{
    /*_view[3] = _view[0] * _position.x + _view[1] * _position.y + _view[3];
    _position.x = 0.f;
    _position.y = 0.f;*/

    /*Vec4 zoom( _scale.x, _scale.y, 1.f, 1.f );
    _view[0] *= zoom;
    _view[1] *= zoom;
    _view[2] *= zoom;
    _scale.x = _scale.y = 1.f;

    _view[3].x = _position.x;
    _view[3].y = _position.y;*/

    // TODO: Handle zooming properly.
    _view = Math::CreateTransformationMatrix( _position,
                                              Quaternion(),
                                              _scale );

    _dirty = false;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
