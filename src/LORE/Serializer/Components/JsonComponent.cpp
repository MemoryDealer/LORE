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

#include "JsonComponent.h"

#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

JsonSerializerComponent::~JsonSerializerComponent()
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void JsonSerializerComponent::serialize( const string& file )
{
  std::ofstream stream( file, std::ofstream::out );
  if ( !stream.is_open() ) {
    throw Lore::Exception( "Failed to open file for " + file + " for writing" );
  }

  // Create RapidJSON document and add existing values to it.
  rapidjson::Document doc( rapidjson::kObjectType );

  std::function<void( const SerializerValue&, rapidjson::Value& )> AddValues =
    [&] ( const SerializerValue& value, rapidjson::Value& root ) {
    auto& allocator = doc.GetAllocator();
    rapidjson::Value key( value.getKey(), allocator );
    rapidjson::Value newValue;

    switch ( value.getType() ) {
    default:
      break;

    case SerializerValue::Type::Container:
      newValue.SetObject();
      for ( auto& childValue : value._values ) {
        AddValues( childValue.second, newValue );
      }
      root.AddMember( key, newValue, allocator );
      break;

    case SerializerValue::Type::Bool:
      newValue = value.toBool();
      root.AddMember( key, newValue, allocator );
      break;

    case SerializerValue::Type::Array: {
      newValue.SetArray();
      auto& arrayValues = value.toArray();
      for ( const auto& arrayValue : arrayValues ) {
        switch ( arrayValue.getType() ) {
        case SerializerValue::Type::Container:
        {
          rapidjson::Value newArrayValue( rapidjson::kObjectType );
          for ( auto& childValue : arrayValue._values ) {
            AddValues( childValue.second, newArrayValue );
          }
          newValue.PushBack( newArrayValue, allocator );
        } break;

        case SerializerValue::Type::Bool: {
          rapidjson::Value newArrayValue;
          newArrayValue = arrayValue.toBool();
          newValue.PushBack( newArrayValue, allocator );
        } break;

        case SerializerValue::Type::Array: {
          // Do nothing with recursive arrays for now.
        } break;

        case SerializerValue::Type::String: {
          rapidjson::Value newArrayValue;
          newArrayValue.SetString( arrayValue.toString().c_str(),
                              static_cast<rapidjson::SizeType>( arrayValue.toString().length() ),
                              allocator );
          newValue.PushBack( newArrayValue, allocator );
        } break;

        case SerializerValue::Type::Int:
        {
          rapidjson::Value newArrayValue;
          newArrayValue = arrayValue.toInt();
          newValue.PushBack( newArrayValue, allocator );
        } break;

        case SerializerValue::Type::Real:
        {
          rapidjson::Value newArrayValue;
          newArrayValue = arrayValue.toReal();
          newValue.PushBack( newArrayValue, allocator );
        } break;

        }
      }
      root.AddMember( key, newValue, allocator );
    } break;

    case SerializerValue::Type::String:
      newValue.SetString( value.toString().c_str(),
                          static_cast<rapidjson::SizeType>( value.toString().length() ),
                          allocator );
      root.AddMember( key, newValue, allocator );
      break;

    case SerializerValue::Type::Int:
      newValue = value.toInt();
      root.AddMember( key, newValue, allocator );
      break;

    case SerializerValue::Type::Real:
      newValue = value.toReal();
      root.AddMember( key, newValue, allocator );
      break;
    }
  };

  // Recurse serializer values and add to RapidJSON value.
  rapidjson::Value root( rapidjson::kObjectType );
  for ( auto& childValue : _values._values ) {
    AddValues( childValue.second, root );
  }

  // Copy root value to entire document.
  doc.CopyFrom( root, doc.GetAllocator() );

  // Generate the JSON pretty-formatted string.
  rapidjson::StringBuffer sb;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer( sb );
  doc.Accept( writer );

  // Write string to file.
  stream << sb.GetString();
  stream.close();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void JsonSerializerComponent::deserialize( const string& file )
{
  std::ifstream stream( file );
  if ( !stream.is_open() ) {
    throw Lore::Exception( "Failed to open file " + file + " for reading" );
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

  std::function<void(rapidjson::Value::ConstMemberIterator, SerializerValue&)> HandleValue =
    [&] ( rapidjson::Value::ConstMemberIterator it, SerializerValue& serializerValue ) {
    const auto& key = it->name;
    const auto& value = it->value;

    switch ( value.GetType() ) {
    default:
      log_error( string( "Unknown type for value " ) + key.GetString() );
      break;

    case rapidjson::Type::kNullType:

      break;

    case rapidjson::Type::kFalseType:
      serializerValue[key.GetString()] = false;
      break;

    case rapidjson::Type::kTrueType:
      serializerValue[key.GetString()] = true;
      break;

    case rapidjson::Type::kObjectType: {
      auto object = value.GetObject();
      for ( auto objectIt = object.MemberBegin(); objectIt != object.MemberEnd(); ++objectIt ) {
        auto& objectValue = serializerValue[key.GetString()];
        HandleValue( objectIt, objectValue );
      }
    } break;

    case rapidjson::Type::kArrayType: {
      SerializerValue::Array values;
      // Add all array values.
      for ( auto arrayIt = value.Begin(); arrayIt != value.End(); ++arrayIt ) {
        SerializerValue v( key.GetString() );
        switch ( arrayIt->GetType() ) {
        case rapidjson::Type::kNullType:
          // Do nothing.
          break;

        case rapidjson::Type::kArrayType:
          // Do nothing for recursive arrays for now.
          break;

        case rapidjson::Type::kFalseType:
          v = false;
          break;

        case rapidjson::Type::kTrueType:
          v = true;
          break;

        case rapidjson::Type::kObjectType: {
          auto object = arrayIt->GetObject();
          for ( auto objectIt = object.MemberBegin(); objectIt != object.MemberEnd(); ++objectIt ) {
            HandleValue( objectIt, v );
          }
        } break;

        case rapidjson::Type::kStringType:
          v = string( arrayIt->GetString() );
          break;

        case rapidjson::Type::kNumberType:
          if ( arrayIt->IsInt() ) {
            v = arrayIt->GetInt();
          }
          else if ( arrayIt->IsFloat() ) {
            v = arrayIt->GetFloat();
          }
          break;
        }
        values.push_back( v );
      }
      serializerValue[key.GetString()] = values;
    } break;

    case rapidjson::Type::kStringType:
      serializerValue[key.GetString()] = string( value.GetString() );
      break;

    case rapidjson::Type::kNumberType:
      if ( value.IsInt() ) {
        serializerValue[key.GetString()] = value.GetInt();
      }
      else if ( value.IsFloat() ) {
        serializerValue[key.GetString()] = value.GetFloat();
      }
      break;
    }
  };

  for ( auto it = doc.MemberBegin(); it != doc.MemberEnd(); ++it ) {
    HandleValue( it, _values );
  }

  stream.close();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
