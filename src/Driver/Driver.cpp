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
#include <LORE2D/Scene/Background.h>
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
  Lore::TexturePtr tex = Lore::Resource::LoadTexture( "tex1", "C:\\doge.jpg" );
  auto entity = Lore::Resource::CreateEntity( "e1", Lore::MeshType::TexturedQuad );
  //entity->getMaterial()->getPass().texture = Lore::StockResource::GetTexture("White");
  entity->getMaterial()->getPass().setTextureScrollSpeed( Lore::Vec2( 0.01f, 0.f ) );
  entity->setTexture( tex );

  node = scene->getNode( "A" );
  node->attachObject( entity );

  node->scale( Lore::Vec2( 2.f, 2.0f ) );

  // TODO: This change should propagate to renderer.
  //tex->setMaterial( context->getStockResourceController()->getMaterial( "UnlitStandardQuad" ) );
  node->getChild( "AChild" )->scale( Lore::Vec2( 0.5f, 1.f ) );

  node->getChild( "AChild" )->attachObject( entity );
  auto achild = node->getChild( "AChild" );
  achild->setDepth( 50 );
  node->getChild( "AChild" )->setColorModifier( Lore::StockColor::Red );
  node->getChild( "AChild" )->setPosition( Lore::Vec2( -0.25f, 0.25f ) );

  auto n2 = scene->createNode( "n2" );
  n2->attachObject( entity );
  n2->scale( Lore::Vec2( 4.2f, 0.65f ) );
  n2->translate( 0.25f, 0.25f );
  n2->setDepth( 25 );

  auto light = scene->createLight( "l1" );
  light->setColor( Lore::Color( 0.f, .6f, 0.9f ) );
  //light->setAttenuation( 3250.f, 1.f, 0.0014f, 0.000007f );
  node->attachObject( light );

  auto l2 = scene->createLight( "l2" );
  l2->setColor( Lore::Color( 0.1f, 1.f, 0.2f ) );
  //n2->attachObject( l2 );

  Lore::Resource::LoadTexture( "bg_default", "C:\\clouds.jpg" );
  Lore::BackgroundPtr bg = scene->getBackground();
  Lore::Background::Layer& layer = bg->addLayer( "default" );
  layer.setTexture( Lore::Resource::GetTexture( "bg_default" ) );
  layer.setScrollSpeed( Lore::Vec2( 0.001f, 0.002f ) );
  layer.setDepth( 950.f );

  auto& layer2 = bg->addLayer( "face" );
  layer2.setTexture( Lore::Resource::LoadTexture( "bg_face", "C:\\awesomeface.png" ) );
  layer2.setScrollSpeed( Lore::Vec2( -0.002f, 0.001f ) );
  layer2.setDepth( 500.f );

  auto& layer3 = bg->addLayer( "face2" );
  layer3.setTexture( Lore::Resource::GetTexture( "bg_face" ) );
  layer3.setScrollSpeed( Lore::Vec2( 0.002f, -0.001f ) );
  layer3.setDepth( 499.f );

  auto& layer4 = bg->addLayer( "face3" );
  layer4.setTexture( Lore::Resource::GetTexture( "bg_face" ) );
  layer4.setScrollSpeed( Lore::Vec2( -0.001f, -0.001f ) );
  layer4.setDepth( 498.f );

  {
    auto& layer4 = bg->addLayer( "face4" );
    layer4.setTexture( Lore::Resource::GetTexture( "bg_face" ) );
    layer4.setScrollSpeed( Lore::Vec2( -0.003f, 0.003f ) );
    layer4.setDepth( 497.f );
  }

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
    node->getChild( "AChild" )->rotate( Lore::Degree( -.1f ) );

    n2->rotate( Lore::Degree( 0.025f ) );

    if ( GetAsyncKeyState( VK_F1 ) ) {
      node->translate( -0.01f, 0.f );
    }
    else if ( GetAsyncKeyState( VK_F2 ) ) {

      node->translate( 0.01f, 0.f );
    }

    const float speed = 0.01f;
    if ( GetAsyncKeyState( 0x57 ) ) {
      camera->translate( 0.f, -speed );
    }
    else if ( GetAsyncKeyState( 0x53 ) ) {
      camera->translate( 0.f, speed );
    }
    if ( GetAsyncKeyState( 0x41 ) ) {
      camera->translate( speed, 0.f );
    }
    else if ( GetAsyncKeyState( 0x44 ) ) {
      camera->translate( -speed, 0.f );
    }
    if ( GetAsyncKeyState( 0x5A ) ) {
      camera->zoom( 0.01f );
    }
    else if ( GetAsyncKeyState( 0x58 ) ) {
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
