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
#include <LORE/Resource/Material.h>
#include <LORE/Resource/Sprite.h>

#include <Plugins/OpenGL/Resource/GLResourceController.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore::OpenGL;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GLStockResource2DFactory::GLStockResource2DFactory( Lore::ResourceControllerPtr controller )
  : StockResourceFactory( controller )
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::GPUProgramPtr GLStockResource2DFactory::createUberProgram( const string& name, const Lore::UberProgramParameters& params )
{
  const bool textured = params.textured;
  const bool lit = !!( params.maxDirectionalLights || params.maxPointLights );
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
  src += "layout (location = " + std::to_string( layoutLocation++ ) + ") in vec2 vertex;";
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
    src += "out vec2 FragPos;";
  }

  //
  // main function.

  src += "void main(){";

  if ( instanced ) {
    src += "gl_Position = transform * instanceMatrix * vec4(vertex, 1.0, 1.0);";
  }
  else {
    src += "gl_Position = transform * vec4(vertex, 1.0, 1.0);";
  }
  if ( textured ) {
    src += "TexCoord = vec2(texCoord.x * texSampleRegion.w + texSampleRegion.x, texCoord.y * texSampleRegion.h + texSampleRegion.y);";
  }

  if ( lit ) {
    if ( instanced ) {
      src += "FragPos = (instanceMatrix * vec4(vertex, 0.0, 1.0)).xy;";
    }
    else {
      src += "FragPos = (model * vec4(vertex, 0.0, 1.0)).xy;";
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

  src += "uniform float gamma;";

  if ( textured ) {
    src += "in vec2 TexCoord;";
    src += "uniform vec2 texSampleOffset = vec2(1.0, 1.0);";

    for ( uint32_t i = 0; i < params.maxDiffuseTextures; ++i ) {
      src += "uniform sampler2D diffuseTexture" + std::to_string( i ) + ";";
    }
    src += "uniform sampler2D specularTexture0;";

    src += "uniform float diffuseMixValues[" + std::to_string( params.maxDiffuseTextures ) + "];";
  }

  // Material.
  src += "struct Material {";
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
    src += "float range;";
    src += "float constant;";
    src += "float linear;";
    src += "float quadratic;";
    src += "float intensity;";
    src += "};";

    src += "uniform Light pointLights[" + std::to_string( params.maxPointLights ) + "];";
    src += "uniform int numPointLights;";

    src += "uniform vec4 sceneAmbient;";

    src += "in vec2 FragPos;";

    //
    // Light functions.

    // Point light.
    src += "vec3 CalcPointLight(Light l) {";

    src += "const float d = length(l.pos.xy - FragPos);";
    src += "const float att = l.range * l.intensity / (l.constant + l.linear * d + l.quadratic * pow(d, 2.0));";

    src += "const vec3 lDiffuse = l.diffuse * material.diffuse.rgb * att;";
    src += "return lDiffuse;";

    src += "}";
  }

  // Texture functions.

  if ( textured ) {
    src += "vec4 SampleDiffuseTextures() {";
    {
      for ( uint32_t i = 0; i < params.maxDiffuseTextures; ++i ) {
        if ( 0 == i ) {
          src += "vec4 diffuse0 = texture(diffuseTexture0, TexCoord + texSampleOffset);";
        }
        else {
          src += "vec4 diffuse" + std::to_string( i ) + " = mix(texture(diffuseTexture" +
            std::to_string( i ) + ", TexCoord + texSampleOffset), diffuse" + std::to_string( i - 1 ) +
            ", diffuseMixValues[" + std::to_string( i ) + "]);";
        }
      }

      src += "return diffuse" + std::to_string( params.maxDiffuseTextures - 1 ) + ";";
    }
    src += "}";

    src += "vec4 SampleSpecularTextures() {";
    {
      src += "return texture(specularTexture0, TexCoord + texSampleOffset);";
    }
    src += "}";
  }

  //
  // main function.

  src += "void main(){";

  // Setup default values for calculating pixel.
  src += "vec4 texSample = vec4(1.0, 1.0, 1.0, 1.0);";

  if ( textured ) {
    src += "texSample = SampleDiffuseTextures();";
    src += "if ( texSample.a < 0.1 ) {";
    src += "  discard;";
    src += "}";
  }

  // Apply alpha blending from material.
  src += "texSample.a *= material.diffuse.a;";

  if ( lit ) {
    src += "vec3 lighting = material.ambient.rgb * sceneAmbient.rgb;";

    src += "for(int i=0; i<numPointLights; ++i){";
    src += "  lighting += CalcPointLight(pointLights[i]);";
    src += "}";

    src += "texSample *= vec4(lighting, 1.0);";
  }

  // Final pixel.
  src += "pixel = texSample;";
  src += "pixel.rgb = pow(texSample.rgb, vec3(1.0 / gamma));";

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

  program->setDiffuseSamplerCount( params.maxDiffuseTextures );
  program->setSpecularSamplerCount( params.maxSpecularTextures );

  if ( !program->link() ) {
    throw Lore::Exception( "Failed to link GPUProgram " + name );
  }

  //
  // Add uniforms.

  program->addTransformVar( "transform" );

  program->addUniformVar( "gamma" );

  if ( lit ) {
    program->addUniformVar( "model" );
    program->addUniformVar( "material.ambient" );
    program->addUniformVar( "material.diffuse" );
    program->addUniformVar( "material.specular" );
    program->addUniformVar( "material.shininess" );
    program->addUniformVar( "numPointLights" );
    program->addUniformVar( "sceneAmbient" );

    for ( uint32_t i = 0; i < params.maxPointLights; ++i ) {
      const string idx( "pointLights[" + std::to_string( i ) + "]" );
      program->addUniformVar( idx + ".pos" );
      program->addUniformVar( idx + ".ambient" );
      program->addUniformVar( idx + ".diffuse" );
      program->addUniformVar( idx + ".specular" );
      program->addUniformVar( idx + ".range" );
      program->addUniformVar( idx + ".constant" );
      program->addUniformVar( idx + ".linear" );
      program->addUniformVar( idx + ".quadratic" );
      program->addUniformVar( idx + ".intensity" );
    }
  }

  if ( textured ) {
    for ( uint32_t i = 0; i < params.maxDiffuseTextures; ++i ) {
      program->addUniformVar( "diffuseTexture" + std::to_string( i ) );
      program->addUniformVar( "diffuseMixValues[" + std::to_string( i ) + "]" );
    }
    program->addUniformVar( "specularTexture0" );

    program->addUniformVar( "texSampleOffset" );
    program->addUniformVar( "texSampleRegion.x" );
    program->addUniformVar( "texSampleRegion.y" );
    program->addUniformVar( "texSampleRegion.w" );
    program->addUniformVar( "texSampleRegion.h" );
  }

  // Setup uniform updaters.

  auto UniformUpdater = []( const RenderView& rv,
                            const GPUProgramPtr program,
                            const MaterialPtr material,
                            const RenderQueue::LightData& lights ) {
    program->setUniformVar( "gamma", rv.gamma );

    if ( material->lighting ) {
      // Update material uniforms.
      program->setUniformVar( "material.ambient", material->ambient );
      program->setUniformVar( "material.diffuse", material->diffuse );
      program->setUniformVar( "material.specular", material->specular );
      program->setUniformVar( "material.shininess", material->shininess );
      program->setUniformVar( "sceneAmbient", rv.scene->getAmbientLightColor() );

      // Update uniforms for light data.
      program->setUniformVar( "numPointLights", static_cast< int >( lights.pointLights.size() ) );

      int i = 0;
      for ( const auto& pair : lights.pointLights ) {
        const string idx( "pointLights[" + std::to_string( i++ ) + "]" );
        const auto pointLight = pair.first;
        const auto pos = pair.second;

        program->setUniformVar( idx + ".pos", glm::vec3( pos ) );
        program->setUniformVar( idx + ".ambient", glm::vec3( pointLight->getAmbient() ) );
        program->setUniformVar( idx + ".diffuse", glm::vec3( pointLight->getDiffuse() ) );
        program->setUniformVar( idx + ".specular", glm::vec3( pointLight->getSpecular() ) );
        program->setUniformVar( idx + ".range", pointLight->getRange() );
        program->setUniformVar( idx + ".constant", pointLight->getConstant() );
        program->setUniformVar( idx + ".linear", pointLight->getLinear() );
        program->setUniformVar( idx + ".quadratic", pointLight->getQuadratic() );
        program->setUniformVar( idx + ".intensity", pointLight->getIntensity() );
      }
    }
  };

  auto UniformNodeUpdater = []( const GPUProgramPtr program,
                                const MaterialPtr material,
                                const NodePtr node,
                                const glm::mat4& viewProjection ) {

    // Update texture data.
    if ( material->sprite ) {
      size_t spriteFrame = 0;
      const auto spc = node->getSpriteController();
      if ( spc ) {
        spriteFrame = spc->getActiveFrame();
      }

      auto sprite = material->sprite;
      int textureUnit = 0;
      const auto diffuseCount = sprite->getTextureCount( spriteFrame, Texture::Type::Diffuse );
      for ( int i = 0; i < diffuseCount; ++i ) {
        auto texture = sprite->getTexture( spriteFrame, Texture::Type::Diffuse, i );
        texture->bind( textureUnit );
        program->setUniformVar( "diffuseTexture" + std::to_string( i ), textureUnit );
        ++textureUnit;
      }
      for ( int i = 0; i < sprite->getTextureCount( spriteFrame, Texture::Type::Specular ); ++i ) {
        auto texture = sprite->getTexture( spriteFrame, Texture::Type::Specular, i );
        texture->bind( textureUnit );
        program->setUniformVar( "specularTexture" + std::to_string( i ), textureUnit );
        ++textureUnit;
      }
      // Set mix values.
      for ( int i = 0; i < static_cast< int >( program->getDiffuseSamplerCount() ); ++i ) {
        program->setUniformVar( "diffuseMixValues[" + std::to_string( i ) + "]",
                                sprite->getMixValue( spriteFrame, Texture::Type::Diffuse, i ) );
      }

      program->setUniformVar( "texSampleOffset", material->getTexCoordOffset() );

      const Rect sampleRegion = material->getTexSampleRegion();
      program->setUniformVar( "texSampleRegion.x", sampleRegion.x );
      program->setUniformVar( "texSampleRegion.y", sampleRegion.y );
      program->setUniformVar( "texSampleRegion.w", sampleRegion.w );
      program->setUniformVar( "texSampleRegion.h", sampleRegion.h );
    }

    // Apply model-view-projection matrix.
    glm::mat4 model = node->getFullTransform();
    model[3][2] = node->getDepth(); // Override depth in 2D.
    const glm::mat4 mvp = viewProjection * model * node->getFlipMatrix();
    program->setTransformVar( mvp );

    // Supply model matrix for lighting calculations.
    if ( material->lighting ) {
      program->setUniformVar( "model", model );
    }
  };

  auto InstancedUniformNodeUpdater = []( const GPUProgramPtr program,
                                         const MaterialPtr material,
                                         const NodePtr node,
                                         const glm::mat4& viewProjection ) {
    // Update texture data.
    if ( material->sprite ) {
      size_t spriteFrame = 0;
      const auto spc = node->getSpriteController();
      if ( spc ) {
        spriteFrame = spc->getActiveFrame();
      }

      auto sprite = material->sprite;
      int textureUnit = 0;
      const auto diffuseCount = sprite->getTextureCount( spriteFrame, Texture::Type::Diffuse );
      for ( int i = 0; i < diffuseCount; ++i ) {
        auto texture = sprite->getTexture( spriteFrame, Texture::Type::Diffuse, i );
        texture->bind( textureUnit );
        program->setUniformVar( "diffuseTexture" + std::to_string( i ), textureUnit );
        ++textureUnit;
      }
      for ( int i = 0; i < sprite->getTextureCount( spriteFrame, Texture::Type::Specular ); ++i ) {
        auto texture = sprite->getTexture( spriteFrame, Texture::Type::Specular, i );
        texture->bind( textureUnit );
        program->setUniformVar( "specularTexture" + std::to_string( i ), textureUnit );
        ++textureUnit;
      }
      // Set mix values.
      for ( int i = 0; i < static_cast< int >( program->getDiffuseSamplerCount() ); ++i ) {
        program->setUniformVar( "diffuseMixValues[" + std::to_string( i ) + "]",
                                sprite->getMixValue( spriteFrame, Texture::Type::Diffuse, i ) );
      }

      program->setUniformVar( "texSampleOffset", material->getTexCoordOffset() );

      const Rect sampleRegion = material->getTexSampleRegion();
      program->setUniformVar( "texSampleRegion.x", sampleRegion.x );
      program->setUniformVar( "texSampleRegion.y", sampleRegion.y );
      program->setUniformVar( "texSampleRegion.w", sampleRegion.w );
      program->setUniformVar( "texSampleRegion.h", sampleRegion.h );
    }

    // Apply model-view-projection matrix.
    glm::mat4 transform = viewProjection;
    // TODO: Depth overriding breaks instanced rendering.
    //transform[3][2] = node->getDepth(); // Override depth in 2D.
    program->setTransformVar( transform * node->getFlipMatrix() );

    // Supply model matrix for lighting calculations.
    if ( material->lighting ) {
      program->setUniformVar( "model", transform );
    }
  };

  program->setUniformUpdater( UniformUpdater );
  if ( instanced ) {
    program->setUniformNodeUpdater( InstancedUniformNodeUpdater );
  }
  else {
    program->setUniformNodeUpdater( UniformNodeUpdater );
  }

  return program;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::GPUProgramPtr GLStockResource2DFactory::createSkyboxProgram( const string& name, const Lore::SkyboxProgramParameters& params )
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

  src += "uniform float gamma;";

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

  // Gamma correction.
  src += "pixel.rgb = pow(pixel.rgb, vec3(1.0 / gamma));";

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

  program->addUniformVar( "gamma" );

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

Lore::GPUProgramPtr GLStockResource2DFactory::createEnvironmentMappingProgram( const string& name, const Lore::EnvironmentMappingProgramParameters& params )
{
  // Nothing in 2D for now.
  return nullptr;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::GPUProgramPtr GLStockResource2DFactory::createPostProcessingProgram( const string& name, const PostProcessingProgramParameters& params )
{
  const string header = "#version " +
    std::to_string( APIVersion::GetMajor() ) + std::to_string( APIVersion::GetMinor() ) + "0" +
    " core\n";

  //
  // Vertex shader.

  string src = header;

  //
  // Outs.

  src += "out vec2 TexCoord;";

  //
  // main function.

  src += "void main() {";
  {
    src += "uint idx = uint(gl_VertexID);";
    src += "gl_Position = vec4( idx & 1U, idx >> 1U, 0.0, 0.5) * 4.0 - 1.0;"; // From https://gist.github.com/mhalber/0a9b8a78182eb62659fc18d23fe5e94e

    src += "TexCoord = vec2(gl_Position.xy * 0.5 + 0.5);";
  }
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

  src += "out vec4 pixel;";
  src += "in vec2 TexCoord;";

  //
  // Uniforms.

  src += "uniform sampler2D frameBuffer;";

  //
  // main function.

  src += "void main() {";
  {
    src += "vec3 hdrColor = texture(frameBuffer, TexCoord).rgb;";
    src += "pixel = vec4(hdrColor, 1.0);";
  }
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

  program->addUniformVar( "buffer" );

  return program;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::GPUProgramPtr GLStockResource2DFactory::createBoxProgram( const string& name )
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
