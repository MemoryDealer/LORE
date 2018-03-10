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

#include "Camera.h"

#include <LORE/Math/Math.h>
#include <LORE/Resource/ResourceController.h>
#include <LORE/Scene/Node.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace LocalNS {

  constexpr real ZoomLimit = 0.1f;

}
using namespace LocalNS;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera::setPosition( const Vec2& pos )
{
  setPosition( Vec3( pos ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera::setPosition( const real x, const real y )
{
  setPosition( x, y, 0.f ); // Call Vec3 function with z-value of zero.
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera::setPosition( const real x, const real y, const real z )
{
  setPosition( Vec3( x, y, 0.f ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera::translate( const Vec2& offset )
{
  translate( Vec3( offset ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera::translate( const real xOffset, const real yOffset )
{
  translate( xOffset, yOffset, 0.f ); // Call Vec3 function with z-value of zero.
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera::translate( const real xOffset, const real yOffset, const real zOffset )
{
  translate( Vec3( xOffset, yOffset, zOffset ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera::zoom( const real amount )
{
  _zoom += amount * _zoom; // Scale zooming speed as it goes farther.
  if ( _zoom < ZoomLimit ) {
    _zoom = ZoomLimit;
  }

  _dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera::setZoom( const real amount )
{
  _zoom = amount;
  if ( _zoom < ZoomLimit ) {
    _zoom = ZoomLimit;
  }
  
  _dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

string Camera::getName() const
{
  return _name;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Vec3 Camera::getPosition() const
{
  return _position;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Matrix4 Camera::getViewMatrix()
{
  if ( _viewMatrixDirty ) {
    _updateViewMatrix();
    _viewMatrixDirty = false;
  }

  return _view;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera::trackNode( NodePtr node, const TrackingStyle& mode )
{
  _trackingNode = node;
  _trackingStyle = mode;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera::updateTracking( const real aspectRatio )
{
  if ( !_trackingNode ) {
    return;
  }

  const Vec3 nodePos = _trackingNode->getPosition();

  switch ( _trackingStyle ) {
  default:
  case TrackingStyle::Simple:
    _position.x = nodePos.x / aspectRatio;
    _position.y = nodePos.y;
    break;
  }

  _dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera::_dirty()
{
  _viewMatrixDirty = true;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera2D::setPosition( const Vec3& pos )
{
  _position = pos / _zoom;
  _dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera2D::translate( const Vec3& offset )
{
  _position += offset / _zoom;
  _dirty();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera2D::_updateViewMatrix()
{
  Vec3 scale( _zoom, _zoom, 1.f );
  _position.z = 0.f;
  _view = Math::CreateTransformationMatrix( _position,
                                            Quaternion(),
                                            scale );

  // Wtf? Have to invert x/y values...not sure why at the moment.
  _view[3][0] *= -_zoom;
  _view[3][1] *= -_zoom;
  // TODO: What to do with z view value?
  //_view[3][2] *= _zoom;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera3D::setPosition( const Vec3& pos )
{
  _position = pos;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera3D::translate( const Vec3& offset )
{
  _position += offset;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera3D::lookAt( const Vec3& pos, const Vec3& target, const Vec3& up )
{
  _look = ( target - pos ).normalizedCopy();
  _right = ( up.cross( _look ) ).normalizedCopy();
  _up = _look.cross( _right );
  _position = pos;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Camera3D::_updateViewMatrix()
{
  // Keep camera's axes orthogonal to each other and of unit length.
  const Vec3 L = _look.normalizedCopy();
  const Vec3 U = ( L.cross( _right ) ).normalizedCopy();

  // U, L already ortho-normal, so no need to normalize cross product.
  const Vec3 R = U.cross( L );

  // Fill in the view matrix entries.
  const real x = -( _position.dot( R ) );
  const real y = -( _position.dot( U ) );
  const real z = -( _position.dot( L ) );

  // V =
  // | u_x    v_x    w_x    0 |
  // | u_y    v_y    w_y    0 |
  // | u_z    v_z    w_z    0 |
  // | -Q*u   -Q*v   -Q*w   1 |

  _view[0][0] = _right.x;
  _view[1][0] = _right.y;
  _view[2][0] = _right.z;
  _view[3][0] = x;

  _view[0][1] = _up.x;
  _view[1][1] = _up.y;
  _view[2][1] = _up.z;
  _view[3][1] = y;

  _view[0][2] = _look.x;
  _view[1][2] = _look.y;
  _view[2][2] = _look.z;
  _view[3][2] = z;

  _view[0][3] = 0.f;
  _view[1][3] = 0.f;
  _view[2][3] = 0.f;
  _view[3][3] = 1.f;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
