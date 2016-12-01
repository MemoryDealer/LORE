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

#include <LORE2D/Window/Window.h>

namespace Lore {

    class Context;

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    class LORE_EXPORT IRenderPluginLoader
    {

    public:

        virtual ~IRenderPluginLoader() { }

        virtual bool load( const string& file ) = 0;

        virtual std::unique_ptr<Context> createContext() = 0;

        virtual std::unique_ptr<Window> createWindow( const string& title,
                                                      const uint width,
                                                      const uint height ) = 0;

    protected:

        virtual void free() = 0;

    };

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

#if defined( _WIN32 ) || defined( _WIN64 )
#include <Windows.h>

    class LORE_EXPORT RenderPluginLoader : public IRenderPluginLoader
    {

    private:

        HMODULE _hModule;

    public:

        explicit constexpr RenderPluginLoader();

        virtual ~RenderPluginLoader() override;

        virtual bool load( const string& file ) override;

        virtual std::unique_ptr<Context> createContext() override;

        virtual std::unique_ptr<Window> createWindow( const string& title,
                                                      const uint width,
                                                      const uint height ) override;

    protected:

        virtual void free() override;

    };
#endif

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    inline std::unique_ptr<IRenderPluginLoader> CreateRenderPluginLoader()
    {
        std::unique_ptr<IRenderPluginLoader> rpl = std::make_unique<RenderPluginLoader>();
        return rpl;
    }

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
