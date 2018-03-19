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

#include "GLStockResource.h"

#include <LORE/Core/APIVersion.h>

#include <Plugins/OpenGL/Resource/GLResourceController.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore::OpenGL;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GLStockResource3DFactory::GLStockResource3DFactory( Lore::ResourceControllerPtr controller )
  : StockResourceFactory( controller )
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::GPUProgramPtr GLStockResource3DFactory::createUberProgram( const string& name, const Lore::UberProgramParameters& params )
{
  const bool textured = ( params.numTextures > 0 );
  const bool lit = ( params.maxLights > 0 );
  const bool instanced = params.instanced;
  const string header = "#version " +
    std::to_string( APIVersion::GetMajor() ) + std::to_string( APIVersion::GetMinor() ) + "0" +
    " core\n";

  //
  // Vertex shader.

  string src = header;

  //
  // Layout.

  uint32_t layoutLocation = 0;
  src += "layout (location = " + std::to_string( layoutLocation++ ) + ") in vec3 vertex;";
  src += "layout (location = " + std::to_string( layoutLocation++ ) + ") in vec3 normal;";
  if ( textured ) {
    src += "layout (location = " + std::to_string( layoutLocation++ ) + ") in vec2 texCoord;";
  }
  if ( instanced ) {
    src += "layout (location = " + std::to_string( layoutLocation++ ) + ") in mat4 instanceMatrix;";
  }

  //
  // Structs.

  src += "struct Rect {";
  src += "float x;";
  src += "float y;";
  src += "float w;";
  src += "float h;";
  src += "};";

  //
  // Uniforms and outs.

  src += "uniform mat4 transform;";
  src += "uniform Rect texSampleRegion;";

  if ( textured ) {
    src += "out vec2 TexCoord;";
  }

  if ( lit ) {
    src += "uniform mat4 model;";
    src += "out vec3 FragPos;";
    src += "out vec3 Normal;";
  }

  //
  // main function.

  src += "void main(){";
  {
    if ( lit ) {
      if ( instanced ) {
        src += "FragPos = vec3(instanceMatrix * vec4(vertex, 1.0));";
      }
      else {
        src += "FragPos = vec3(model * vec4(vertex, 1.0));";
      }
      src += "Normal = mat3(transpose(inverse(model))) * normal;";
      //src += "Normal = normal;";
    }

    if ( textured ) {
      src += "TexCoord = vec2(texCoord.x * texSampleRegion.w + texSampleRegion.x, texCoord.y * texSampleRegion.h + texSampleRegion.y);";
    }

    if ( instanced ) {
      src += "gl_Position = transform * instanceMatrix * vec4(vertex, 1.0);";
    }
    else {
      src += "gl_Position = transform * vec4(vertex, 1.0);";
    }
  }
  src += "}";

  // Compile vertex shader.
  auto vsptr = _controller->create<Shader>( name + "_VS" );
  vsptr->init( Shader::Type::Vertex );
  if ( !vsptr->loadFromSource( src ) ) {
    throw Lore::Exception( "Failed to compile uber vertex shader for " + name );
  }

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  //
  // Fragment shader.

  src.clear();
  src = header;

  //
  // Uniforms and ins.

  src += "uniform vec3 viewPos;";

  //if ( textured ) {
    src += "in vec2 TexCoord;";
    src += "uniform vec2 texSampleOffset = vec2(1.0, 1.0);";
  //}

  // Material.
  src += "struct Material {";
  src += "sampler2D diffuseTexture;";
  src += "sampler2D specularTexture;";
  src += "vec4 ambient;";
  src += "vec4 diffuse;";
  src += "vec4 specular;";
  src += "float shininess;";
  src += "};";
  src += "uniform Material material;";

  // Final pixel output color.
  src += "out vec4 pixel;";

  // Lighting.
  if ( lit ) {
    src += "struct Light {";
    src += "vec3 pos;";
    src += "vec3 ambient;";
    src += "vec3 diffuse;";
    src += "vec3 specular;";
    src += "float constant;";
    src += "float linear;";
    src += "float quadratic;";
    src += "float intensity;";
    src += "};";

    src += "uniform Light lights[" + std::to_string( params.maxLights ) + "];";
    src += "uniform int numLights;";

    src += "uniform vec4 sceneAmbient;";

    src += "in vec3 FragPos;";
    src += "in vec3 Normal;";

    //
    // Light functions.

    //
    // Point light.

    src += "vec3 CalcPointLight(Light light, vec3 normal, vec3 viewDir) {";
    {
      src += "vec3 lightDir = normalize(light.pos - FragPos);";

      // Diffuse shading.
      src += "float diff = max(dot(normal, lightDir), 0.0);";

      // Specular shading.
      src += "vec3 reflectDir = reflect(-lightDir, normal);";
      src += "float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);";

      // Attenuation.
      src += "float distance = length(light.pos - FragPos);";
      src += "float attenuation = light.intensity / (light.constant + light.linear * distance + light.quadratic * (distance * distance));";

      // Combine results.
      src += "vec3 ambient = light.ambient * vec3(texture(material.diffuseTexture, TexCoord));";
      src += "vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuseTexture, TexCoord));";
      src += "vec3 specular = light.specular * spec * vec3(texture(material.specularTexture, TexCoord));";
      src += "ambient *= attenuation;";
      src += "diffuse *= attenuation;";
      src += "specular *= attenuation;";
      src += "return (ambient + diffuse + specular);";
    }
    src += "}";
  }

  //
  // main function.

  src += "void main(){";
  {
    src += "vec3 result = vec3(0.0, 0.0, 0.0);";

    if ( lit ) {
      src += "vec3 norm = normalize(Normal);";
      src += "vec3 viewDir = normalize(viewPos - FragPos);";

      // Point lights.
      src += "for(int i = 0; i < numLights; i++) {";
      {
        src += "result += CalcPointLight(lights[i], norm, viewDir);";
      }
      src += "}";
    }
    else {
      src += "result = vec3(1.0, 1.0, 1.0);";
    }

    //src += "result *= (material.ambient.rgb + material.diffuse.rgb);";

    // Final pixel.
    src += "pixel = vec4(result, 1.0);";
  }
  src += "}";

  auto fsptr = _controller->create<Shader>( name + "_FS" );
  fsptr->init( Shader::Type::Fragment );
  if ( !fsptr->loadFromSource( src ) ) {
    throw Lore::Exception( "Failed to compile uber fragment shader for " + name );
  }

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  //
  // GPU program.

  auto program = _controller->create<GPUProgram>( name );
  program->init();
  program->attachShader( vsptr );
  program->attachShader( fsptr );

  if ( !program->link() ) {
    throw Lore::Exception( "Failed to link GPUProgram " + name );
  }

  //
  // Add uniforms.

  program->addTransformVar( "transform" );

  if ( lit ) {
    program->addUniformVar( "model" );
    program->addUniformVar( "material.ambient" );
    program->addUniformVar( "material.diffuse" );
    program->addUniformVar( "material.specular" );
    program->addUniformVar( "material.shininess" );
    program->addUniformVar( "numLights" );
    program->addUniformVar( "sceneAmbient" );
    program->addUniformVar( "viewPos" );
  }

  if ( textured ) {
    program->addUniformVar( "texSampleOffset" );
    program->addUniformVar( "texSampleRegion.x" );
    program->addUniformVar( "texSampleRegion.y" );
    program->addUniformVar( "texSampleRegion.w" );
    program->addUniformVar( "texSampleRegion.h" );
  }

  return program;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::GPUProgramPtr GLStockResource3DFactory::createSkyboxProgram( const string& name, const Lore::SkyboxProgramParameters& params )
{
  const string header = "#version " +
    std::to_string( APIVersion::GetMajor() ) + std::to_string( APIVersion::GetMinor() ) + "0" +
    " core\n";

  //
  // Vertex shader.

  string src = header;

  //
  // Layout.

  src += "layout (location = 0) in vec2 vertex;";
  src += "layout (location = 1) in vec2 texCoord;";

  //
  // Structs.

  src += "struct Rect {";
  src += "float x;";
  src += "float y;";
  src += "float w;";
  src += "float h;";
  src += "};";

  //
  // Uniforms and outs.

  src += "uniform mat4 transform;";
  src += "uniform Rect texSampleRegion;";

  src += "out vec2 TexCoord;";

  //
  // main function.

  src += "void main(){";

  src += "gl_Position = transform * vec4(vertex, transform[3][2], 1.0);";
  src += "TexCoord = vec2(texCoord.x * texSampleRegion.w + texSampleRegion.x, texCoord.y * texSampleRegion.h + texSampleRegion.y);";

  src += "}";

  auto vsptr = _controller->create<Shader>( name + "_VS" );
  vsptr->init( Shader::Type::Vertex );
  if ( !vsptr->loadFromSource( src ) ) {
    throw Lore::Exception( "Failed to compile skybox vertex shader for " + name );
  }

  // ::::::::::::::::::::::::::::::::: //

  //
  // Fragment shader.

  src.clear();
  src = header;

  src += "uniform sampler2D tex;";
  src += "in vec2 TexCoord;";
  src += "out vec4 pixel;";

  // Material.
  src += "struct Material {";
  src += "vec4 diffuse;";
  src += "};";
  src += "uniform Material material;";

  if ( params.scrolling ) {
    src += "uniform vec2 texSampleOffset = vec2(1.0, 1.0);";
  }

  //
  // main function.

  src += "void main(){";

  src += "pixel = texture(tex, TexCoord + texSampleOffset);";
  src += "if ( pixel.a < 0.1 ) {";
  src += "discard;";
  src += "}";

  // Apply alpha blending from material.
  src += "pixel.a *= material.diffuse.a;";

  src += "}";

  auto fsptr = _controller->create<Shader>( name + "_FS" );
  fsptr->init( Shader::Type::Fragment );
  if ( !fsptr->loadFromSource( src ) ) {
    throw Lore::Exception( "Failed to compile skybox fragment shader for " + name );
  }

  // ::::::::::::::::::::::::::::::::: //

  //
  // GPU program.

  auto program = _controller->create<Lore::GPUProgram>( name );
  program->init();
  program->attachShader( vsptr );
  program->attachShader( fsptr );

  if ( !program->link() ) {
    throw Lore::Exception( "Failed to link GPUProgram " + name );
  }

  program->addTransformVar( "transform" );

  program->addUniformVar( "texSampleRegion.x" );
  program->addUniformVar( "texSampleRegion.y" );
  program->addUniformVar( "texSampleRegion.w" );
  program->addUniformVar( "texSampleRegion.h" );

  program->addUniformVar( "material.diffuse" );

  if ( params.scrolling ) {
    program->addUniformVar( "texSampleOffset" );
  }

  return program;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::GPUProgramPtr GLStockResource3DFactory::createBoxProgram( const string& name )
{
  const string header = "#version " +
    std::to_string( APIVersion::GetMajor() ) + std::to_string( APIVersion::GetMinor() ) + "0" +
    " core\n";

  //
  // Vertex shader.

  string src = header;

  //
  // Layout.

  src += "layout (location = 0) in vec2 vertex;";
  src += "layout (location = 1) in vec2 texCoord;";

  //
  // Uniforms and outs.

  src += "uniform mat4 transform;";
  src += "out vec2 TexCoord;";

  //
  // main function.

  src += "void main() {";

  src += "gl_Position = transform * vec4(vertex, 1.0, 1.0);";
  src += "TexCoord = texCoord;";

  src += "}";

  auto vsptr = _controller->create<Shader>( name + "_VS" );
  vsptr->init( Shader::Type::Vertex );
  if ( !vsptr->loadFromSource( src ) ) {
    throw Lore::Exception( "Failed to compile text vertex shader for " + name );
  }

  // ::::::::::::::::::::::::::::::::: //

  //
  // Fragment shader.

  src.clear();
  src = header;

  //
  // Ins/outs and uniforms.

  src += "in vec2 TexCoord;";
  src += "out vec4 pixel;";

  src += "uniform vec4 borderColor;";
  src += "uniform vec4 fillColor;";
  src += "uniform float borderWidth;";
  src += "uniform vec2 scale;";

  //
  // main function.

  src += "void main() {";

  src += "const float maxX = 1.0 - borderWidth / scale.x;";
  src += "const float minX = borderWidth / scale.x;";
  src += "const float maxY = 1.0 - borderWidth / scale.y;";
  src += "const float minY = borderWidth / scale.y;";

  src += "if (TexCoord.x < maxX && TexCoord.x > minX &&"
    "    TexCoord.y < maxY && TexCoord.y > minY) {";
  src += "  pixel = fillColor;";
  src += "} else {";
  src += "  pixel = borderColor;";
  src += "}";

  src += "}";

  auto fsptr = _controller->create<Shader>( name + "_FS" );
  fsptr->init( Shader::Type::Fragment );
  if ( !fsptr->loadFromSource( src ) ) {
    throw Lore::Exception( "Failed to compile text fragment shader for " + name );
    // TODO: Rollback vertex shaders in case of failed fragment shader.
  }

  // ::::::::::::::::::::::::::::::::: //

  //
  // GPU program.

  auto program = _controller->create<Lore::GPUProgram>( name );
  program->init();
  program->attachShader( vsptr );
  program->attachShader( fsptr );

  if ( !program->link() ) {
    throw Lore::Exception( "Failed to link GPUProgram " + name );
  }

  program->addTransformVar( "transform" );
  program->addUniformVar( "borderColor" );
  program->addUniformVar( "fillColor" );
  program->addUniformVar( "borderWidth" );
  program->addUniformVar( "scale" );

  return program;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
