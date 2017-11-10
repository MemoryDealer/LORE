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
#include <sstream>

#include <LORE2D/Lore.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// Note: Error 1282 on binding calls with no prior errors indicates a wrong context.
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

#include <LORE2D/Memory/PoolCluster.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

static Lore::Timer __timer;
static Lore::CameraPtr __camera;
static Lore::NodePtr __sonic;
static void UpdateFPS( Lore::TextboxPtr textbox );
static void OnKeyChanged( const Lore::Keycode key, const bool state );
static void OnChar( const char c );

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
  __camera = camera;
  Lore::RenderView rv( "main", scene, vp );
  rv.camera = camera;
  rv.ui = Lore::Resource::CreateUI( "UI-1" );
  //rv.renderTarget = Lore::Resource::CreateRenderTarget( "rt1", 1920, 1080 );
  window->addRenderView( rv );

  //
  // Create sonic node and light.

  auto sonicNode = scene->createNode( "sonic" );
  sonicNode->setDepth( -50.f );
  auto sonicEntity = Lore::Resource::CreateEntity( "sonic", Lore::MeshType::TexturedQuad );
  auto sonicTexture = Lore::Resource::LoadTexture( "sonic-mobile", "H:\\sonic-mobile.png" );
  sonicEntity->setTexture( sonicTexture );
  //sonicEntity->getMaterial()->getPass().blendingMode.enabled = true;
  sonicEntity->getMaterial()->diffuse.a = 0.5f;
  sonicNode->attachObject( sonicEntity );

  auto sonicLight = scene->createLight( "sonic-light" );
  sonicLight->setColor( Lore::StockColor::White );
  sonicNode->attachObject( sonicLight );

  camera->trackNode( sonicNode );

  //
  // Create some doges.

  auto dogeEntity = Lore::Resource::CreateEntity( "doge", Lore::MeshType::TexturedQuad );
  auto dogeTexture = Lore::Resource::LoadTexture( "doge", "H:\\doge.jpg" );
  std::vector<Lore::NodePtr> doges;
  dogeEntity->setTexture( dogeTexture );
  for ( int i = 0; i < 5; ++i ) {
    auto node = scene->createNode( "doge" + std::to_string( i ) );
    node->attachObject( dogeEntity );
    node->setPosition( static_cast< float >( i ) / 2.f, 0.f );
    doges.push_back( node );
  }

  // Create some blended boxes.

  auto boxEntity = Lore::Resource::CreateEntity( "box", Lore::MeshType::Quad );
  boxEntity->getMaterial()->blendingMode.enabled = true;
  boxEntity->getMaterial()->diffuse = Lore::Color( 0.1f, 0.4f, 0.8f, 0.95f );
  for ( int i = 0; i < 5; ++i ) {
    auto node = scene->createNode( "box" + std::to_string( i ) );
    node->attachObject( boxEntity );
    node->setPosition( -1.f - static_cast< float >( i ) / 2.f, 0.f );
  }

  // Create floating text.

  auto textNode = scene->createNode( "text1" );
  auto textbox = Lore::Resource::CreateTextbox( "text1" );
  auto font = Lore::Resource::LoadFont( "Sega", "res/fonts/sega.ttf", 14 );
  textbox->setFont( font );
  textbox->setText( "SEGA" );
  textNode->attachObject( textbox );

  //
  // Create background.

  Lore::Resource::LoadTexture( "bg_city", "H:\\clouds.jpg" );
  Lore::Resource::LoadTexture( "death-egg", "H:\\bg.png" );
  auto bg = scene->getBackground();
  auto& layer = bg->addLayer( "1" );
  layer.setTexture( Lore::Resource::GetTexture( "bg_city" ) );
  layer.setScrollSpeed( Lore::Vec2( 0.0008f, 0.0004f ) );
  layer.setParallax( Lore::Vec2( 0.05f, 0.05f ) );
  layer.setDepth( 1001.f );

  auto& layer2 = bg->addLayer( "2" );
  layer2.setTexture( Lore::Resource::GetTexture( "death-egg" ) );
  layer2.setParallax( Lore::Vec2(0.1f, 0.1f) );
  layer2.getMaterial()->blendingMode.enabled = true;
  layer2.getMaterial()->diffuse.a = 0.75f;
  layer2.getMaterial()->setTextureSampleRegion( Lore::Rect( 0.0f, 0.0f, 0.55f, 0.55f ) );
  //layer.getMaterial()->getPass().setTextureSampleRegion( 0.15f, 0.05f, 0.08f, 0.58f );

  //Lore::Resource::LoadTexture( "bg_default", "C:\\clouds.jpg" );
  //Lore::BackgroundPtr bg = scene->getBackground();
  //Lore::Background::Layer& layer = bg->addLayer( "default" );
  //layer.setTexture( Lore::Resource::GetTexture( "bg_default" ) );
  //layer.setScrollSpeed( Lore::Vec2( 0.001f, 0.002f ) );
  //layer.setDepth( 950.f );

  //auto& layer2 = bg->addLayer( "face" );
  //layer2.setTexture( Lore::Resource::LoadTexture( "bg_face", "C:\\awesomeface.png" ) );
  //layer2.setScrollSpeed( Lore::Vec2( -0.002f, 0.001f ) );
  //layer2.setDepth( 500.f );

  //auto& layer3 = bg->addLayer( "face2" );
  //layer3.setTexture( Lore::Resource::GetTexture( "bg_face" ) );
  //layer3.setScrollSpeed( Lore::Vec2( 0.002f, -0.001f ) );
  //layer3.setDepth( 499.f );

  //auto& layer4 = bg->addLayer( "face3" );
  //layer4.setTexture( Lore::Resource::GetTexture( "bg_face" ) );
  //layer4.setScrollSpeed( Lore::Vec2( -0.001f, -0.001f ) );
  //layer4.setDepth( 498.f );
  ////layer4.getMaterial()->getPass().setTextureSampleRegion( 0.5f, 0.5f, 0.5f, 0.5f );

  //{
  //  auto& layer4 = bg->addLayer( "face4" );
  //  layer4.setTexture( Lore::Resource::GetTexture( "bg_face" ) );
  //  layer4.setScrollSpeed( Lore::Vec2( -0.003f, 0.003f ) );
  //  layer4.setDepth( 497.f );
  //}

  scene->setAmbientLightColor( Lore::Color( 0.12f, 0.12f, 0.12f, 1.f ) );

  /*for ( int i = 0; i < 80; ++i ) {
  auto n = node->createChildNode( "a" + std::to_string( i ) );
  n->translate( 0.5f * static_cast<float>( i + 1 ), 0.f );
  n->attachObject( entity );

  n->setColorModifier( Lore::Color( 1.f - static_cast< Lore::real >( i * .1f ), 1.f, 1.f ) );
  }*/

  // ---

  // RTT.

  /*Lore::ScenePtr postScene = context->createScene( "pp" );
  Lore::CameraPtr postCamera = Lore::Resource::CreateCamera( "postCam" );
  Lore::RenderView postProcessor( "pp", postScene, vp );
  postProcessor.camera = postCamera;
  window->addRenderView( postProcessor );

  auto pLight = postScene->createLight( "l1" );
  pLight->setColor( Lore::Color( .9f, .4f, .9f, 1.f ) );

  postScene->setAmbientLightColor( Lore::Color( 0.12f, 0.12f, 0.12f, 1.f ) );

  auto ppe = Lore::Resource::CreateEntity( "ppe", Lore::MeshType::TexturedQuad );
  ppe->getMaterial()->lighting = false;
  ppe->getMaterial()->program = Lore::StockResource::GetGPUProgram( "UnlitTexturedRTT" );
  ppe->setTexture( rv.renderTarget->getTexture() );
  auto ppeNode = postScene->createNode( "ppe" );
  ppeNode->attachObject( ppe );
  ppeNode->attachObject( pLight );
  ppeNode->scale( Lore::Vec2( 10.6664f, 8.f ) );*/

  // ---

  //
  // UI.

  auto panel = rv.ui->createPanel( "P-1" );
  auto fpsElement = panel->createElement( "FPS" );
  auto fpsTextbox = Lore::Resource::CreateTextbox( "fps" );
  fpsElement->setTextbox( fpsTextbox );
  fpsElement->setPosition( -1.f, 0.92f );
  fpsTextbox->setText( "Calculating..." );
  fpsTextbox->setTextColor( Lore::StockColor::Green );
  //fpsTextbox->setFont( font );

  __sonic = sonicNode;
  Lore::Input::SetKeyCallback( OnKeyChanged );
  Lore::Input::SetCharCallback( OnChar );

  float f = 0.f;
  __timer.reset();
  bool pause = true;
  while ( context->active() ) {
    __timer.tick();

    //node->translate( 0.01f * std::sinf( f ), 0.01f * std::cosf( f ) );
    f += 0.0005f;
    //sonicNode->scale( 10.05f * std::sinf( f ) );
    //textureElement->setPosition( f, 0.5f );
    // TODO: Repro case where both quads appeared to be scaling with only rotations being done (create test for this).
    //node->getChild( "AChild" )->rotate( Lore::Degree( -.1f ) );

    {
      static float sin = 0.f;
      textNode->setPosition( 0.f, std::sinf( sin ) * 0.5f );
      sin += 0.01f;
    }

    for ( auto doge : doges ) {
      doge->rotate( Lore::Degree( .1f ) );
    }

    float speed = 0.01f;
    if ( Lore::Input::GetKeymodState( Lore::Keymod::Shift ) ) {
      speed *= 2.f;
    }
    if ( Lore::Input::GetKeyState( Lore::Keycode::W ) ) {
      sonicNode->translate( 0.f, speed );
    }
    else if ( Lore::Input::GetKeyState( Lore::Keycode::S ) ) {
      sonicNode->translate( 0.f, -speed );
    }
    if ( Lore::Input::GetKeyState( Lore::Keycode::D ) ) {
      sonicNode->translate( speed, 0.f );
    }
    else if ( Lore::Input::GetKeyState( Lore::Keycode::A ) ) {
      sonicNode->translate( -speed, 0.f );
    }
    if ( Lore::Input::GetKeyState( Lore::Keycode::Z ) ) {
      camera->zoom( 0.01f );
    }
    else if ( Lore::Input::GetKeyState( Lore::Keycode::X ) ) {
      camera->zoom( -0.01f );
    }

    if ( Lore::Input::GetKeyState( Lore::Keycode::F8 ) ) {
      static bool destroyed = false;
      if ( !destroyed ) {
        Lore::Resource::DestroyTexture( sonicTexture );
        destroyed = true;
      }
    }

    if ( Lore::Input::GetKeyState( Lore::Keycode::Escape ) ) {
      pause = false;
      break;
    }

    if ( Lore::Input::GetMouseButtonState( Lore::MouseButton::Left ) ) {
      printf( "Left!\n" );
    }
    if ( Lore::Input::GetMouseButtonState( Lore::MouseButton::Right ) ) {
      printf( "Right!\n" );
    }

    UpdateFPS( fpsTextbox );
    context->renderFrame();
  }

  DestroyLoreContext( context );

  #ifdef _DEBUG
  if ( pause ) {
    system( "pause" );
  }
  #endif

  return 0;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

// TODO: Move this to internal debug UI.
static void UpdateFPS( Lore::TextboxPtr textbox )
{
  static int32_t frameCount = 0;
  static float elapsed = 0.f;

  ++frameCount;

  // Get averages over one second period.
  if ( ( __timer.getTotalElapsedTime() - elapsed ) >= 1.f ) {
    float fps = static_cast< float >( frameCount );
    float mspf = 1000.f / fps;

    std::ostringstream oss;
    oss.precision( 3 );
    oss << "FPS: " << fps << "    " << "Frame Time: " << mspf << " (ms)";
    textbox->setText( oss.str() );

    frameCount = 0;
    elapsed += 1.f;
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

static void OnKeyChanged( const Lore::Keycode key, const bool state )
{
  const float speed = 0.01f;

  switch ( key ) {
  default:
    break;

  case Lore::Keycode::C:
    {
      static bool enabled = true;
      enabled = !enabled;
      Lore::Input::SetCursorEnabled( enabled );
    }
    break;
  }
}

static void OnChar( const char c )
{
  printf( "OnChar: %c\n", c );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
