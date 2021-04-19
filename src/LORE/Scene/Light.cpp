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

#include "Light.h"

#include <LORE/Config/Config.h>
#include <LORE/Resource/ResourceController.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Light::~Light()
{
  if ( shadowMap ) {
    Resource::DestroyRenderTarget( shadowMap );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void DirectionalLight::init()
{
  if ( GET_VARIANT<bool>( Config::GetValue( "shadows" ) ) ) {
    shadowMap = Resource::CreateDepthShadowMap( _name + "_shadowmap", 2048, 2048, 0 ); // TODO: Customize res.
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void PointLight::init()
{
  _type = Type::Point;

  if ( GET_VARIANT<bool>( Config::GetValue( "shadows" ) ) ) {
    shadowMap = Resource::CreateDepthShadowCubemap( _name + "_shadowmap", 2048, 2048 ); // TODO: Customize res.

    shadowTransforms.resize( 6 );

    const float aspect = static_cast<float> ( 2048 ) / 2048; // TODO: Use custom res;
    const float zNear = 1.0f;
    _shadowProj = glm::perspective( glm::radians( 90.0f ), aspect, zNear, shadowFarPlane );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void PointLight::updateShadowTransforms( const glm::vec3& pos )
{
  shadowTransforms[0] = _shadowProj * glm::lookAt( pos, pos + Vec3PosX, Vec3NegY );
  shadowTransforms[1] = _shadowProj * glm::lookAt( pos, pos + Vec3NegX, Vec3NegY );
  shadowTransforms[2] = _shadowProj * glm::lookAt( pos, pos + Vec3PosY, Vec3PosZ );
  shadowTransforms[3] = _shadowProj * glm::lookAt( pos, pos + Vec3NegY, Vec3NegZ );
  shadowTransforms[4] = _shadowProj * glm::lookAt( pos, pos + Vec3PosZ, Vec3NegY );
  shadowTransforms[5] = _shadowProj * glm::lookAt( pos, pos + Vec3NegZ, Vec3NegY );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
