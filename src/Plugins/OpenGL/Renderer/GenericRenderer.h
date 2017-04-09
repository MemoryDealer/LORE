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

#include <LORE2D/Renderer/IRenderer.h>

#include <LORE2D/Resource/Material.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore { namespace OpenGL {

    ///
    /// \class GenericRenderer
    /// \brief Renders a scene normally, without any special behavior.
    class GenericRenderer : public Lore::IRenderer
    {

    public:

        const size_t DefaultRenderQueueCount = 100;

    public:

        GenericRenderer();

        virtual ~GenericRenderer() override;

        virtual RenderQueue::Entry addRenderable( Lore::RenderablePtr r,
                                                  Lore::Matrix4& model ) override;

        virtual void present( const Lore::RenderView& rv,
                              const WindowPtr window ) override;

    private:

        void activateQueue( const uint id, Lore::RenderQueue& rq );

        void renderMaterialMap( const Lore::ScenePtr scene,
                                RenderQueue::MaterialMap& mm,
                                const Matrix4& view,
                                const Matrix4& projection ) const;

    private:

        using RenderQueueList = std::vector<RenderQueue>;
        using ActiveRenderQueueList = std::map<uint, RenderQueue&>;

    private:

        RenderQueueList _queues;
        ActiveRenderQueueList _activeQueues;

    };

}}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
