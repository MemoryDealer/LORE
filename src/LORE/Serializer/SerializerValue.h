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

#include <LORE/Math/Math.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  class LORE_EXPORT SerializerValue final
  {

  public:

    using Array = std::vector<SerializerValue>;
    using Values = std::unordered_map<string, SerializerValue>;

    enum class Type
    {
      Container,
      Bool,
      Array,
      String,
      Int,
      Real
    };

    Type getType() const
    {
      if ( HAS_VARIANT_TYPE<bool>( _value ) ) {
        return Type::Bool;
      }
      if ( HAS_VARIANT_TYPE<string>( _value ) ) {
        return Type::String;
      }
      if ( HAS_VARIANT_TYPE<int>( _value ) ) {
        return Type::Int;
      }
      if ( HAS_VARIANT_TYPE<real>( _value ) ) {
        return Type::Real;
      }
      if ( HAS_VARIANT_TYPE<Array>( _value ) ) {
        return Type::Array;
      }

      return Type::Container;
    }

  public:

    SerializerValue() : _key() {}
    SerializerValue( const string& key ) : _key( key ) {}
    ~SerializerValue() = default;

    SerializerValue& operator[]( const string& key )
    {
      auto it = _values.find( key );
      if ( _values.end() == it ) {
        SerializerValue v( key );
        auto iit = _values.insert( { key, v } );
        return iit.first->second;
      }
      return it->second;
    }

    const SerializerValue& operator[]( const string& key ) const
    {
      return get( key );
    }

    //
    // Getters.

    string getKey() const
    {
      return _key;
    }

    bool hasValue( const string& key ) const
    {
      return ( _values.end() == _values.find( key ) );
    }

    const SerializerValue& get( const string& key ) const
    {
      auto it = _values.find( key );
      if ( _values.end() == it ) {
        throw Lore::Exception( "Non-existent key lookup on const SerializerValue" );
      }
      return it->second;
    }

    const Values& getValues() const
    {
      return _values;
    }

    bool toBool() const
    {
      return GET_VARIANT<bool>( _value );
    }

    const string& toString() const
    {
      return GET_VARIANT<string>( _value );
    }

    int toInt() const
    {
      return GET_VARIANT<int>( _value );
    }

    real toReal() const
    {
      return GET_VARIANT<real>( _value );
    }

    const Array& toArray() const
    {
      return GET_VARIANT<Array>( _value );
    }

    glm::vec4 toVec4() const
    {
      if ( Type::Array == getType() ) {
        const auto& values = toArray();
        assert( 4 == values.size() );
        return glm::vec4( values[0].toReal(), values[1].toReal(), values[2].toReal(), values[3].toReal() );
      }
      throw Lore::Exception( "Value not array type" );
    }

    Rect toRect() const
    {
      if ( Type::Array == getType() ) {
        const auto& values = toArray();
        assert( 4 == values.size() );
        return Rect( values[0].toReal(), values[1].toReal(), values[2].toReal(), values[3].toReal() );
      }
      throw Lore::Exception( "Value not array type" );
    }

    //
    // Setters.

    void operator = ( const bool value )
    {
      _value = value;
    }

    void operator = ( const string& value )
    {
      _value = value;
    }

    void operator = ( const char* value )
    {
      _value = string( value );
    }

    void operator = ( const int value )
    {
      _value = value;
    }

    void operator = ( const real value )
    {
      _value = value;
    }

    void operator = ( const Array& value )
    {
      _value = value;
    }

  private:

    friend class SerializerComponent;
    friend class JsonSerializerComponent;

  private:

    using ValueHolder = LORE_VARIANT<MONOSTATE, bool, string, int, real, Array>;

  private:

    string _key { };
    ValueHolder _value {};
    Values _values {};

  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
