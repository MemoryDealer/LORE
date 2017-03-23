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

#include <LORE2D/Memory/Alloc.h>
#include <LORE2D/Window/RenderView.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

    class LORE_EXPORT Window : public Alloc<Window>
    {

    public:

        enum class Mode {
            Windowed,
            Fullscreen,
            FullscreenBorderless
        };

    public:

        Window();

        virtual ~Window();

        //
        // Rendering.

        virtual void renderFrame() { }

        virtual void addRenderView( const RenderView& renderView );

        virtual void removeRenderView( const RenderView& renderView );
        virtual void removeRenderView( const string& name );

        virtual RenderView& getRenderView( const string& name );

        virtual void resetRenderViews();

        //
        // Modifiers.

        virtual void setTitle( const string& title );

        virtual void setDimensions( const int width, const int height );

        virtual void setMode( const Mode& mode );

        virtual void setActive() = 0;

        //
        // Getters.

        inline string getTitle() const
        {
            return _title;
        }

        inline int getWidth() const
        {
            return _frameBufferWidth;
        }

        inline int getHeight() const
        {
            return _frameBufferHeight;
        }

        inline real getAspectRatio() const
        {
            return _aspectRatio;
        }

        inline void getDimensions( int& width, int& height )
        {
            width = _frameBufferWidth;
            height = _frameBufferHeight;
        }

        ///
        /// \brief Returns full width of window, including borders.
        inline int getFullWidth() const
        {
            return _width;
        }

        ///
        /// \brief Returns full height of window, including borders.
        inline int getFullHeight() const
        {
            return _height;
        }

        ResourceControllerPtr getResourceController() const;

        StockResourceControllerPtr getStockResourceController() const;

    protected:

        using RenderViewList = std::vector<RenderView>;

    protected:

        string _title;
        int _width, _height;
        int _frameBufferWidth, _frameBufferHeight;
        real _aspectRatio;
        Mode _mode;

        RenderViewList _renderViews;

        std::unique_ptr<ResourceController> _controller;
        std::unique_ptr<StockResourceController> _stockController;

    };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
