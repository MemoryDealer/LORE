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

#include <memory>
#include <LORE2D/Lore.h>
#include <LORE2D/Resource/Renderable/Texture.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// Note: Error 1282 on binding calls with no prior errors indicates a wrong context.
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

#include <LORE2D/Memory/PoolCluster.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

int main( int argc, char** argv )
{
    auto context = Lore::CreateContext( Lore::RenderPlugin::OpenGL );

    Lore::WindowPtr window = context->createWindow( "Test", 640, 480 );
    window->setActive();

    /* 
    To load on a thread:

    "There is a way to use multiple contexts with GLFW.

    Make a second invisible window that has the main window as the parent (the last parameter is for list sharing)
    E.g.: GLUtil::_loaderWindow = glfwCreateWindow(1, 1, "whatever", nullptr, GLUtil::_mainWindow);
    Create another thread and in its run()(or whatever) method call this: glfwMakeContextCurrent(GLUtil::_loaderWindow);

    If you are using GLEW, build GLEW with the GLEW_MX define and call init() in the second thread as well. Read up on GLEW_MX about multithreaded usage. I strongly recommend boost thread specific pointers for this.

    Remember to destroy the second window just like the main one. That's it. I've been using this method for background resource loading for years and haven't ran into any issues.

    Hope it helps."
    */

    //context->createWindow( "Test2", 720, 640 );

    Lore::ScenePtr scene = context->createScene( "Default" );
    scene->setBackgroundColor( Lore::StockColor::Black );

    Lore::Viewport vp( 0.f, 0.f, 1.f, 1.f );
    Lore::CameraPtr camera = Lore::Resource::CreateCamera( "cam1" );
    Lore::RenderView rv( "main", scene, vp );
    rv.camera = camera;
    window->addRenderView( rv );

    Lore::NodePtr node = scene->createNode( "A" );
    node->setDepth( 0 );
    node = node->createChildNode( "AChild" );
    node = scene->createNode( "B " );
    node = node->createChildNode( "BChild" );
    node = node->createChildNode( "BChildChild" );

    // Textures.
    Lore::TexturePtr tex = Lore::Resource::LoadTexture( "tex1", "C:\\doggo.png" );
    auto entity = Lore::Resource::CreateEntity( "e1", Lore::MeshType::TexturedQuad );
    entity->getMaterial()->getPass().texture = tex;

    node = scene->getNode( "A" );
    //tex->getMaterial()->getPass().emissive = Lore::Color( 0.f, 0.2f, 0.f );
    //
    node->attachObject( entity );

    node->scale( Lore::Vec2( 2.f, 2.0f ) );

    // TODO: This change should propagate to renderer.
    //tex->setMaterial( context->getStockResourceController()->getMaterial( "UnlitStandardQuad" ) );
    node->getChild( "AChild" )->scale( Lore::Vec2( 0.5f, 1.f ) );

    node->getChild( "AChild" )->attachObject( entity );
    node->getChild( "AChild" )->setDepth( -66 );
    node->getChild( "AChild" )->setColorModifier( Lore::StockColor::Red );
    node->getChild( "AChild" )->setPosition( Lore::Vec2( -0.25f, 0.25f ) );

    auto n2 = scene->createNode( "n2" );
    n2->attachObject( entity );
    n2->scale( Lore::Vec2( 4.2f, 0.65f ) );
    n2->translate( 0.25f, 0.25f );
    n2->setDepth( -25 );

    auto light = scene->createLight( "l1" );
    light->setColor( Lore::Color( 1.f, .6f, 0.f ) );
    //light->setAttenuation( 3250.f, 1.f, 0.0014f, 0.000007f );
    node->attachObject( light );

    auto l2 = scene->createLight( "l2" );
    l2->setColor( Lore::Color( 0.1f, 1.f, 0.2f ) );
    n2->attachObject( l2 );

   

    scene->setAmbientLightColor( Lore::Color( 0.12f, 0.12f, 0.12f ) );

    /*for ( int i = 0; i < 80; ++i ) {
        auto n = node->createChildNode( "a" + std::to_string( i ) );
        n->translate( 0.5f * static_cast<float>( i + 1 ), 0.f );
        n->attachObject( entity );

        n->setColorModifier( Lore::Color( 1.f - static_cast< Lore::real >( i * .1f ), 1.f, 1.f ) );
    }*/

    float f = 0.f;
    while ( context->active() ) { 
        //node->translate( 0.01f * std::sinf( f ), 0.01f * std::cosf( f ) );
        f += 0.05f;
        //node->scale( 0.025f * std::sinf( f ) );

        // TODO: Repro case where both quads appeared to be scaling with only rotations being done.
        node->rotate( Lore::Degree( 0.5f ) );
        ////node->getChild( "AChild" )->rotate( Lore::Degree( -.1f ) );

        //n2->rotate( Lore::Degree( 0.025f ) );

        if ( GetAsyncKeyState( VK_F1 ) ) {
            //camera->translate( -0.01f, 0.f );
            node->translate( -0.01f, 0.f );
        }
        else if ( GetAsyncKeyState( VK_F2 ) ) {
            //camera->translate( 0.01f, 0.f );
            node->translate( 0.01f, 0.f );
        }
        else if ( GetAsyncKeyState( VK_F3 ) ) {
            //camera->translate( 0.f, -0.01f );
        }
        else if ( GetAsyncKeyState( VK_F4 ) ) {
            //camera->translate( 0.f, 0.01f );
        }
        else if ( GetAsyncKeyState( VK_F5 ) ) {
            camera->zoom( 0.01f );
        }
        else if ( GetAsyncKeyState( VK_F6 ) ) {
            camera->zoom( -0.01f );
        }

        if ( GetAsyncKeyState( VK_F8 ) ) {
            static bool destroyed = false;
            if ( !destroyed ) {
                Lore::Resource::DestroyTexture( tex );
                destroyed = true;
            }
        }

        context->renderFrame();
    }

    DestroyLoreContext( context );

#ifdef _DEBUG
    system( "pause" );
#endif
    return 0;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
