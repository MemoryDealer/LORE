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

#include <LORE2D/Resource/ResourceController.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore { namespace OpenGL {

  ///
  /// \class GLResourceController
  /// \details OpenGL render plugin resource creation and loading support.
  class GLResourceController : public Lore::ResourceController
  {

  public:

    GLResourceController();
    ~GLResourceController() override = default;

    //
    // Functions for loading resources from the storage.

    TexturePtr loadTexture( const string& name,
                            const string& file,
                            const string& groupName = DefaultGroupName ) override;
    FontPtr loadFont( const string& name,
                      const string& file,
                      const uint32_t size,
                      const string& groupName = DefaultGroupName ) override;

    //
    // Factory functions for creation of resources.

    GPUProgramPtr createGPUProgram( const string& name,
                                    const string& groupName = DefaultGroupName ) override;
    ShaderPtr createVertexShader( const string& name,
                                  const string& groupName = DefaultGroupName ) override;
    ShaderPtr createFragmentShader( const string& name,
                                    const string& groupName = DefaultGroupName ) override;
    VertexBufferPtr createVertexBuffer( const string& name,
                                        const MeshType& type,
                                        const string& groupName = DefaultGroupName ) override;
    TexturePtr createTexture( const string& name,
                              const uint32_t width,
                              const uint32_t height,
                              const string& groupName = DefaultGroupName ) override;
    TexturePtr createTexture( const string& name,
                              const uint32_t width,
                              const uint32_t height,
                              const Color& color,
                              const string& groupName = DefaultGroupName ) override;
    RenderTargetPtr createRenderTarget( const string& name,
                                        const uint32_t width,
                                        const uint32_t height,
                                        const string& groupName = DefaultGroupName ) override;

    //
    // Destruction methods.

    void destroyTexture( TexturePtr texture ) override;
    void destroyTexture( const string& name, const string& groupName = DefaultGroupName ) override;
    void destroyFont( FontPtr font ) override;
    void destroyVertexShader( ShaderPtr vertexShader ) override;
    void destroyFragmentShader( ShaderPtr fragmentShader ) override;
    void destroyGPUProgram( GPUProgramPtr gpuProgram ) override;
    void destroyVertexBuffer( VertexBufferPtr vertexBuffer ) override;
    void destroyRenderTarget( RenderTargetPtr renderTarget ) override;

  };

}}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
