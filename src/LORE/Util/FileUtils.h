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

namespace Lore { namespace Util {

  static string GetFileExtension( const string& file )
  {
    const auto idx = file.rfind( '.' );
    if ( string::npos == idx ) {
      return string( "" );
    }

    return file.substr( idx + 1, file.size() - idx );
  }

  ///
  /// \brief Returns file name from a full file path.
  static string GetFileName( const string& file, const bool keepExtension = false )
  {
    auto lastSlash = file.rfind( '/' ) + 1;
    if ( 0 == lastSlash ) {
      // TODO: Generalize platform slashes.
      lastSlash = file.rfind( '\\' ) + 1;
    }
    const auto cutoff = ( keepExtension ) ? file.length() : file.rfind( '.' );

    return file.substr( lastSlash, cutoff - lastSlash );
  }

  ///
  /// \brief Returns directory of the file in the specified file path.
  static string GetFileDir( const string& filePath )
  {
    const auto lastSlash = filePath.rfind( '/' );
    return filePath.substr( 0, lastSlash );
  }

}}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
