#pragma once
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

namespace Lore {

  class SerializerValue final
  {

  public:

    enum class Type
    {
      Null,
      Bool,
      Array,
      String,
      Int,
      Real
    };

    Type getType() const
    {
      if ( std::holds_alternative<bool>( _value ) ) {
        return Type::Bool;
      }
      if ( std::holds_alternative<string>( _value ) ) {
        return Type::String;
      }
      if ( std::holds_alternative<int>( _value ) ) {
        return Type::Int;
      }
      if ( std::holds_alternative<real>( _value ) ) {
        return Type::Real;
      }

      return Type::Null;
    }

    bool getBool() const
    {
      return std::get<bool>( _value );
    }

    string getString() const
    {
      return std::get<string>( _value );
    }

    int getInt() const
    {
      return std::get<int>( _value );
    }

    real getReal() const
    {
      return std::get<real>( _value );
    }

    //
    // Setters.

    void setValue( const bool value )
    {
      _value = value;
    }

    void setValue( const char* value )
    {
      _value = string( value );
    }

    void setValue( const string& value )
    {
      _value = value;
    }

    void setValue( const int value )
    {
      _value = value;
    }

    void setValue( const real value )
    {
      _value = value;
    }

  private:

    using Value = std::variant<bool, string, int, real>;
    Value _value;

  };

  using SerializerValueMap = std::unordered_map<string, SerializerValue>;

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
