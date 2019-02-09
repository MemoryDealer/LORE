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

GLStockResource3DFactory::GLStockResource3DFactory( Lore::ResourceControllerPtr controller )
  : StockResourceFactory( controller )
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::GPUProgramPtr GLStockResource3DFactory::createUberProgram( const string& name, const Lore::UberProgramParameters& params )
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
  src += "layout (location = " + std::to_string( layoutLocation++ ) + ") in vec3 vertex;";
  src += "layout (location = " + std::to_string( layoutLocation++ ) + ") in vec3 normal;";
  if ( textured ) {
    src += "layout (location = " + std::to_string( layoutLocation++ ) + ") in vec2 texCoord;";
    src += "layout (location = " + std::to_string( layoutLocation++ ) + ") in vec3 tangent;";
    src += "layout (location = " + std::to_string( layoutLocation++ ) + ") in vec3 bitangent;";
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
    src += "out mat3 TBN;";
//     src += "out vec3 TangentLightPos;";
//     src += "out vec3 TangentViewPos;";
//     src += "out vec3 TangentFragPos;";

    if ( lit ) {
      src += "uniform vec3 lightPos;";
      src += "uniform vec3 viewPos;";
    }
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
        src += "mat3 normalMatrix = mat3(instanceMatrix);";
        src += "Normal = normalMatrix * normal;";
      }
      else {
        src += "FragPos = vec3(model * vec4(vertex, 1.0));";
        src += "mat3 normalMatrix = mat3(transpose(inverse(model)));";
        src += "Normal = normalMatrix * normal;";
      }
    }

    if ( textured ) {
      src += "TexCoord = vec2(texCoord.x * texSampleRegion.w + texSampleRegion.x, texCoord.y * texSampleRegion.h + texSampleRegion.y);";

      if ( lit ) {
        src += "vec3 T = normalize(vec3(model * vec4(tangent, 0.0)));";
        src += "vec3 B = normalize(vec3(model * vec4(bitangent, 0.0)));";
        src += "vec3 N = normalize(vec3(model * vec4(normal, 0.0)));";
        src += "TBN = mat3(T, B, N);";

        //src += "vec3 T = normalize(normalMatrix * tangent);";
        //src += "vec3 N = normalize(normalMatrix * normal);";
        //       src += "T = normalize(T - dot(T, N) * N);";
       // src += "vec3 B = cross(N, T);";
        // 
        //       src += "mat3 TBN = transpose(mat3(T, B, N));";
        //       src += "TangentLightPos = TBN * lightPos;";
        //       src += "TangentViewPos = TBN * viewPos;";
        //       src += "TangentFragPos = TBN * FragPos;";
      }
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

  src += "uniform float gamma;";

  if ( textured ) {
    src += "in vec2 TexCoord;";
    src += "uniform vec2 texSampleOffset = vec2(1.0, 1.0);";
  }

  if ( textured ) {
    for ( uint32_t i = 0; i < params.maxDiffuseTextures; ++i ) {
      src += "uniform sampler2D diffuseTexture" + std::to_string( i ) + ";";
    }
    src += "uniform sampler2D specularTexture0;";
    src += "uniform sampler2D normalTexture0;";

    src += "uniform float diffuseMixValues[" + std::to_string( params.maxDiffuseTextures ) + "];";

    src += "uniform float useNormalTextures = 1.0;";
    src += "uniform float useSpecularTextures = 1.0;";
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
    src += "struct DirectionalLight {";
    {
      src += "vec3 direction;";
      src += "vec3 ambient;";
      src += "vec3 diffuse;";
      src += "vec3 specular;";
    }
    src += "};";

    src += "struct PointLight {";
    {
      src += "vec3 pos;";
      src += "vec3 ambient;";
      src += "vec3 diffuse;";
      src += "vec3 specular;";
      src += "float range;";
      src += "float constant;";
      src += "float linear;";
      src += "float quadratic;";
      src += "float intensity;";
    }
    src += "};";

    src += "uniform DirectionalLight dirLights[" + std::to_string( params.maxDirectionalLights ) + "];";
    src += "uniform int numDirLights;";

    src += "uniform PointLight pointLights[" + std::to_string( params.maxPointLights ) + "];";
    src += "uniform int numPointLights;";

    src += "uniform vec4 sceneAmbient;";

    src += "in vec3 FragPos;";
    src += "in vec3 Normal;";

    // Texture functions.

    if ( textured ) {
      src += "in mat3 TBN;";

      src += "vec3 SampleDiffuseTextures() {";
      {
        for ( uint32_t i = 0; i < params.maxDiffuseTextures; ++i ) {
          if ( 0 == i ) {
            src += "vec3 diffuse0 = vec3(texture(diffuseTexture0, TexCoord + texSampleOffset));";
          }
          else {
            src += "vec3 diffuse" + std::to_string( i ) + " = mix(vec3(texture(diffuseTexture" +
              std::to_string( i ) + ", TexCoord + texSampleOffset)), diffuse" + std::to_string( i - 1 ) +
              ", diffuseMixValues[" + std::to_string( i ) + "]);";
          }
        }

        src += "return diffuse" + std::to_string( params.maxDiffuseTextures - 1 ) + ";";
      }
      src += "}";

      src += "vec3 SampleSpecularTextures() {";
      {
        src += "return useSpecularTextures * vec3(texture(specularTexture0, TexCoord + texSampleOffset)) * material.specular.rgb;";
      }
      src += "}";

      src += "vec3 SampleNormalTextures() {";
      {
        src += "return vec3(texture(normalTexture0, TexCoord + texSampleOffset));";
      }
      src += "}";
    }

    //
    // Directional light.

    src += "vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir) {";
    {
      src += "vec3 lightDir = normalize(-light.direction);";

      // Diffuse shading.
      src += "float diff = max(dot(normal, lightDir), 0.0);";

      // Specular shading.
      src += "vec3 reflectDir = reflect(-lightDir, normal);";
      src += "float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);";

      // Combine results.
      if ( textured ) {
        src += "vec3 ambient = light.ambient * SampleDiffuseTextures();";
        src += "vec3 diffuse = light.diffuse * diff * SampleDiffuseTextures();";
        src += "vec3 specular = light.specular * spec * SampleSpecularTextures();";
      }
      else {
        src += "vec3 ambient = light.ambient * vec3(material.ambient);";
        src += "vec3 diffuse = light.diffuse * diff * vec3(material.diffuse);";
        src += "vec3 specular = light.specular * spec * vec3(material.specular);";
      }
      src += "return (ambient + diffuse + specular);";
    }
    src += "}";

    //
    // Point light.

    src += "vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir) {";
    {
      if ( textured ) {
        src += "vec3 lightDir = normalize(light.pos - FragPos);";
      }
      else {
        src += "vec3 lightDir = normalize(light.pos - FragPos);";
      }

      // Diffuse shading.
      src += "float diff = max(dot(normal, lightDir), 0.0);";

      // Specular shading.
      src += "vec3 halfwayVec = normalize(lightDir + viewDir);";
      src += "float spec = pow(max(dot(normal, halfwayVec), 0.0), material.shininess);";
//       src += "vec3 reflectDir = reflect(-lightDir, normal);";
//       src += "float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);";

      // Attenuation.
      if ( textured ) {
        src += "float distance = length(light.pos - FragPos);";
      }
      else {
        src += "float distance = length(light.pos - FragPos);";
      }
      src += "float attenuation = light.range * light.intensity / (light.constant + light.linear * distance + light.quadratic * (distance * distance));";

      // Combine results.
      if ( textured ) {
        src += "vec3 ambient = light.ambient * material.ambient.rgb * SampleDiffuseTextures();";
        src += "vec3 diffuse = light.diffuse * diff * material.diffuse.rgb * SampleDiffuseTextures();";
        src += "vec3 specular = light.specular * spec * material.specular.rgb * SampleSpecularTextures();";
      }
      else {
        src += "vec3 ambient = light.ambient * vec3(material.ambient);";
        src += "vec3 diffuse = light.diffuse * diff * vec3(material.diffuse);";
        src += "vec3 specular = light.specular * spec * vec3(material.specular);";
      }
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
      if ( textured ) {
        //src += "vec3 norm = normalize(SampleNormalTextures() * 2.0 - 1.0) * useNormalTextures;";
        //src += "norm += (normalize(Normal) * (1.0 - useNormalTextures));";

        src += "vec3 norm = normalize(SampleNormalTextures()) * useNormalTextures;";
        src += "norm = normalize(TBN * norm) * useNormalTextures;";
        src += "norm += (normalize(Normal) * (1.0 - useNormalTextures));";

        //src += "vec3 norm = normalize(Normal);"; // This restores normals to working state.
      }
      else {
        src += "vec3 norm = normalize(Normal);";
      }
      src += "vec3 viewDir = normalize(viewPos - FragPos);";

      // Directional lights.
      src += "for(int i = 0; i < numDirLights; ++i) {";
      {
        src += "result += CalcDirectionalLight(dirLights[i], norm, viewDir);";
      }
      src += "}";

      // Point lights.
      src += "for(int i = 0; i < numPointLights; ++i) {";
      {
        src += "result += CalcPointLight(pointLights[i], norm, viewDir);";
      }
      src += "}";
    }
    else {
      src += "result = vec3(1.0, 1.0, 1.0);";
    }

    // Apply scene ambient lighting.
    if ( lit ) {
      if ( textured ) {
        src += "result += sceneAmbient.xyz *  SampleDiffuseTextures();";
      }
      else {
        src += "result += sceneAmbient.xyz * material.ambient.xyz;";
      }
    }

    // Final pixel.
    src += "pixel = vec4(result, material.diffuse.a);";

    // Gamma correction.
    src += "pixel.rgb = pow(pixel.rgb, vec3(1.0 / gamma));";
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
    program->addUniformVar( "numDirLights" );
    program->addUniformVar( "numPointLights" );
    program->addUniformVar( "sceneAmbient" );
    program->addUniformVar( "viewPos" );

    for ( uint32_t i = 0; i < params.maxDirectionalLights; ++i ) {
      const string idx( "dirLights[" + std::to_string( i ) + "]" );
      program->addUniformVar( idx + ".ambient" );
      program->addUniformVar( idx + ".diffuse" );
      program->addUniformVar( idx + ".specular" );
      program->addUniformVar( idx + ".direction" );
    }

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
    program->addUniformVar( "specularTexture0");
    program->addUniformVar( "normalTexture0" );
    program->addUniformVar( "useSpecularTextures" );
    program->addUniformVar( "useNormalTextures" );

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
    if ( material->lighting ) {
      program->setUniformVar( "viewPos", rv.camera->getPosition() );

      program->setUniformVar( "gamma", rv.gamma );

      // Update material uniforms.
      program->setUniformVar( "material.ambient", material->ambient );
      program->setUniformVar( "material.diffuse", material->diffuse );
      program->setUniformVar( "material.specular", material->specular );
      program->setUniformVar( "material.shininess", material->shininess );
      program->setUniformVar( "sceneAmbient", rv.scene->getAmbientLightColor() );

      // Update uniforms for light data.
      program->setUniformVar( "numDirLights", static_cast< int >( lights.directionalLights.size() ) );
      program->setUniformVar( "numPointLights", static_cast< int >( lights.pointLights.size() ) );

      int i = 0;
      for ( const auto& directionalLight : lights.directionalLights ) {
        const string idx( "dirLights[" + std::to_string( i++ ) + "]" );
        program->setUniformVar( idx + ".ambient", glm::vec3( directionalLight->getAmbient() ) );
        program->setUniformVar( idx + ".diffuse", glm::vec3( directionalLight->getDiffuse() ) );
        program->setUniformVar( idx + ".specular", glm::vec3( directionalLight->getSpecular() ) );
        program->setUniformVar( idx + ".direction", glm::vec3( directionalLight->getDirection() ) );
      }

      i = 0;
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

  auto UniformNodeUpdater = [] ( const GPUProgramPtr program,
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

      // Apply texturing with sprite data.
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
      program->setUniformVar( "useNormalTextures", ( sprite->getTextureCount( spriteFrame, Texture::Type::Normal ) ) ? 1.f : 0.f );
      for ( int i = 0; i < sprite->getTextureCount( spriteFrame, Texture::Type::Normal ); ++i ) {
        auto texture = sprite->getTexture( spriteFrame, Texture::Type::Normal, i );
        texture->bind( textureUnit );
        program->setUniformVar( "normalTexture" + std::to_string( i ), textureUnit );
        ++textureUnit;
      }
      // Set mix values.
      for ( int i = 0; i < static_cast<int>( program->getDiffuseSamplerCount() ); ++i ) {
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
    const glm::mat4 model = node->getFullTransform();
    const glm::mat4 mvp = viewProjection * model;
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
      program->setUniformVar( "useNormalTextures", ( sprite->getTextureCount( spriteFrame, Texture::Type::Normal ) ) ? 1.f : 0.f );
      for ( int i = 0; i < sprite->getTextureCount( spriteFrame, Texture::Type::Normal ); ++i ) {
        auto texture = sprite->getTexture( spriteFrame, Texture::Type::Normal, i );
        texture->bind( textureUnit );
        program->setUniformVar( "normalTexture" + std::to_string( i ), textureUnit );
        ++textureUnit;
      }
      // Set mix values.
      for ( int i = 0; i < static_cast<int>( program->getDiffuseSamplerCount() ); ++i ) {
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

    // Apply view-projection matrix for instanced shaders.
    program->setTransformVar( viewProjection );

    // Supply model matrix for lighting calculations.
    if ( material->lighting ) {
      program->setUniformVar( "model", viewProjection );
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

  src += "layout (location = 0) in vec3 pos;";

  //
  // Uniforms and outs.

  src += "out vec3 TexCoords;";

  src += "uniform mat4 viewProjection;";

  //
  // main function.

  src += "void main(){";
  {
    src += "TexCoords = pos;";
    src += "vec4 opPos = viewProjection * vec4(pos, 1.0);";
    src += "gl_Position = opPos.xyww;";
  }
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

  // Ins/outs and uniforms.

  src += "out vec4 pixel;";

  src += "in vec3 TexCoords;";

  src += "uniform samplerCube skybox;";

  //
  // main function.

  src += "void main(){";
  {
    // Sample the texture value from the cubemap.
    src += "pixel = texture(skybox, TexCoords);";
  }
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

  //
  // Add uniforms.

  program->addUniformVar( "viewProjection" );

  // Uniform updaters.

  auto UniformUpdater = []( const RenderView& rv,
                            const GPUProgramPtr program,
                            const MaterialPtr material,
                            const RenderQueue::LightData& lights ) {
    return;
  };

  auto UniformNodeUpdater = []( const GPUProgramPtr program,
                                const MaterialPtr material,
                                const NodePtr node,
                                const glm::mat4& viewProjection ) {
    program->setUniformVar( "viewProjection", viewProjection );
    auto texture = material->sprite->getTexture( 0, Texture::Type::Cubemap );
    texture->bind();
  };

  program->setUniformUpdater( UniformUpdater );
  program->setUniformNodeUpdater( UniformNodeUpdater );

  return program;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::GPUProgramPtr GLStockResource3DFactory::createEnvironmentMappingProgram( const string& name, const Lore::EnvironmentMappingProgramParameters& params )
{
  const string header = "#version " +
    std::to_string( APIVersion::GetMajor() ) + std::to_string( APIVersion::GetMinor() ) + "0" +
    " core\n";

  //
  // Vertex shader.

  string src = header;

  //
  // Layout.

  src += "layout (location = 0) in vec3 pos;";
  src += "layout (location = 1) in vec3 normal;";

  //
  // Uniforms and outs.

  src += "out vec3 Position;";
  src += "out vec3 Normal;";

  src += "uniform mat4 model;";
  src += "uniform mat4 viewProjection;";

  //
  // main function.

  src += "void main(){";
  {
    src += "Normal = mat3(transpose(inverse(model))) * normal;";
    src += "Position = vec3(model * vec4(pos, 1.0));";
    src += "gl_Position = viewProjection * model * vec4(pos, 1.0);";
  }
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

  // Ins/outs and uniforms.

  src += "out vec4 pixel;";

  src += "in vec3 Position;";
  src += "in vec3 Normal;";

  src += "uniform vec3 cameraPos;";
  src += "uniform samplerCube envTexture;";

  //
  // main function.

  src += "void main(){";
  {
    switch ( params.mode ) {
    default:
    case Lore::EnvironmentMappingProgramParameters::Mode::Reflect:
      src += "vec3 I = normalize(Position - cameraPos);";
      src += "vec3 R = reflect(I, normalize(Normal));";
      src += "pixel = vec4(texture(envTexture, R).rgb, 1.0);";
      break;

    case Lore::EnvironmentMappingProgramParameters::Mode::Refract:
      src += "float ratio = 1.0 / 1.52;";
      src += "vec3 I = normalize(Position - cameraPos);";
      src += "vec3 R = refract(I, normalize(Normal), ratio);";
      src += "pixel = vec4(texture(envTexture, R).rgb, 1.0);";
      break;
    }
  }
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

  //
  // Add uniforms.

  program->addUniformVar( "model" );
  program->addUniformVar( "viewProjection" );
  program->addUniformVar( "cameraPos" );

  // Uniform updaters.

  auto UniformUpdater = []( const RenderView& rv,
                            const GPUProgramPtr program,
                            const MaterialPtr material,
                            const RenderQueue::LightData& lights ) {
    program->setUniformVar( "cameraPos", rv.camera->getPosition() );
  };

  auto UniformNodeUpdater = []( const GPUProgramPtr program,
                                const MaterialPtr material,
                                const NodePtr node,
                                const glm::mat4& viewProjection ) {
    auto model = node->getFullTransform();
    program->setUniformVar( "model", model );
    program->setUniformVar( "viewProjection", viewProjection );
    auto texture = material->sprite->getTexture( 0, Texture::Type::Cubemap );
    texture->bind();
  };

  program->setUniformUpdater( UniformUpdater );
  program->setUniformNodeUpdater( UniformNodeUpdater );

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
