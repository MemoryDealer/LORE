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

#include <LORE2D/Core/Iterator.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

    using id = string;

    ///
    /// \class Registry
    /// \brief Generic container class supporting multiple map types.
    template<template <typename ...> typename MapType, typename T, typename ... MapParams>
    class Registry
    {

    public:

        using Iterator = MapIterator<MapType<string, T*, MapParams ...>>;
        using ConstIterator = ConstMapIterator<MapType<string, T*, MapParams ...>>;

    public:

        constexpr
        explicit Registry()
        : _container()
        {
        }

        void insert( const id& id_, T* resource )
        {
            if ( _container.find( id_ ) != _container.end() ) {
                throw Lore::Exception( "Resource with id " + id_ + " already exists" );
            }

            auto it = _container.begin();
            _container.insert( it, std::pair<id, T*>( id_, resource ) );
        }

        void remove( const id& id_ )
        {
            auto lookup = _container.find( id_ );
            if ( _container.end() == lookup ) {
                log_warning( "Tried to remove resource with id " + id_ + " which does not exist" );
                return;
            }

            _container.erase( id_ );
        }

        T* get( const id& id_ ) const
        {
            auto lookup = _container.find( id_ );
            if ( _container.end() == lookup ) {
                throw Lore::ItemIdentityException( "Resource with id " + id_ + " does not exist" );
            }

            return lookup->second;
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

        ConstIterator getConstIterator()
        {
            return ConstIterator( std::begin( _container ), std::end( _container ) );
        }

        //
        // Deleted functions/operators.

        Registry& operator = ( const Registry& rhs ) = delete;
        Registry( const Registry& rhs ) = delete;

    private:

        MapType<string, T*, MapParams ...> _container;

    };

    ///
    /// \class SafeRegistry
    /// \brief Thread-safe version of Registry.


}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
