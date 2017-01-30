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
#include <LORE2D/Resource/Material.h>
#include <LORE2D/Resource/Renderable/Texture.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

const static Lore::string vshader_src =
"#version 450 core\n"
"layout (location = 0) in vec2 vertex;\n"
"layout (location = 1) in vec2 texCoord;"
"uniform mat4 transform;\n"
""
"out vec2 TexCoord;"
"void main()\n"
"{\n"
"gl_Position = transform * vec4(vertex, 0.0f, 1.0f);\n"
"TexCoord = vec2(texCoord.x, 1.0 - texCoord.y);"
"}\n";

const static Lore::string pshader_src =
"#version 450 core\n"
"uniform sampler2D tex1;"
"in vec2 TexCoord;"
"out vec4 color;\n"
"void main()\n"
"{\n"
//"color = vec4( 0.0f, 0.0f, 1.0f, 1.0f );\n"
"color = texture( tex1, TexCoord);"
"}\n";

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// Note: Error 1282 on binding calls with no prior errors indicates a wrong context.
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
    Lore::ResourceController& loader = *context->getResourceController();

    Lore::ScenePtr scene = context->createScene( "Default" );
    scene->setBackgroundColor( Lore::StockColor::Black );

    Lore::Viewport vp( 0.f, 0.f, 1.f, 1.f );
    Lore::CameraPtr camera = loader.createCamera( "cam1" );
    Lore::RenderView rv( "main", scene, vp );
    rv.camera = camera;
    window->addRenderView( rv );

    Lore::NodePtr node = scene->createNode( "A" );
    node = node->createChildNode( "AChild" );
    node = scene->createNode( "B " );
    node = node->createChildNode( "BChild" );
    node = node->createChildNode( "BChildChild" );

    // Textures.
    Lore::TexturePtr tex = loader.loadTexture( "tex1", "C:\\texture.png" );

   // Lore::GPUProgramPtr program = loader.createGPUProgram( "GPU1" );
   // Lore::ShaderPtr vshader = loader.createVertexShader( "v1" );
   // vshader->loadFromSource( vshader_src.c_str() );

   // Lore::ShaderPtr fshader = loader.createFragmentShader( "f1" );
   // fshader->loadFromSource( pshader_src.c_str() );

   // Lore::VertexBufferPtr vb = loader.createVertexBuffer( "vb1", Lore::VertexBuffer::Type::TexturedQuad );
   // vb->build();

   //// ; How will changing material on renderable propagate to render queues? Should do so with pointers?
   // // Remove createVertexBuffer and have GPUProgram own one, so there's not another variable to deal with
   // // Consider doing the same for vertex and fragment shader
   // program->setVertexBuffer( vb );
   // program->attachShader( vshader );
   // program->attachShader( fshader );
   // program->link();

   // program->addTransformVar( "transform" );

    //Lore::MaterialPtr mat = loader.createMaterial( "mat1" );
    //mat->getPass( 0 ).program = program;
    //Lore::MaterialPtr mat = Lore::StockResource::GetMaterial( "StandardTexturedQuad" );
    //Lore::MaterialPtr mat = loader.createMaterial( "mat1" );
    //mat->getPass( 0 ).program = Lore::StockResource::GetGPUProgram( "StandardTexturedQuad" );
    //mat->getPass( 0 ).program = program;
    //tex->setMaterial( mat );

    node = scene->getNode( "A" );
    node->attachObject( ( Lore::RenderablePtr )tex );

    node->getChild( "AChild" )->attachObject( ( Lore::RenderablePtr )tex );
    node->getChild( "AChild" )->setPosition( Lore::Vec2( -0.25f, 0.25f ) );

    float f = 0.f;
    while ( context->active() ) { 
        //node->translate( 0.05f * std::sinf( f ), 0.05f * std::cosf( f ) );
        f += 0.05f;
        //node->scale( 0.025f * std::sinf( f ) );

        // TODO: Repro case where both quads appeared to be scaling with only rotations being done.
        node->rotate( Lore::Degree( 0.5f ) );
        node->getChild( "AChild" )->rotate( Lore::Degree( -1.f ) );

        if ( GetAsyncKeyState( VK_F1 ) ) {
            camera->translate( -0.01f, 0.f );
        }
        else if ( GetAsyncKeyState( VK_F2 ) ) {
            camera->translate( 0.01f, 0.f );
        }
        else if ( GetAsyncKeyState( VK_F3 ) ) {
            camera->translate( 0.f, -0.01f );
        }
        else if ( GetAsyncKeyState( VK_F4 ) ) {
            camera->translate( 0.f, 0.01f );
        }
        else if ( GetAsyncKeyState( VK_F5 ) ) {
            camera->zoom( 0.01f );
        }
        else if ( GetAsyncKeyState( VK_F6 ) ) {
            camera->zoom( -0.01f );
        }

        context->renderFrame( 0 );
    }

    DestroyLoreContext( context );

#ifdef _DEBUG
    system( "pause" );
#endif
    return 0;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
