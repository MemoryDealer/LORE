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
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"gl_Position = projection * view * model * vec4(vertex, 1.0f, 1.0f);\n"
"}\n";

const static Lore::string pshader_src =
"#version 450 core\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = vec4( 0.0f, 1.0f, 0.0f, 1.0f );\n"
"}\n";

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

int main( int argc, char** argv )
{
    auto context = Lore::CreateContext( Lore::RenderPlugin::OpenGL );

    Lore::WindowPtr window = context->createWindow( "Test", 640, 480 );
    window->setActive();
    //context->createWindow( "Test2", 720, 640 );

    Lore::ScenePtr scene = context->createScene( "Default" );
    scene->setBackgroundColor( Lore::StockColor::Black );

    Lore::Viewport vp( 0.f, 0.f, 1.f, 1.f );
    Lore::RenderView rv( "main", scene, vp );
    window->addRenderView( rv );

    Lore::NodePtr node = scene->createNode( "A" );
    node = node->createChildNode( "AChild" );
    node = scene->createNode( "B " );
    node = node->createChildNode( "BChild" );
    node = node->createChildNode( "BChildChild" );

    Lore::ResourceLoader& loader = context->getResourceLoader();

    // Textures.
    Lore::TexturePtr tex = loader.loadTexture( "tex1", "C:\\Texture.png" );

    Lore::GPUProgramPtr program = loader.createGPUProgram( "GPU1" );
    Lore::ShaderPtr vshader = loader.createVertexShader( "v1" );
    vshader->loadFromSource( vshader_src.c_str() );

    Lore::ShaderPtr fshader = loader.createFragmentShader( "f1" );
    fshader->loadFromSource( pshader_src.c_str() );

    Lore::VertexBufferPtr vb = loader.createVertexBuffer( "vb1", Lore::VertexBuffer::Type::Quad );
    vb->addAttribute( Lore::VertexBuffer::AttributeType::Float, 2 );
    //vb->addAttribute( Lore::VertexBuffer::AttributeType::Float, 2 );
    vb->build();

   // ; How will changing material on renderable propagate to render queues? Should do so with pointers?

    program->setVertexBuffer( vb );
    program->attachShader( vshader );
    program->attachShader( fshader );
    program->link();

    program->addUniformVar( "model" );
    program->addUniformVar( "view" );
    program->addUniformVar( "projection" );

    Lore::MaterialPtr mat = loader.createMaterial( "mat1" );
    mat->getPass( 0 ).program = program;
    tex->setMaterial( mat );

    node->attachObject( ( Lore::RenderablePtr )tex );

    auto it = scene->getNode( "A" )->getChildNodeIterator();
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
