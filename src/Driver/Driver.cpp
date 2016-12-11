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

#include <memory>
#include <LORE2D/Lore.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

int main( int argc, char** argv )
{
    auto context = Lore::CreateContext( Lore::RenderPlugin::OpenGL );

    Lore::WindowPtr window = context->createWindow( "Test", 640, 480 );
    context->createWindow( "Test2", 720, 640 );

    Lore::ScenePtr scene = context->createScene( "Default" );
    scene->setBackgroundColor( Lore::StockColor::White );

    Lore::Viewport vp( 0.f, 0.f, 0.5f, 1.f );
    Lore::RenderView rv( "main", scene, vp );
    window->addRenderView( rv );

    Lore::NodePtr node = scene->createNode( "A" );
    node = node->createChildNode( "AChild" );
    node = scene->createNode( "B " );
    node = node->createChildNode( "BChild" );
    node = node->createChildNode( "BChildChild" );

    auto it = scene->getNode( "A" )->getChildIterator();
    while ( it.hasMore() ) {
        node = it.getNext();
        ;
    }

    while ( context->active() ) {
        context->renderFrame( 0 );
    }

    DestroyLoreContext( context );

#ifdef _DEBUG
    system( "pause" );
#endif
    return 0;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
