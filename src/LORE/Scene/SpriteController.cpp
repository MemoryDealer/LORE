// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// The MIT License (MIT)
// This source file is part of LORE
// ( Lightweight Object-oriented Rendering Engine )
//
// Copyright (c) 2017-2021 Jordan Sparks
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

#include "SpriteController.h"

#include <LORE/Core/Context.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void SpriteController::useAnimationSet( const SpriteAnimationSetPtr set )
{
  _animations.clear();
  _animations = set->getAnimations();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Animation& SpriteController::createAnimation( const string& name )
{
  auto lookup = _animations.find( name );
  if ( _animations.end() != lookup ) {
    throw Lore::Exception( "Animation " + name + " already exists" );
  }

  Animation animation;
  auto result = _animations.insert( { name, animation } );
  return result.first->second;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Animation& SpriteController::getAnimation( const string& name )
{
  auto lookup = _animations.find( name );
  if ( _animations.end() == lookup ) {
    throw Lore::Exception( "Animation " + name + " does not exist" );
  }

  return lookup->second;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

bool SpriteController::hasActiveAnimation() const
{
  return ( _animations.end() != _activeAnimation );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Animation& SpriteController::getActiveAnimation() const
{
  if ( !hasActiveAnimation() ) {
    throw Lore::Exception( "There is no active animation" );
  }

  return _activeAnimation->second;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

bool SpriteController::getXFlipped() const
{
  return _xFlipped;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

bool SpriteController::getYFlipped() const
{
  return _yFlipped;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void SpriteController::startAnimation( const string& name )
{
  _activeAnimation = _animations.find( name );

  if ( hasActiveAnimation() ) {
    // Reset active frame to beginning.
    _activeFrame = 0;

    // Setup a callback to process animation.
    if ( !_animationCallback ) {
      _activeAnimation->second.lastTimePoint = std::chrono::high_resolution_clock::now();

      // Callback function to update animation.
      _animationCallback = [this] ( const FrameEvent& ) {
        Animation& animation = _activeAnimation->second;

        // Calculate delta time since last animation tick.
        const auto now = std::chrono::high_resolution_clock::now();
        const auto dt = std::chrono::duration_cast< std::chrono::milliseconds >( now - animation.lastTimePoint ).count();

        // Advance active frame to the next if delta time surpasses active frame delta time value.
        if ( dt > (  + animation.deltaTimes[animation.activeFrameIndex] ) ) {
          // Increment index in the list of animation frames.
          const auto size = animation.frames.size();
          animation.activeFrameIndex = ( ( size - 1 ) == animation.activeFrameIndex ) ? 0 : animation.activeFrameIndex + 1;

          // Assign the active frame to the VALUE at the animation active frame index.
          _activeFrame = animation.frames[animation.activeFrameIndex];

          // Assign new time point to now.
          animation.lastTimePoint = now;
        }
      };

      Context::RegisterFrameStartedCallback( _animationCallback );
    }
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void SpriteController::stopAnimation( const string& name )
{
  _activeAnimation = _animations.end();
  Context::UnregisterFrameStartedCallback( _animationCallback );
  _animationCallback = nullptr;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void SpriteController::setXFlipped( const bool flipped )
{
  _xFlipped = flipped;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void SpriteController::setYFlipped( const bool flipped )
{
  _yFlipped = flipped;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void SpriteAnimationSet::addAnimation( const string& name, const Animation& animation )
{
  auto lookup = _animations.find( name );
  if ( _animations.end() != lookup ) {
    throw Lore::Exception( "Animation with name " + name + " already exists" );
  }

  _animations[name] = animation;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

const AnimationMap& SpriteAnimationSet::getAnimations() const
{
  return _animations;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
