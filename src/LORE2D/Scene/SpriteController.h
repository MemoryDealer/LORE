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

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  class SpriteAnimationSet;

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  ///
  /// \class SpriteController
  /// \brief Provides functionality to animate sprites for a single node.
  class LORE_EXPORT SpriteController final
  {

  public:

    struct Animation
    {
      using FrameList = std::vector<size_t>;
      using DeltaTimeList = std::vector<long>;

      size_t activeFrameIndex { 0 };
      FrameList frames {};
      DeltaTimeList deltaTimes {};
      std::chrono::high_resolution_clock::time_point lastTimePoint {};
    };

    using AnimationMap = std::map<string, Animation>;

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  public:

    SpriteController() = default;
    ~SpriteController() = default;

    //
    // Modifiers.

    void setActiveFrame( const size_t frame )
    {
      _activeFrame = frame;
    }

    void useAnimationSet( const SpriteAnimationSetPtr set );

    Animation& createAnimation( const string& name );

    void startAnimation( const string& name );

    void stopAnimation( const string& name );

    void setXFlipped( const bool flipped );
    void setYFlipped( const bool flipped );

    //
    // Getters.

    size_t getActiveFrame() const
    {
      return _activeFrame;
    }

    Animation& getAnimation( const string& name );

    bool hasActiveAnimation() const;

    Animation& getActiveAnimation() const;

    bool getXFlipped() const;
    bool getYFlipped() const;

  private:

    size_t _activeFrame { 0 };
    AnimationMap _animations {};
    AnimationMap::iterator _activeAnimation { _animations.end() };
    FrameListenerController::FrameStartedCallback _animationCallback { nullptr };
    bool _xFlipped { false };
    bool _yFlipped { false };

  };

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  class LORE_EXPORT SpriteAnimationSet : public Alloc<SpriteAnimationSet>
  {

    LORE_OBJECT_BODY()

  public:

    SpriteAnimationSet() = default;
    ~SpriteAnimationSet() = default;

    void addAnimation( const string& name, const SpriteController::Animation& animation );

    const SpriteController::AnimationMap& getAnimations() const;

  private:

    virtual void _reset() override { }

  private:

    SpriteController::AnimationMap _animations {};

  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
