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

#include <LORE2D/Memory/MemoryPool.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

    ///
    /// \class PoolCluster
    /// \brief Owner of multiple MemoryPool instances for registered types.
    class PoolCluster final
    {

    public:

        PoolCluster( const string& name )
        : _name( name )
        , _pools()
        { }

        ~PoolCluster()
        { }

        //
        // Pool management.

        template<typename T>
        void registerPool( const size_t size )
        {
            auto t = std::type_index( typeid( T ) );
            auto lookup = _pools.find( t );
            if ( _pools.end() == lookup ) {
                _pools[t] = std::make_unique<MemoryPool<T>>( typeid( T ).name(), size );
            }
        }

        template<typename T>
        void resizePool( const size_t newSize )
        {
            auto pool = _getPool<T>();
            if ( pool ) {
                pool->resize( newSize );
            }
        }

        template<typename T>
        void unregisterPool()
        {
            auto t = std::type_index( typeid( T ) );
            auto lookup = _pools.find( t );
            if ( _pools.end() != lookup ) {
                _pools.erase( lookup );
            }
        }

        void resetAllPools()
        {
            for ( auto& pair : _pools ) {
                pair.second->resetAll();
            }
        }

        //
        // Object management.

        template<typename T>
        T* create()
        {
            auto pool = _getPool<T>();
            if ( pool ) {
                MemoryPool<T>* TPool = static_cast< MemoryPool<T>* >( pool );
                return TPool->create();
            }

            throw Lore::Exception( "PoolCluster::create<T>: Pool of type " +
                                   string( typeid( T ).name() ) + " does not exist" );
        }

        template<typename T>
        void destroy( T* object )
        {
            auto pool = _getPool<T>();
            if ( pool ) {
                MemoryPool<T>* TPool = static_cast< MemoryPool<T>* >( pool );
                TPool->destroy( object );
            }
            else {
                throw Lore::Exception( "PoolCluster::create<T>: Pool of type " +
                                       string( typeid( T ).name() ) + " does not exist" );
            }
        }

    private:

        using PoolTable = std::unordered_map<std::type_index, std::unique_ptr<MemoryPoolBase>>;

    private:

        template<typename T>
        MemoryPoolBase* _getPool()
        {
            auto t = std::type_index( typeid( T ) );
            auto lookup = _pools.find( t );
            if ( _pools.end() != lookup ) {
                return lookup->second.get();
            }

            return nullptr;
        }

    private:

        string _name;
        PoolTable _pools;

    };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
