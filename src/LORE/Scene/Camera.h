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

    struct PostProcessing
    {
      RenderTargetPtr renderTarget {};
      RenderTargetPtr doubleBuffer {}; // For Gaussian blur.
      PrefabPtr prefab {};
      PrefabPtr doubleBufferPrefab {};
      float exposure { 0.5f };
      float bloomThreshold { 10.0f };
      float gamma { 2.2f };
    };

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    Camera() = default;
    virtual ~Camera();

    //
    // Modifiers.

    void setPosition( const glm::vec2& pos );

    void setPosition( const real x, const real y );

    void setPosition( const real x, const real y, const real z );

    void translate( const glm::vec2& offset );

    void translate( const real xOffset, const real yOffset );

    void translate( const real xOffset, const real yOffset, const real zOffset );

    void zoom( const real amount );

    void setZoom( const real amount );

    virtual void setPosition( const glm::vec3& pos ) = 0;

    virtual void translate( const glm::vec3& offset ) = 0;

    virtual void pitch( const real amount ) = 0;

    virtual void yaw( const real amount ) = 0;

    virtual void lookAt( const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up ) = 0;

    void initPostProcessing( const u32 width, const u32 height, const u32 sampleCount );

    //
    // Getters.

    string getName() const;

    glm::vec3 getPosition() const;

    glm::vec3 getTarget() const;

    glm::mat4 getViewMatrix();

    // Tracking.

    enum class TrackingStyle
    {
      Simple
    };

    void trackNode( NodePtr node, const TrackingStyle& mode = TrackingStyle::Simple );

    void updateTracking();

  ///

    friend class Context;

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    void _dirty();
    virtual void _updateViewMatrix() = 0;

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    string _name { };

    glm::vec3 _position { 0.f };
    glm::vec3 _target { 0.f };
    glm::mat4 _view { 0.f };
    real _zoom { 1.f };

    bool _viewMatrixDirty { true };

    NodePtr _trackingNode { nullptr };
    TrackingStyle _trackingStyle { TrackingStyle::Simple };

    std::unique_ptr<PostProcessing> postProcessing {};

  };

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  class LORE_EXPORT Camera2D final : public Camera
  {

  public:

    Camera2D() = default;
    ~Camera2D() override = default;

    //
    // Modifiers.

    void setPosition( const glm::vec3& pos ) override;

    void translate( const glm::vec3& offset ) override;

    void pitch( const real amount ) override { }

    void yaw( const real amount ) override { }

    void lookAt( const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up ) override { }

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

    void setPosition( const glm::vec3& pos ) override;

    void translate( const glm::vec3& offset ) override;

    void pitch( const real amount ) override;

    void yaw( const real amount ) override;

    void lookAt( const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up ) override;

  protected:

    void _updateViewMatrix() override;

  private:

    glm::vec3 _up {};

    real _pitch { 0.f };
    real _yaw { -90.f };

  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
