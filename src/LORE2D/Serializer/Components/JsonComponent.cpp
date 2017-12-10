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

#include "JsonComponent.h"

#include <rapidjson/document.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

JsonSerializerComponent::~JsonSerializerComponent()
{

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void JsonSerializerComponent::serialize( const string& file )
{

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void JsonSerializerComponent::deserialize( const string& file )
{
  std::ifstream stream( file );
  if ( !stream.is_open() ) {
    throw Lore::Exception( "Failed to open file " + file );
  }

  // Read file contents into buffer.
  stream.seekg( 0, std::ios::end );
  const auto size = stream.tellg();
  string buffer( size, ' ' );
  stream.seekg( 0 );
  stream.read( &buffer[0], size );

  // Parse string into JSON document.
  rapidjson::Document doc;
  doc.Parse( buffer.c_str() );

  if ( doc.HasParseError() ) {
    throw Lore::Exception( "Parse error for JSON document: " + std::to_string( doc.GetParseError() ) );
  }

  for ( auto it = doc.MemberBegin(); it != doc.MemberEnd(); ++it ) {
    const auto& key = it->name;
    const auto& value = it->value;

    switch ( value.GetType() ) {
    default:
      log_error( string( "Unknown type for value " ) + key.GetString() );
      break;

    case rapidjson::Type::kNullType:

      break;

    case rapidjson::Type::kFalseType:
      _values[key.GetString()].setValue( false );
      break;

    case rapidjson::Type::kTrueType:
      _values[key.GetString()].setValue( true );
      break;

    case rapidjson::Type::kObjectType:

      break;

    case rapidjson::Type::kArrayType:

      break;

    case rapidjson::Type::kStringType:
      _values[key.GetString()].setValue( value.GetString() );
      break;

    case rapidjson::Type::kNumberType:
      if ( value.IsInt() ) {
        _values[key.GetString()].setValue( value.GetInt() );
      }
      else if ( value.IsFloat() ) {
        _values[key.GetString()].setValue( value.GetFloat() );
      }
      break;
    }
  }

  stream.close();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
