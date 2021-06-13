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

#include "Shaders.h"

#include <LORE/Lore.h>

#include <LORE/Core/APIVersion.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void LoadCustomShaders()
{
  std::string name = "space1";
  const std::string header = "#version " +
    std::to_string( Lore::APIVersion::GetMajor() ) + std::to_string( Lore::APIVersion::GetMinor() ) + "0" +
    " core\n";

  //
  // Vertex shader.

  std::string src = header;

  src.append( R"(
    layout (location = 0) in vec3 vertex;
    layout (location = 1) in vec2 texCoord;

    uniform mat4 transform;

    out vec4 FragPos;
    out vec2 UV;

    void main()
    {
      gl_Position = transform * vec4(vertex, 1.0);
      FragPos = transform * vec4(vertex, 1.0);
      UV = texCoord;
    }

  )" );

  auto vsptr = Lore::Resource::CreateShader( name + "_VS", Lore::Shader::Type::Vertex );
  if ( !vsptr->loadFromSource( src ) ) {
    throw Lore::Exception( "Failed to compile vertex shader for " + name );
  }

  //
  // Fragment shader.

  src = header;

  // Modified from source: http://glslsandbox.com/e#73375.0
  src.append( R"(
    layout (location = 0) out vec4 pixel;
    layout (location = 1) out vec4 brightPixel;

    in vec4 FragPos;
    in vec2 UV;

    uniform float time;
    uniform mat4 view;
    uniform vec2 resolution;

    #define iterations 15
    #define formuparam 0.530

    #define volsteps 18
    #define stepsize 0.120

    #define zoom   0.5
    #define tile   0.850
    #define speed  0.3

    #define brightness 0.0005
    #define darkmatter 0.400
    #define distfading 0.760
    #define saturation 0.800

    void main()
    {
      //get coords and direction
      vec2 uv = UV;
      //uv.y*=resolution.y/resolution.x;
      vec3 dir=vec3(uv*zoom,1.);
  
      float a2=time*0.0+.5;
      float a1=0.0;
      mat2 rot1=mat2(cos(a1),sin(a1),-sin(a1),cos(a1));
      mat2 rot2=rot1;//mat2(cos(a2),sin(a2),-sin(a2),cos(a2));
      dir.xz*=rot1;
      dir.xy*=rot2;
  
      //from.x-=time;
      vec3 from=vec3(0.,0.,0.);
      from+=vec3(.05*time*speed,.05*time*speed,-2.);
  
      from.xz*=rot1;
      from.xy*=rot2;
  
      //volumetric rendering
      float s=.4,fade=.2;
      vec3 v=vec3(0.4);
      for (int r=0; r<volsteps; r++) {
        vec3 p=from+s*dir*.5;
        p = abs(vec3(tile)-mod(p,vec3(tile*2.))); // tiling fold
        float pa,a=pa=0.;
        for (int i=0; i<iterations; i++) { 
          p=abs(p)/dot(p,p)-formuparam; // the magic formula
          a+=abs(length(p)-pa); // absolute sum of average change
          pa=length(p);
        }
        float dm=max(0.,darkmatter-a*a*.001); //dark matter
        a*=a*a*2.; // add contrast
        if (r>3) fade*=1.-dm; // dark matter, don't render near
        //v+=vec3(dm,dm*.5,0.);
        v+=fade;
        v+=vec3(s,s*s,s*s*s*s)*a*brightness*fade; // coloring based on distance
        fade*=distfading; // distance fading
        s+=stepsize;
      }
      v=mix(vec3(length(v)),v,saturation); //color adjust
      pixel = vec4(v*.01,1.);
      brightPixel = pixel * 0.01; //vec4(0.0, 0.0, 0.0, 1.0); // no bloom
    }
  )" );

  auto fsptr = Lore::Resource::CreateShader( name + "_FS", Lore::Shader::Type::Fragment );
  if ( !fsptr->loadFromSource( src ) ) {
    throw Lore::Exception( "Failed to compile fragment shader for " + name );
  }

  //
  // Link and setup uniforms/updaters.

  auto program = Lore::Resource::CreateGPUProgram( name );
  program->attachShader( vsptr );
  program->attachShader( fsptr );
  if ( !program->link() ) {
    throw Lore::Exception( "Failed to link program " + name );
  }

  program->addTransformVar( "transform" );
  program->addUniformVar( "time" );
  program->addUniformVar( "view" );
  program->addUniformVar( "resolution" );

  {
    auto UniformUpdater = []( const Lore::RenderView& rv,
      const Lore::GPUProgramPtr program,
      const Lore::MaterialPtr material,
      const Lore::RenderQueue::LightData& lights ) {
        static float time = 0.0f;
        time += 0.01f;
        program->setUniformVar( "time", time );

        program->setUniformVar( "view", rv.camera->getViewMatrix() );
        program->setUniformVar( "resolution", glm::vec2( rv.gl_viewport.width, rv.gl_viewport.height ) );
    };

    auto UniformNodeUpdater = []( const Lore::GPUProgramPtr program,
      const Lore::MaterialPtr material,
      const Lore::NodePtr node,
      const glm::mat4& viewProjection ) {
        const glm::mat4 model = node->getFullTransform();
        const glm::mat4 mvp = viewProjection * model;
        program->setTransformVar( mvp );
    };

    program->setUniformUpdater( UniformUpdater );
    program->setUniformNodeUpdater( UniformNodeUpdater );
  }

  program->allowMeshMaterialSettings = false;

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  name = "Space2";

  //
  // Vertex shader.

  src = header;

  src.append( R"(
    layout (location = 0) in vec3 vertex;
    layout (location = 1) in vec2 texCoord;

    uniform mat4 transform;

    out vec4 FragPos;
    out vec2 UV;

    void main()
    {
      gl_Position = transform * vec4(vertex, 1.0);
      FragPos = transform * vec4(vertex, 1.0);
      UV = texCoord;
    }

  )" );

  vsptr = Lore::Resource::CreateShader( name + "_VS", Lore::Shader::Type::Vertex );
  if ( !vsptr->loadFromSource( src ) ) {
    throw Lore::Exception( "Failed to compile vertex shader for " + name );
  }

  //
  // Fragment shader.

  src = header;

  // Modified from source: http://glslsandbox.com/e#73363.0
  src.append( R"(
    layout (location = 0) out vec4 pixel;
    layout (location = 1) out vec4 brightPixel;

    in vec4 FragPos;
    in vec2 UV;

    uniform float time;
    uniform mat4 view;
    uniform vec2 resolution;

    #define mouse vec2(sin(time)/48., cos(time)/48.)
    #define iterations 14
    #define formuparam2 0.79
 
    #define volsteps 5
    #define stepsize 0.390
 
    #define zoom 0.900
    #define tile   0.850
    #define speed2  500.0 
    #define brightness 0.003
    #define darkmatter 0.400
    #define distfading 0.560
    #define saturation 0.800


    #define transverseSpeed zoom*2.0
    #define cloud 0.11 

 
    float triangle(float x, float a) { 
      float output2 = 2.0*abs(  2.0*  ( (x/a) - floor( (x/a) + 0.5) ) ) - 1.0;
      return output2;
    }
 
    float field(in vec3 p) {	
      float strength = 7. + .03 * log(1.e-6 + fract(sin(time) * 4373.11));
      float accum = 0.;
      float prev = 0.;
      float tw = 0.;	

      for (int i = 0; i < 6; ++i) {
        float mag = dot(p, p);
        p = abs(p) / mag + vec3(-.5, -.8 + 0.1*sin(time*0.7 + 2.0), -1.1+0.3*cos(time*0.3));
        float w = exp(-float(i) / 7.);
        accum += w * exp(-strength * pow(abs(mag - prev), 2.3));
        tw += w;
        prev = mag;
      }
      return max(0., 5. * accum / tw - .7);
    }



    void main() {   
      const float uvf = 1.0;
      vec2 uv2 = 2. * UV / vec2(uvf);
      vec2 uvs = uv2 * vec2(uvf)  / uvf;
  
      float time2 = time;               
      float speed = speed2;
      speed = .01 * cos(time2*0.02 + 3.1415926/4.0);          
    
      float formuparam = formuparam2;
  
      vec2 uv = uvs;		       
  
      float a_xz = 0.9;
      float a_yz = -.6;
      float a_xy = 0.9 + time*0.08;	
  
      mat2 rot_xz = mat2(cos(a_xz),sin(a_xz),-sin(a_xz),cos(a_xz));	
      mat2 rot_yz = mat2(cos(a_yz),sin(a_yz),-sin(a_yz),cos(a_yz));		
      mat2 rot_xy = mat2(cos(a_xy),sin(a_xy),-sin(a_xy),cos(a_xy));
  

      float v2 =1.0;	
      vec3 dir=vec3(uv*zoom,1.); 
      vec3 from=vec3(0.0, 0.0,0.0);                               
            from.x -= 5.0*(mouse.x-0.5);
            from.y -= 5.0*(mouse.y-0.5);
               
               
      vec3 forward = vec3(0.,0.,1.);   
      from.x += transverseSpeed*(1.0)*cos(0.01*time) + 0.001*time;
      from.y += transverseSpeed*(1.0)*sin(0.01*time) +0.001*time;
      from.z += 0.003*time;	
  
      dir.xy*=rot_xy;
      forward.xy *= rot_xy;
      dir.xz*=rot_xz;
      forward.xz *= rot_xz;	
      dir.yz*= rot_yz;
      forward.yz *= rot_yz;
  
      from.xy*=-rot_xy;
      from.xz*=rot_xz;
      from.yz*= rot_yz;
  
      float zooom = (time2-3311.)*speed;
      from += forward* zooom;
      float sampleShift = mod( zooom, stepsize );
   
      float zoffset = -sampleShift;
      sampleShift /= stepsize;
  
  
      float s=0.24;
      float s3 = s + stepsize/2.0;
      vec3 v=vec3(0.);
      float t3 = 0.0;	
  
      vec3 backCol2 = vec3(0.);
      for (int r=0; r<volsteps; r++) {
        vec3 p2=from+(s+zoffset)*dir;
        vec3 p3=from+(s3+zoffset)*dir;
    
        p2 = abs(vec3(tile)-mod(p2,vec3(tile*2.)));
        p3 = abs(vec3(tile)-mod(p3,vec3(tile*2.)));		
        #ifdef cloud
        t3 = field(p3);
        #endif
    
        float pa,a=pa=0.;
        for (int i=0; i<iterations; i++) {
          p2=abs(p2)/dot(p2,p2)-formuparam;
      
          float D = abs(length(p2)-pa);
          a += i > 7 ? min( 12., D) : D;
          pa=length(p2);
        }
    
        a*=a*a;
    
        float s1 = s+zoffset;
    
        float fade = pow(distfading,max(0.,float(r)-sampleShift));		
      
        v+=fade;

        if( r == 0 )
          fade *= (1. - (sampleShift));
    
        if( r == volsteps-1 )
          fade *= sampleShift;
        v+=vec3(s1,s1*s1,s1*s1*s1*s1)*a*brightness*fade;
    
        backCol2 += mix(.4, 1., v2) * vec3(1.8 * t3 * t3 * t3, 1.4 * t3 * t3, t3) * fade;

    
        s+=stepsize;
        s3 += stepsize;		
      }
           
      v=mix(vec3(length(v)),v,saturation);	

      vec4 forCol2 = vec4(v*.01,1.);	
      #ifdef cloud
      backCol2 *= cloud;
      #endif	
      backCol2.b *= 1.8;
      backCol2.r *= 0.05;	
  
      backCol2.b = 0.5*mix(backCol2.g, backCol2.b, 0.8);
      backCol2.g = 0.0;
      backCol2.bg = mix(backCol2.gb, backCol2.bg, 0.5*(cos(time*0.01) + 1.0));	
      pixel = forCol2 + vec4(backCol2, 1.0);
      brightPixel = vec4(0.0, 0.0, 0.0, 1.0);
    }
  )" );

  fsptr = Lore::Resource::CreateShader( name + "_FS", Lore::Shader::Type::Fragment );
  if ( !fsptr->loadFromSource( src ) ) {
    throw Lore::Exception( "Failed to compile fragment shader for " + name );
  }

  //
  // Link and setup uniforms/updaters.

  program = Lore::Resource::CreateGPUProgram( name );
  program->attachShader( vsptr );
  program->attachShader( fsptr );
  if ( !program->link() ) {
    throw Lore::Exception( "Failed to link program " + name );
  }

  program->addTransformVar( "transform" );
  program->addUniformVar( "time" );
  program->addUniformVar( "view" );
  program->addUniformVar( "resolution" );

  {
    auto UniformUpdater = []( const Lore::RenderView& rv,
      const Lore::GPUProgramPtr program,
      const Lore::MaterialPtr material,
      const Lore::RenderQueue::LightData& lights ) {
        static float time = 0.0f;
        time += 0.01f;
        program->setUniformVar( "time", time );

        program->setUniformVar( "view", rv.camera->getViewMatrix() );
        program->setUniformVar( "resolution", glm::vec2( rv.gl_viewport.width, rv.gl_viewport.height ) );
    };

    auto UniformNodeUpdater = []( const Lore::GPUProgramPtr program,
      const Lore::MaterialPtr material,
      const Lore::NodePtr node,
      const glm::mat4& viewProjection ) {
        const glm::mat4 model = node->getFullTransform();
        const glm::mat4 mvp = viewProjection * model;
        program->setTransformVar( mvp );
    };

    program->setUniformUpdater( UniformUpdater );
    program->setUniformNodeUpdater( UniformNodeUpdater );
  }

  program->allowMeshMaterialSettings = false;

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  name = "Water";

  //
  // Vertex shader.

  src = header;

  src.append( R"(
    layout (location = 0) in vec3 vertex;
    layout (location = 1) in vec2 texCoord;

    uniform mat4 transform;

    out vec4 FragPos;
    out vec2 UV;

    void main()
    {
      gl_Position = transform * vec4(vertex, 1.0);
      FragPos = transform * vec4(vertex, 1.0);
      UV = texCoord;
    }

  )" );

  vsptr = Lore::Resource::CreateShader( name + "_VS", Lore::Shader::Type::Vertex );
  if ( !vsptr->loadFromSource( src ) ) {
    throw Lore::Exception( "Failed to compile vertex shader for " + name );
  }

  //
  // Fragment shader.

  src = header;

  // Modified from source: http://glslsandbox.com/e#73280.0
  src.append( R"(
    layout (location = 0) out vec4 pixel;
    layout (location = 1) out vec4 brightPixel;

    in vec4 FragPos;
    in vec2 UV;

    uniform float time;
    uniform mat4 view;
    uniform vec2 resolution;

    #define TAU 6.28318530718
    #define MAX_ITER 4
    //#define SHOW_TILING

    void main( void ) {


	    float t = time * .5+23.0;
        // uv should be the 0-1 uv of texture...
	    vec2 uv = UV * 1.5;


    #ifdef SHOW_TILING
	    vec2 p = mod(uv*TAU*2.0, TAU)-250.0;
    #else
        vec2 p = mod(uv*TAU, TAU)-250.0;
    #endif
	    vec2 i = vec2(p);
	    float c = 1.0;
	    float inten = .005;

	    for (int n = 1; n < MAX_ITER; n++) 
	    {
		    float t = t * (1.0 - (3.5 / float(n+1)));
		    i = p + vec2(cos(t - i.x) + sin(t + i.y), sin(t - i.y) + cos(t + i.x));
		    c += 1.0/length(vec2(p.x / (sin(i.x+t)/inten),p.y / (cos(i.y+t)/inten)));
	    }
	    c /= float(MAX_ITER);
	    c = 1.17-pow(c, 1.4);
	    vec3 colour = vec3(pow(abs(c), 8.0));
    	    colour = clamp(colour + vec3(0.0, 0.35, 0.6), 0.0, 1.0);
    
    #ifdef SHOW_TILING
	    // Flash tile borders...
	    vec2 pixel = 2.0 / resolution.xy;
	    uv *= 2.0;

	    float f = floor(mod(time*.5, 2.0)); 	// Flash value.
	    vec2 first = step(pixel, uv) * f;		   	// Rule out first screen pixels and flash.
	    uv  = step(fract(uv), pixel);				// Add one line of pixels per tile.
	    colour = mix(colour, vec3(1.0, 1.0, 0.0), (uv.x + uv.y) * first.x * first.y); // Yellow line
	
    #endif

	    pixel = vec4(colour, 1.0);
      brightPixel = vec4(0.0, 0.0, 0.0, 1.0);
    }

  )" );

  fsptr = Lore::Resource::CreateShader( name + "_FS", Lore::Shader::Type::Fragment );
  if ( !fsptr->loadFromSource( src ) ) {
    throw Lore::Exception( "Failed to compile fragment shader for " + name );
  }

  //
  // Link and setup uniforms/updaters.

  program = Lore::Resource::CreateGPUProgram( name );
  program->attachShader( vsptr );
  program->attachShader( fsptr );
  if ( !program->link() ) {
    throw Lore::Exception( "Failed to link program " + name );
  }

  program->addTransformVar( "transform" );
  program->addUniformVar( "time" );
  program->addUniformVar( "view" );
  program->addUniformVar( "resolution" );

  {
    auto UniformUpdater = []( const Lore::RenderView& rv,
      const Lore::GPUProgramPtr program,
      const Lore::MaterialPtr material,
      const Lore::RenderQueue::LightData& lights ) {
        static float time = 0.0f;
        time += 0.01f;
        program->setUniformVar( "time", time );

        program->setUniformVar( "view", rv.camera->getViewMatrix() );
        program->setUniformVar( "resolution", glm::vec2( 1.f, 1.f ) );
    };

    auto UniformNodeUpdater = []( const Lore::GPUProgramPtr program,
      const Lore::MaterialPtr material,
      const Lore::NodePtr node,
      const glm::mat4& viewProjection ) {
        const glm::mat4 model = node->getFullTransform();
        const glm::mat4 mvp = viewProjection * model;
        program->setTransformVar( mvp );
    };

    program->setUniformUpdater( UniformUpdater );
    program->setUniformNodeUpdater( UniformNodeUpdater );
  }

  program->allowMeshMaterialSettings = false;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
