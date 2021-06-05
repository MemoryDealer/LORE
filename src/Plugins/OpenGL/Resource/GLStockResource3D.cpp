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

#include <LORE/Config/Config.h>
#include <LORE/Core/APIVersion.h>
#include <LORE/Resource/Material.h>
#include <LORE/Resource/Sprite.h>

#include <Plugins/OpenGL/Resource/GLResourceController.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore::OpenGL;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

constexpr int instancedMatrixTexUnit = 5;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GLStockResource3DFactory::GLStockResource3DFactory( Lore::ResourceControllerPtr controller )
  : StockResourceFactory( controller )
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::GPUProgramPtr GLStockResource3DFactory::createUberProgram( const string& name, const Lore::UberProgramParameters& params )
{
  const bool textured = params.textured;
  const bool normalMapping = ( params.maxNormalTextures > 0 );
  const bool lit = !!( params.maxDirectionalLights || params.maxPointLights );
  const bool instanced = params.instanced;
  const bool shadows = params.shadows;
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

    if ( lit && normalMapping ) { // We need the light data in the vertex shader also for normal mapping.
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
        if ( shadows ) {
          src += "float shadowFarPlane;";
        }
      }
      src += "};";

      src += "uniform PointLight pointLights[" + std::to_string( params.maxPointLights ) + "];";
      src += "uniform vec3 viewPos;";
      src += "uniform int numPointLights;";

      src += "uniform DirectionalLight dirLights[" + std::to_string( params.maxDirectionalLights ) + "];";
      src += "uniform int numDirLights;";

      src += "out vec3 tangentLightPos[" + std::to_string( params.maxPointLights ) + "];";
      src += "out vec3 tangentDirLightDirection[" + std::to_string( params.maxDirectionalLights ) + "];";
      src += "out vec3 tangentViewPos;";
      src += "out vec3 tangentFragPos;";
    }
  }

  if ( lit ) {
    src += "uniform mat4 model;";
    src += "out vec3 FragPos;";
    src += "out vec3 Normal;";

    if ( shadows ) {
      src += "out vec4 FragPosDirLightSpace[" + std::to_string( params.maxDirectionalLights ) + "];";
      src += "uniform mat4 dirLightSpaceMatrix[" + std::to_string( params.maxDirectionalLights ) + "];";

      if ( !normalMapping ) { // Already added for normal mapping.
        src += "uniform int numDirLights;";
      }
    }
  }

  //
  // main function.

  src += "void main(){";
  {
    if ( lit ) {
      if ( instanced ) {
        src += "FragPos = vec3(instanceMatrix * vec4(vertex, 1.0));";
        src += "Normal = mat3(instanceMatrix) * normal;";
      }
      else {
        src += "FragPos = vec3(model * vec4(vertex, 1.0));";
        src += "Normal = mat3(transpose(inverse(model))) * normal;";
      }

      if ( shadows ) {
        src += "for (int i = 0; i < numDirLights; ++i) {";
        {
          src += "FragPosDirLightSpace[i] = dirLightSpaceMatrix[i] * vec4(FragPos, 1.0);";
        }
        src += "}";
      }
    }

    if ( textured ) {
      src += "TexCoord = vec2(texCoord.x * texSampleRegion.w + texSampleRegion.x, texCoord.y * texSampleRegion.h + texSampleRegion.y);";

      if ( lit && normalMapping ) {
        if ( instanced ) {
          src += "mat3 normalMat = transpose(inverse(mat3(instanceMatrix)));";
        }
        else {
          src += "mat3 normalMat = transpose(inverse(mat3(model)));";
        }

        src += "vec3 T = normalize(normalMat * tangent);";
        src += "vec3 N = normalize(normalMat * normal);";
        src += "T = normalize(T - dot(T, N) * N);";

        src += "vec3 B = cross(N, T);";

        /*src += "if (dot(cross(N, T), B) < 0.0) {";
        {
          src += "T = T * -1.0;";
        }
        src += "}";*/

        src += "mat3 TBN = transpose(mat3(T, B, N));";

        src += "for (int i = 0; i < numPointLights; ++i) {";
        {
          src += "tangentLightPos[i] = TBN * pointLights[i].pos;";
        }
        src += "}";

        src += "for (int i = 0; i < numDirLights; ++i) {";
        {
          src += "tangentDirLightDirection[i] = TBN * -dirLights[i].direction;";
        }
        src += "}";

        src += "tangentViewPos = TBN * viewPos;";
        src += "tangentFragPos = TBN * FragPos;";
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
    src += "uniform vec2 texSampleOffset = vec2(0.0, 0.0);";
    src += "uniform vec2 uvScale = vec2(0.0, 0.0);";
  }

  if ( textured ) {
    for ( u8 i = 0; i < params.maxDiffuseTextures; ++i ) {
      src += "uniform sampler2D diffuseTexture" + std::to_string( i ) + ";";
    }
    src += "uniform sampler2D specularTexture0;";

    src += "uniform float diffuseMixValues[" + std::to_string( params.maxDiffuseTextures ) + "];";

    if ( lit && normalMapping ) {
      src += "uniform int numNormalTextures;";

      for ( u8 i = 0; i < params.maxNormalTextures; ++i ) {
        src += "uniform sampler2D normalTexture" + std::to_string( i ) + ";";
      }

      src += "in vec3 tangentLightPos[" + std::to_string( params.maxPointLights ) + "];";
      src += "in vec3 tangentDirLightDirection[" + std::to_string( params.maxDirectionalLights ) + "];";
      src += "in vec3 tangentViewPos;";
      src += "in vec3 tangentFragPos;";
    }
  }

  src += "uniform float bloomThreshold;";

  // Material.
  src += "struct Material {";
  {
    src += "vec4 ambient;";
    src += "vec4 diffuse;";
    src += "vec4 specular;";
    src += "float shininess;";
    src += "float opacity;";
  }
  src += "};";
  src += "uniform Material material;";

  // Final pixel output color.
  src += "layout (location = 0) out vec4 pixel;";
  src += "layout (location = 1) out vec4 brightPixel;";

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
      if ( shadows ) {
        src += "float shadowFarPlane;";
      }
    }
    src += "};";

    src += "uniform DirectionalLight dirLights[" + std::to_string( params.maxDirectionalLights ) + "];";
    if ( shadows ) {
      src += "in vec4 FragPosDirLightSpace[" + std::to_string( params.maxDirectionalLights ) + "];";
      src += "uniform sampler2D dirLightShadowMap[" + std::to_string( params.maxDirectionalLights ) + "];";

      src += "uniform float omniBias = 0.05f;";
    }
    src += "uniform int numDirLights;";

    src += "uniform PointLight pointLights[" + std::to_string( params.maxPointLights ) + "];";
    src += "uniform int numPointLights;";
    if ( shadows ) {
      src += "uniform samplerCube shadowCubemap[" + std::to_string( params.maxPointLights ) + "];";
    }

    src += "uniform vec4 sceneAmbient;";

    src += "in vec3 FragPos;";
    src += "in vec3 Normal;";

    // Shadows.

    if ( shadows ) {
      src += "float CalcDirShadows(vec4 fragPosDirLightSpace, sampler2D shadowMap, float dotNormalDir) {";
      {
        // We must do the perspective divide manually (only needed for projection matrices though).
        src += "vec3 projCoords = fragPosDirLightSpace.xyz / fragPosDirLightSpace.w;";
        // Convert NDC coordinates to [0,1] range.
        src += "projCoords = projCoords * 0.5 + 0.5;";

        // Prevent over-sampling the depth shadow map.
        src += "if (projCoords.z > 1.0) {";
        {
          src += "return 0.0;";
        }
        src += "}";

        // Closest depth is from shadow map, while current depth is known from the fragment.
        src += "float currentDepth = projCoords.z;";

        src += "float bias = 0.001;"; // TODO: Make shadow bias customizable per light.
        //src += "float bias = max(0.05 * (1.0 - dotNormalDir), 0.005);";

        // Soft shadows with PCF.
        src += "float shadow = 0.0;";
        src += "vec2 texelSize = 1.0 / textureSize(shadowMap, 0);";
        src += "const int halfKernelWidth = 1;";
        src += "for (int x = -halfKernelWidth; x <= halfKernelWidth; ++x) {";
        {
          src += "for (int y = -halfKernelWidth; y <= halfKernelWidth; ++y) {";
          {
            src += "float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;";
            src += "shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;";
          }
          src += "}";
        }
        src += "}";
        src += "shadow /= ((halfKernelWidth * 2 + 1) * (halfKernelWidth * 2 + 1));";

        src += "return shadow;";
      }
      src += "}";

      //src += "vec3 sampleOffsetDirections[20] = vec3[]\
      //  (\
      //    vec3( 1, 1, 1 ), vec3( 1, -1, 1 ), vec3( -1, -1, 1 ), vec3( -1, 1, 1 ),\
      //    vec3( 1, 1, -1 ), vec3( 1, -1, -1 ), vec3( -1, -1, -1 ), vec3( -1, 1, -1 ),\
      //    vec3( 1, 1, 0 ), vec3( 1, -1, 0 ), vec3( -1, -1, 0 ), vec3( -1, 1, 0 ),\
      //    vec3( 1, 0, 1 ), vec3( -1, 0, 1 ), vec3( 1, 0, -1 ), vec3( -1, 0, -1 ),\
      //    vec3( 0, 1, 1 ), vec3( 0, -1, 1 ), vec3( 0, -1, -1 ), vec3( 0, 1, -1 )\
      //    );";

      src += "float CalcPointShadows(vec3 fragPos, PointLight light, samplerCube shadowCubemap) {";
      {
        src += "vec3 fragToLight = (fragPos - light.pos);";

        // Sample shadow cubemap with this vector.
        src += "float closestDepth = texture(shadowCubemap, fragToLight).r;";
        src += "closestDepth *= light.shadowFarPlane;"; // Re-map back to original space.

        src += "float currentDepth = length(fragToLight);";
        src += "float shadow = (currentDepth - omniBias) > closestDepth ? 1.0 : 0.0;";

        //src += "float shadow = 0.0;";
        //src += "int samples = 20;";
        //src += "float bias = 0.15;";
        //src += "float viewDist = length(viewPos - fragPos);";
        //src += "float diskRadius = (1.0 + (viewDist / light.shadowFarPlane)) / light.shadowFarPlane;";
        //
        //src += "for (int i = 0; i < samples; ++i) {";
        //{
        //  src += "float closestDepth = texture(shadowCubemap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;";
        //  src += "closestDepth *= light.shadowFarPlane;";
        //  src += "if (currentDepth - bias > closestDepth) {";
        //  {
        //    src += "shadow += 1.0;";
        //  }
        //  src += "}";
        //}
        //src += "}";

        //src += "shadow /= float(samples);";

        src += "return shadow;";
      }
      src += "}";
    }

    // Texture functions.

    if ( textured ) {
      src += "vec3 SampleDiffuseTextures() {";
      {
        for ( uint32_t i = 0; i < params.maxDiffuseTextures; ++i ) {
          if ( 0 == i ) {
            src += "vec3 diffuse0 = vec3(texture(diffuseTexture0, TexCoord * uvScale + texSampleOffset));";
          }
          else {
            src += "vec3 diffuse" + std::to_string( i ) + " = mix(vec3(texture(diffuseTexture" +
              std::to_string( i ) + ", TexCoord * uvScale + texSampleOffset)), diffuse" + std::to_string( i - 1 ) +
              ", diffuseMixValues[" + std::to_string( i ) + "]);";
          }
        }

        src += "return diffuse" + std::to_string( params.maxDiffuseTextures - 1 ) + ";";
      }
      src += "}";

      src += "vec3 SampleSpecularTextures() {";
      {
        src += "return vec3(texture(specularTexture0, TexCoord * uvScale + texSampleOffset));";
      }
      src += "}";

      if ( lit && normalMapping ) {
        src += "vec3 SampleNormalTextures() {";
        {
          src += "vec3 normal;";

          // TODO: Multitexturing...
          src += "normal = texture(normalTexture0, TexCoord * uvScale + texSampleOffset).rgb;";
          src += "normal = normalize(normal * 2.0 - 1.0);";

          src += "return normal;";
        }
        src += "}";
      }
    }

    //
    // Directional light.

    src += "vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir, int idx) {";
    {
      if ( textured && normalMapping ) {
        src += "vec3 lightDir = normalize(tangentDirLightDirection[idx]);";
      }
      else {
        src += "vec3 lightDir = normalize(-light.direction);";
      }

      // Diffuse shading.
      src += "float diff = max(dot(normal, lightDir), 0.0);";

      // Specular shading.
      src += "vec3 reflectDir = reflect(-lightDir, normal);";
      src += "float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);";

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
      src += "vec3 result = (ambient + diffuse + specular);";

      // Shadows.
      //src += "float dotNormalDir = dot(normal, lightDir);";
      src += "float dotNormalDir = 1.0;";
      src += "float shadow = CalcDirShadows(FragPosDirLightSpace[idx], dirLightShadowMap[idx], dotNormalDir);";
      src += "result *= (1.0 - shadow);";
      src += "return result;";
    }
    src += "}";

    //
    // Point light.

    src += "vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir, samplerCube shadowCubemap, int i) {";
    {
      if ( textured && normalMapping ) {
        src += "vec3 lightDir = normalize(tangentLightPos[i] - tangentFragPos);";
      }
      else {
        src += "vec3 lightDir = normalize(light.pos - FragPos);";
      }

      // Diffuse shading.
      src += "float diff = max(dot(normal, lightDir), 0.0);";

      // Specular shading.
      src += "vec3 halfwayVec = normalize(lightDir + viewDir);";
      src += "float spec = pow(max(dot(normal, halfwayVec), 0.0), material.shininess);";

      // Attenuation.
      src += "float distance = length(light.pos - FragPos);";
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

      // Shadows and final combine.
      src += "float shadow = CalcPointShadows(FragPos, light, shadowCubemap);";
      src += "vec3 result = (ambient + diffuse + specular) * (1.0 - shadow);";
      src += "return result;";
    }
    src += "}";
  }

  //
  // main function.

  src += "void main(){";
  {
    if ( textured ) {
      src += "vec4 diffuse = texture(diffuseTexture0, TexCoord * uvScale + texSampleOffset);";
      src += "if (diffuse.a < 0.1) {";
      {
        src += "discard;";
      }
      src += "}";
    }

    src += "vec3 result = vec3(0.0, 0.0, 0.0);";

    if ( lit ) {
      if ( textured && normalMapping ) {
        src += "vec3 norm = SampleNormalTextures();";
        src += "vec3 viewDir = normalize(tangentViewPos - tangentFragPos);";
      }
      else {
        src += "vec3 norm = normalize(Normal);";
        src += "vec3 viewDir = normalize(viewPos - FragPos);";
      }

      // Directional lights.
      src += "for(int i = 0; i < numDirLights; ++i) {";
      {
        src += "result += CalcDirectionalLight(dirLights[i], norm, viewDir, i);";
      }
      src += "}";

      // Point lights.
      src += "for(int i = 0; i < numPointLights; ++i) {";
      {
        src += "result += CalcPointLight(pointLights[i], norm, viewDir, shadowCubemap[i], i);";
      }
      src += "}";
    }
    else {
      src += "result = vec3(1.0, 1.0, 1.0);";
    }

    // Apply scene ambient lighting.
    if ( lit ) {
      if ( textured ) {
        src += "result += sceneAmbient.rgb *  SampleDiffuseTextures();";
      }
      else {
        src += "result += sceneAmbient.rgb * material.ambient.rgb;";
      }
    }

    // Final pixel.
    src += "pixel = vec4(result, material.opacity);";

    // Bright pixel pass for bloom.
    src += "vec3 lumConst = vec3(0.2126, 0.7152, 0.0722);";
    src += "float brightness = dot(result, lumConst);";
    src += "if (brightness > bloomThreshold) {";
    {
      src += "brightPixel = vec4(pixel.rgb, 1.0);";
    }
    src += "}";
    src += "else {";
    {
      src += "brightPixel = vec4(0.0, 0.0, 0.0, 1.0);";
    }
    src += "}";

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
  program->addUniformVar( "bloomThreshold" );

  program->addUniformVar( "material.ambient" );
  program->addUniformVar( "material.diffuse" );
  program->addUniformVar( "material.specular" );
  program->addUniformVar( "material.shininess" );
  program->addUniformVar( "material.opacity" );

  if ( lit ) {
    program->addUniformVar( "model" );
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

      if ( shadows ) {
        program->addUniformVar( "dirLightSpaceMatrix[" + std::to_string( i ) + "]" );
        program->addUniformVar( "dirLightShadowMap[" + std::to_string( i ) + "]" );

        program->addUniformVar( "omniBias" );
      }
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

      if ( shadows ) {
        program->addUniformVar( idx + ".shadowFarPlane" );
        program->addUniformVar( "shadowCubemap[" + std::to_string( i ) + "]" );
      }
    }

    program->maxPointLights = params.maxPointLights;
  }

  if ( textured ) {
    for ( uint32_t i = 0; i < params.maxDiffuseTextures; ++i ) {
      program->addUniformVar( "diffuseTexture" + std::to_string( i ) );
      program->addUniformVar( "diffuseMixValues[" + std::to_string( i ) + "]" );

      if ( lit && normalMapping ) {
        program->addUniformVar( "normalTexture" + std::to_string( i ) );
      }
    }
    program->addUniformVar( "specularTexture0");

    program->addUniformVar( "texSampleOffset" );
    program->addUniformVar( "uvScale" );
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
#ifdef LORE_DEBUG_UI
      program->setUniformVar( "bloomThreshold", DebugConfig::bloomEnabled ? DebugConfig::bloomThreshold : 9999999.0f );
#else
      program->setUniformVar( "bloomThreshold", rv.camera->postProcessing->bloomThreshold );
#endif

      // Update material uniforms.
      program->setUniformVar( "material.ambient", material->ambient );
      program->setUniformVar( "material.diffuse", material->diffuse );
      program->setUniformVar( "material.specular", material->specular );
      program->setUniformVar( "material.shininess", material->shininess );
      program->setUniformVar( "material.opacity", material->opacity );
      program->setUniformVar( "sceneAmbient", rv.scene->getAmbientLightColor() );

      // Update uniforms for light data.
      program->setUniformVar( "numDirLights", static_cast< int >( lights.directionalLights.size() ) );
      program->setUniformVar( "numPointLights", static_cast< int >( lights.pointLights.size() ) );

      int shadowMapTexUnit = 10;
      u8 i = 0;
      for ( const auto& directionalLight : lights.directionalLights ) {
        const string idx( "dirLights[" + std::to_string( i ) + "]" );
        program->setUniformVar( idx + ".ambient", glm::vec3( directionalLight->getAmbient() ) );
        program->setUniformVar( idx + ".diffuse", glm::vec3( directionalLight->getDiffuse() ) );
        program->setUniformVar( idx + ".specular", glm::vec3( directionalLight->getSpecular() ) );
        program->setUniformVar( idx + ".direction", glm::vec3( directionalLight->getDirection() ) );

        if ( directionalLight->shadowMap ) {
          directionalLight->shadowMap->getTexture()->bind( shadowMapTexUnit );
          program->setUniformVar( "dirLightShadowMap[" + std::to_string( i ) + "]", shadowMapTexUnit );
          program->setUniformVar( "dirLightSpaceMatrix[" + std::to_string( i ) + "]", directionalLight->viewProj );

          ++shadowMapTexUnit;
        }

        ++i;
      }

      shadowMapTexUnit = 12; // TODO: Config shadow map tex units?
      i = 0;
      for ( const auto& pair : lights.pointLights ) {
        const string idx( "pointLights[" + std::to_string( i ) + "]" );
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

        if ( pointLight->shadowMap ) {
          program->setUniformVar( idx + ".shadowFarPlane", pointLight->shadowFarPlane );

          pointLight->shadowMap->getTexture()->bind( shadowMapTexUnit );
          program->setUniformVar( "shadowCubemap[" + std::to_string( i ) + "]", shadowMapTexUnit );

          ++shadowMapTexUnit;

#ifdef LORE_DEBUG_UI
          program->setUniformVar( "omniBias", DebugConfig::omniBias );
#endif
        }

        ++i;
      }

      // Assign the last shadow map to any unused shadow maps so they don't get filled with the skybox cubemap.
      --shadowMapTexUnit;
      for ( u8 j = i; j < program->maxPointLights; ++j ) {
        program->setUniformVar( "shadowCubemap[" + std::to_string( j ) + "]", shadowMapTexUnit );
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

      auto sprite = material->sprite;
      int textureUnit = 0;
      const auto diffuseCount = sprite->getTextureCount( spriteFrame, Texture::Type::Diffuse );
      for ( u8 i = 0; i < diffuseCount; ++i ) {
        auto texture = sprite->getTexture( spriteFrame, Texture::Type::Diffuse, i );
        texture->bind( textureUnit );
        program->setUniformVar( "diffuseTexture" + std::to_string( i ), textureUnit );
        ++textureUnit;
      }
      for ( u8 i = 0; i < sprite->getTextureCount( spriteFrame, Texture::Type::Specular ); ++i ) {
        auto texture = sprite->getTexture( spriteFrame, Texture::Type::Specular, i );
        texture->bind( textureUnit );
        program->setUniformVar( "specularTexture" + std::to_string( i ), textureUnit );
        ++textureUnit;
      }
      for ( u8 i = 0; i < sprite->getTextureCount( spriteFrame, Texture::Type::Normal ); ++i ) {
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
      program->setUniformVar( "uvScale", material->uvScale );

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
      for ( u8 i = 0; i < sprite->getTextureCount( spriteFrame, Texture::Type::Normal ); ++i ) {
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
      program->setUniformVar( "uvScale", material->uvScale );

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

Lore::GPUProgramPtr GLStockResource3DFactory::createShadowProgram( const string& name, const bool instanced )
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

  if ( instanced ) {
    src += "layout (location = " + std::to_string( instancedMatrixTexUnit ) + ") in mat4 instanceMatrix;";
  }

  //
  // Uniforms and outs.

  src += "uniform mat4 viewProjection;";
  src += "uniform mat4 model;";

  //
  // main function.

  src += "void main(){";
  {
    if ( instanced ) {
      src += "gl_Position = viewProjection * instanceMatrix * vec4(pos, 1.0);";
    }
    else {
      src += "gl_Position = viewProjection * model * vec4(pos, 1.0);";
    }
  }
  src += "}";

  auto vsptr = _controller->create<Shader>( name + "_VS" );
  vsptr->init( Shader::Type::Vertex );
  if ( !vsptr->loadFromSource( src ) ) {
    throw Lore::Exception( "Failed to compile shadow vertex shader for " + name );
  }

  // ::::::::::::::::::::::::::::::::: //

  //
  // Fragment shader.

  src.clear();
  src = header;

  //
  // main function.

  src += "void main(){";
  {
    // Nothing to do here.
  }
  src += "}";

  auto fsptr = _controller->create<Shader>( name + "_FS" );
  fsptr->init( Shader::Type::Fragment );
  if ( !fsptr->loadFromSource( src ) ) {
    throw Lore::Exception( "Failed to compile shadow fragment shader for " + name );
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
  program->addUniformVar( "model" );

  // Uniform updaters.

  auto UniformUpdater = []( const RenderView& rv,
                            const GPUProgramPtr program,
                            const MaterialPtr material,
                            const RenderQueue::LightData& lights )
  {
  };

  auto UniformNodeUpdater = []( const GPUProgramPtr program,
                                const MaterialPtr material,
                                const NodePtr node,
                                const glm::mat4& viewProjection )
  {
    program->setUniformVar( "model", node->getFullTransform() );
  };

  program->setUniformUpdater( UniformUpdater );
  program->setUniformNodeUpdater( UniformNodeUpdater );

  return program;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::GPUProgramPtr GLStockResource3DFactory::createCubemapShadowProgram( const string& name, const bool instanced )
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

  if ( instanced ) {
    src += "layout (location = " + std::to_string( instancedMatrixTexUnit ) + ") in mat4 instanceMatrix;";
  }

  //
  // Uniforms and outs.

  src += "uniform mat4 model;";

  //
  // main function.

  src += "void main() {";
  {
    if ( instanced ) {
      src += "gl_Position = instanceMatrix * vec4(pos, 1.0);";
    }
    else {
      src += "gl_Position = model * vec4(pos, 1.0);";
    }
  }
  src += "}";

  auto vsptr = _controller->create<Shader>( name + "_VS" );
  vsptr->init( Shader::Type::Vertex );
  if ( !vsptr->loadFromSource( src ) ) {
    throw Lore::Exception( "Failed to compile shadow vertex shader for " + name );
  }

  // ::::::::::::::::::::::::::::::::: //

  //
  // Geometry shader.

  src.clear();
  src = header;

  src += "layout (triangles) in;";
  src += "layout (triangle_strip, max_vertices=18) out;";

  src += "uniform mat4 shadowMatrices[6];";

  src += "out vec4 FragPos;";

  //
  // main function.

  src += "void main() {";
  {
    src += "for (int face = 0; face < 6; ++face) {";
    {
      src += "gl_Layer = face;"; // Specify the face to render.
      src += "for (int i = 0; i < 3; ++i) {";
      {
        src += "FragPos = gl_in[i].gl_Position;";
        src += "gl_Position = shadowMatrices[face] * FragPos;";
        src += "EmitVertex();";
      }
      src += "}";

      src += "EndPrimitive();";
    }
    src += "}";
  }
  src += "}";

  auto gsptr = _controller->create<Shader>( name + "_GS" );
  gsptr->init( Shader::Type::Geometry );
  if ( !gsptr->loadFromSource( src ) ) {
    throw Lore::Exception( "Failed to compile shadow geometry shader for " + name );
  }


  // ::::::::::::::::::::::::::::::::: //

  //
  // Fragment shader.

  src.clear();
  src = header;

  src += "in vec4 FragPos;";

  src += "uniform vec3 lightPos;";
  src += "uniform float farPlane;";

  //
  // main function.

  src += "void main() {";
  {
    src += "float lightDist = length(FragPos.xyz - lightPos);";
    src += "lightDist /= farPlane;"; // Re-map to [0, 1].
    src += "gl_FragDepth = lightDist;";
  }
  src += "}";

  auto fsptr = _controller->create<Shader>( name + "_FS" );
  fsptr->init( Shader::Type::Fragment );
  if ( !fsptr->loadFromSource( src ) ) {
    throw Lore::Exception( "Failed to compile shadow fragment shader for " + name );
  }

  // ::::::::::::::::::::::::::::::::: //

  //
  // GPU program.

  auto program = _controller->create<Lore::GPUProgram>( name );
  program->init();
  program->attachShader( vsptr );
  program->attachShader( gsptr );
  program->attachShader( fsptr );

  if ( !program->link() ) {
    throw Lore::Exception( "Failed to link GPUProgram " + name );
  }

  //
  // Add uniforms.

  program->addUniformVar( "model" );
  program->addUniformVar( "lightPos" );
  program->addUniformVar( "farPlane" );

  for ( int i = 0; i < 6; ++i ) {
    program->addUniformVar( "shadowMatrices[" + std::to_string( i ) + "]" );
  }

  // Uniform updaters.

  auto UniformUpdater = []( const RenderView& rv,
                            const GPUProgramPtr program,
                            const MaterialPtr material,
                            const RenderQueue::LightData& lights )
  {
  };

  auto UniformNodeUpdater = []( const GPUProgramPtr program,
                                const MaterialPtr material,
                                const NodePtr node,
                                const glm::mat4& viewProjection )
  {
    program->setUniformVar( "model", node->getFullTransform() );
  };

  program->setUniformUpdater( UniformUpdater );
  program->setUniformNodeUpdater( UniformNodeUpdater );

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
  src += "uniform mat4 cameraModel;";

  //
  // main function.

  src += "void main(){";
  {
    src += "TexCoords = pos;";
    src += "vec4 opPos = viewProjection * cameraModel *  vec4(pos, 1.0);";
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

  src += "layout (location = 0) out vec4 pixel;";
  src += "layout (location = 1) out vec4 brightPixel;";

  src += "in vec3 TexCoords;";

  src += "uniform samplerCube skybox;";

  src += "uniform float gamma;";

  //
  // main function.

  src += "void main(){";
  {
    // Sample the texture value from the cubemap.
    src += "pixel = texture(skybox, TexCoords);";
    src += "pixel.rgb = pow(pixel.rgb, vec3(gamma));"; // Invert gamma correction for post-processing (it doesn't appear to be needed).

    // Don't bloom the skybox.
    src += "brightPixel = vec4(0.0, 0.0, 0.0, 1.0);";
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

  program->addUniformVar( "cameraModel" );
  program->addUniformVar( "viewProjection" );
  program->addUniformVar( "gamma" );

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
