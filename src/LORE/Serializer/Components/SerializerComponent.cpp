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

#include "SerializerComponent.h"

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

SerializerComponent::SerializerComponent()
: _values( "root" )
{ }

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

bool SerializerComponent::valueExists( const string& key )
{
  _lastLookup = _values._values.find( key );
  return ( _values._values.end() != _lastLookup );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

SerializerValue& SerializerComponent::getValue( const string& key )
{
  // Avoid second lookup if previous call to valueExists was for the same key.
  if ( _lastLookup != _values._values.end() && key == _lastLookup->first ) {
    return _lastLookup->second;
  }
  return _values[key];
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

const SerializerValue::Values& SerializerComponent::getValues() const
{
  return _values._values;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

SerializerValue& SerializerComponent::addValue( const string& key )
{
  auto it = _values._values.insert( { key, SerializerValue( key ) } );
  return it.first->second;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void SerializerComponent::addValue( const SerializerValue& value )
{
  _values._values[value.getKey()] = value;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //