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
#include <LORE/Renderer/Renderer.h>
#include <LORE/Resource/IResource.h>
#include <LORE/Scene/Scene.h>
#include <LORE/Shader/Shader.h>
#include <LORE/Shader/VertexBuffer.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  class LORE_EXPORT GPUProgram : public IResource
  {

  public:

      GPUProgram();
      virtual ~GPUProgram();

      virtual void init() = 0;

      virtual void attachShader( ShaderPtr shader );

      virtual ShaderPtr getAttachedShader( const Shader::Type& type );

      virtual bool link() = 0;

      virtual void use() = 0;

      inline bool hasAttachedShader( const Shader::Type& type )
      {
          return ( _shaders.find( type ) != _shaders.end() );
      }

      //
      // Getters.


      //
      // Uniform value updating.

      virtual void addTransformVar( const string& id ) = 0;

      virtual void setTransformVar( const glm::mat4& m ) = 0;

      virtual void addUniformVar( const string& id ) = 0;

      virtual void setUniformVar( const string& id, const glm::mat4& m ) = 0;

      virtual void setUniformVar( const string& id, const glm::vec2& v ) = 0;

      virtual void setUniformVar( const string& id, const glm::vec3& v ) = 0;

      virtual void setUniformVar( const string& id, const glm::vec4& v ) = 0;

      virtual void setUniformVar( const string& id, const real r ) = 0;

      virtual void setUniformVar( const string& id, const int i ) = 0;

      virtual void updateLights( const RenderQueue::LightList& lights ) = 0;

  protected:

      using ShaderMap = std::unordered_map<Shader::Type, ShaderPtr>;

  protected:

      ShaderMap _shaders;

  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
