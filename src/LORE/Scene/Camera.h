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

#include <LORE/Math/Math.h>
#include <LORE/Memory/Alloc.h>
#include <LORE/Resource/IResource.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  class LORE_EXPORT Camera
  {

  public:

    enum class Type
    {
      Type2D,
      Type3D
    };

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    Camera() = default;
    virtual ~Camera() = default;

    //
    // Modifiers.

    void setPosition( const Vec2& pos );

    void setPosition( const real x, const real y );

    void setPosition( const real x, const real y, const real z );

    void translate( const Vec2& offset );

    void translate( const real xOffset, const real yOffset );

    void translate( const real xOffset, const real yOffset, const real zOffset );

    void zoom( const real amount );

    void setZoom( const real amount );

    virtual void setPosition( const Vec3& pos ) = 0;

    virtual void translate( const Vec3& offset ) = 0;

    virtual void lookAt( const Vec3& pos, const Vec3& target, const Vec3& up ) = 0;

    //
    // Getters.

    string getName() const;

    Vec3 getPosition() const;

    Matrix4 getViewMatrix();

    // Tracking.

    enum class TrackingStyle
    {
      Simple
    };

    void trackNode( NodePtr node, const TrackingStyle& mode = TrackingStyle::Simple );

    void updateTracking( const real aspectRatio );

  protected:

    friend class Context;

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    void _dirty();
    virtual void _updateViewMatrix() = 0;

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    string _name { };

    Vec3 _position { };
    Matrix4 _view { };
    real _zoom { 1.f };

    bool _viewMatrixDirty { true };

    NodePtr _trackingNode { nullptr };
    TrackingStyle _trackingStyle { TrackingStyle::Simple };

  };

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  class LORE_EXPORT Camera2D final : public Camera
  {

  public:

    Camera2D() = default;
    ~Camera2D() override = default;

    //
    // Modifiers.

    void setPosition( const Vec3& pos ) override;

    void translate( const Vec3& offset ) override;

    void lookAt( const Vec3& pos, const Vec3& target, const Vec3& up ) override { }

  protected:

    void _updateViewMatrix() override;

  };

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  class LORE_EXPORT Camera3D final : public Camera
  {

  public:

    Camera3D() = default;
    ~Camera3D() override = default;

    //
    // Modifiers.

    void setPosition( const Vec3& pos ) override;

    void translate( const Vec3& offset ) override;

    void lookAt( const Vec3& pos, const Vec3& target, const Vec3& up ) override;

  protected:

    void _updateViewMatrix() override;

  private:

    Vec3 _right {};
    Vec3 _up {};
    Vec3 _look {};

  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
