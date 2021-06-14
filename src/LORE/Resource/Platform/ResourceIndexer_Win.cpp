// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// The MIT License (MIT)
// This source file is part of LORE
// ( Lightweight Object-oriented Rendering Engine )
//
// Copyright (c) 2017-2021 Jordan Sparks
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

#if LORE_PLATFORM == LORE_WINDOWS

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

#include <LORE/Resource/IO/ResourceIndexer.h>
#include <LORE/Resource/ResourceController.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ResourceIndexer::traverseDirectory( const string& directory, ResourceControllerPtr resourceController, const bool recursive )
{
  WIN32_FIND_DATA fd { 0 };
  HANDLE hFind { nullptr };

  // Replace all forward slashes with backslashes.
  string windowsDirectory = directory;
  std::replace( windowsDirectory.begin(), windowsDirectory.end(), '/', '\\' );

  const string wildcard( "\\*.*" );
  windowsDirectory.append( wildcard );
  char buf[MAX_PATH] = { 0 };
  strcpy_s( buf, sizeof(buf), windowsDirectory.c_str() );

  hFind = FindFirstFile( buf, &fd );
  do {
    if ( strncmp( fd.cFileName, ".", 1 ) == 0 ||
         strncmp( fd.cFileName, "..", 2 ) == 0 ) {
      continue;
    }

    if ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
      if ( recursive ) {
        traverseDirectory( directory + "/" + fd.cFileName, resourceController, true );
      }
      continue;
    }

    string file = directory + "/" + fd.cFileName;
    resourceController->indexResourceFile( file );
  } while ( FindNextFile( hFind, &fd ) );
  FindClose( hFind );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

#endif

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
