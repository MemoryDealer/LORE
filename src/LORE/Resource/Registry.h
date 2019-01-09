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

#include <LORE/Core/Iterator.h>
#include <LORE/Util/Util.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  using ID = string;

  ///
  /// \class Registry
  /// \brief Generic container class supporting multiple map types.
  template<template <typename ...> class MapType, typename T, typename ... MapParams>
  class Registry
  {

  public:

    using Iterator = MapIterator<MapType<string, T*, MapParams ...>>;
    using ConstIterator = ConstMapIterator<MapType<string, T*, MapParams ...>>;

  public:

    constexpr
    explicit Registry()
    {
    }

    void insert( const ID& id, T* resource )
    {
      const auto transformedID = Util::ToLower( id );
      if ( _container.find( transformedID ) != _container.end() ) {
        throw Lore::Exception( "Resource with id " + transformedID + " already exists" );
      }

      auto it = _container.begin();
      _container.insert( it, std::pair<ID, T*>( transformedID, resource ) );
    }

    void remove( const ID& id )
    {
      const auto transformedID = Util::ToLower( id );
      auto lookup = _container.find( transformedID );
      if ( _container.end() == lookup ) {
        LogWrite( Warning, "Tried to remove resource with id %s which does not exist", transformedID.c_str() );
        return;
      }

      _container.erase( transformedID );
    }

    void clear()
    {
      _container.clear();
    }

    T* get( const ID& id ) const
    {
      const auto transformedID = Util::ToLower( id );
      auto lookup = _container.find( transformedID );
      if ( _container.end() == lookup ) {
        throw Lore::ItemIdentityException( "Resource with id " + transformedID + " does not exist" );
      }

      return lookup->second;
    }

    bool exists( const ID& id ) const
    {
      return ( _container.find( Util::ToLower( id ) ) != _container.end() );
    }

    size_t size() const
    {
      return _container.size();
    }

    bool empty() const
    {
      return _container.empty();
    }

    Iterator getIterator()
    {
      return Iterator( std::begin( _container ), std::end( _container ) );
    }

    ConstIterator getConstIterator() const
    {
      return ConstIterator( std::begin( _container ), std::end( _container ) );
    }

    Registry clone()
    {
      Registry clone;
      for ( auto pair : _container ) {
        clone.insert( pair.first, pair.second );
      }
      return clone;
    }

    //
    // Deleted functions/operators.

    Registry& operator = ( const Registry& rhs ) = default;
    Registry( const Registry& rhs ) = default;

  private:

    MapType<string, T*, MapParams ...> _container {};

  };

  ///
  /// \class SafeRegistry
  /// \brief Thread-safe version of Registry.


}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
