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
#include <LORE2D/Resource/IResource.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  class LORE_EXPORT Camera final : public Alloc<Camera>, public IResource // TODO: Should this be a resource?
  {

  public:

    Camera();

    ~Camera() override;

    //
    // Modifiers.

    void setPosition( const Vec2& pos );

    void setPosition( const real x, const real y );

    void translate( const Vec2& offset );

    void translate( const real xOffset, const real yOffset );

    void zoom( const real amount );

    void setZoom( const real amount );

    // TODO: Set projection mode (Ortho/Persp).

    //
    // Getters.

    Vec2 getPosition() const;

    Matrix4 getViewMatrix();

    // Tracking.

    enum class TrackingStyle
    {
      Simple
    };

    void trackNode( NodePtr node, const TrackingStyle& mode = TrackingStyle::Simple );

    void updateTracking(const real aspectRatio);

  private:

    void _dirty();
    void _updateViewMatrix();

  private:

    Vec2 _position {};
    Matrix4 _view {};
    real _zoom { 1.f };

    bool _viewMatrixDirty { true };

    NodePtr _trackingNode { nullptr };
    TrackingStyle _trackingStyle { TrackingStyle::Simple };

  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
