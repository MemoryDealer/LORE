#pragma once
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// The MIT License (MIT)
// This source file is part of LORE2D
// ( Lightweight Object-oriented Rendering Engine )
//
// Copyright (c) 2016 Jordan Sparks
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

#include <string> // TODO: Figure out nicer way for clients to include dependencies.

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

    //
    // Forward declarations.

    class Camera;
    class GPUProgram;
    class IRenderer;
    class Material;
    class Node;
    class PostProcessor;
    class Renderable;
    class Scene;
    class Shader;
    class Texture;
    class Window;

    //
    // Pointer types.

    using CameraPtr = Camera*;
    using GPUProgramPtr = GPUProgram*;
    using MaterialPtr = Material*;
    using NodePtr = Node*;
    using RendererPtr = IRenderer*;
    using PostProcessorPtr = PostProcessor*;
    using RenderablePtr = Renderable*;
    using ScenePtr = Scene*;
    using ShaderPtr = Shader*;
    using TexturePtr = Texture*;
    using WindowPtr = Window*;

    //
    // Types.

    using real = float;
    using uint = unsigned int;
    using string = std::string;

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
