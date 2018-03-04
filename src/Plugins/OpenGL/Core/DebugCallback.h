#pragma once
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// The MIT License (MIT)
// This source file is part of LORE
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

namespace Lore { namespace OpenGL {

    class ContextCallbackHandler
    {

    public:

        inline static void APIENTRY OpenGLDebug( GLenum source,
                                                 GLenum type,
                                                 GLuint id,
                                                 GLenum severity,
                                                 GLsizei length,
                                                 const GLchar* message,
                                                 const void* userParam )
        {
            printf( "[OpenGL] %s\n", message );
            printf( "Source: " );

            switch ( source )
            {
            case GL_DEBUG_SOURCE_API:
                printf( "API" );
                break;
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
                printf( "Window System" );
                break;
            case GL_DEBUG_SOURCE_SHADER_COMPILER:
                printf( "Shader Compiler" );
                break;
            case GL_DEBUG_SOURCE_THIRD_PARTY:
                printf( "Third Party" );
                break;
            case GL_DEBUG_SOURCE_APPLICATION:
                printf( "Application" );
                break;
            case GL_DEBUG_SOURCE_OTHER:
                printf( "Other" );
                break;
            }

            printf( "\n" );
            printf( "Type: " );

            switch ( type )
            {
            case GL_DEBUG_TYPE_ERROR:
                printf( "Error" );
                break;
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
                printf( "Deprecated Behaviour" );
                break;
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
                printf( "Undefined Behaviour" );
                break;
            case GL_DEBUG_TYPE_PORTABILITY:
                printf( "Portability" );
                break;
            case GL_DEBUG_TYPE_PERFORMANCE:
                printf( "Performance" );
                break;
            case GL_DEBUG_TYPE_MARKER:
                printf( "Marker" );
                break;
            case GL_DEBUG_TYPE_PUSH_GROUP:
                printf( "Push Group" );
                break;
            case GL_DEBUG_TYPE_POP_GROUP:
                printf( "Pop Group" );
                break;
            case GL_DEBUG_TYPE_OTHER:
                printf( "Other" );
                break;
            }

            printf( "\n" );
            printf( "ID: %d\n", id );
            printf( "Severity: " );

            switch ( severity )
            {
            case GL_DEBUG_SEVERITY_HIGH:
                printf( "High" );
                break;
            case GL_DEBUG_SEVERITY_MEDIUM:
                printf( "Medium" );
                break;
            case GL_DEBUG_SEVERITY_LOW:
                printf( "Low" );
                break;
            case GL_DEBUG_SEVERITY_NOTIFICATION:
                printf( "Notification" );
                break;
            }

            printf( "\n" );
        }

    };

}}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
