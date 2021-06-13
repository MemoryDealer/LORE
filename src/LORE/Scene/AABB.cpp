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

#include "AABB.h"

#include <LORE/Resource/Box.h>
#include <LORE/Resource/ResourceController.h>
#include <LORE/Scene/Node.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

AABB::AABB( NodePtr node )
: _node( node )
{
  _box = Lore::Resource::CreateBox( node->_scene->getName() + "." + node->getName() + "_AABB", ResourceController::DefaultGroupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

AABB::~AABB()
{
  Lore::Resource::DestroyBox( _box );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void AABB::update()
{
  const auto x = _node->getWorldPosition().x;
  const auto y = _node->getWorldPosition().y;
  const auto w = _node->getDerivedScale().x * 0.2f;
  const auto h = _node->getDerivedScale().y * 0.2f;

  auto angle = glm::eulerAngles( glm::quat_cast( _node->getFullTransform() ) ).z;
  _dimensions.x = h * std::abs( std::sin( angle ) ) + w * std::abs( std::cos( angle ) );
  _dimensions.y = w * std::abs( std::sin( angle ) ) + h * std::abs( std::cos( angle ) );

  _min.x = x - _dimensions.x / 2.f;
  _min.y = y - _dimensions.y / 2.f;
  _max.x = x + _dimensions.x / 2.f;
  _max.y = y + _dimensions.y / 2.f;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

bool AABB::intersects( const AABB& rhs ) const
{
  return !(
    _max.x < rhs._min.x ||
    _max.y < rhs._min.y ||
    _min.x > rhs._max.x ||
    _min.y > rhs._max.y
    );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
