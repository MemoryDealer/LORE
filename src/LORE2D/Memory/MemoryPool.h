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

#include <LORE2D/Memory/Alloc.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

    ///
    /// \class MemoryPoolBase
    /// \brief Non-template base class for MemoryPool, so instances can be
    ///     stored in a map in the PoolCluster class.
    class MemoryPoolBase
    {

    public:

        virtual ~MemoryPoolBase() { }

        virtual void resize( const size_t newSize ) = 0;

        virtual void destroyAll() = 0;

    };

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    ///
    /// \class MemoryPool
    /// \brief Generic memory pool with O(1) creation/deletion time.
    template<typename T>
    class MemoryPool final : public MemoryPoolBase
    {

    public:

        using List = T**;

    public:

        inline MemoryPool( const string& name, const size_t size )
        : _name( name )
        , _size( size )
        , _objects()
        , _next( nullptr )
        {
            // Allocate pool and each object.
            _objects = new T*[_size];
            for ( int i = 0; i < _size; ++i ) {
                _objects[i] = new T();
            }

            // Setup linked list.
            for ( int i = 0; i < _size - 1; ++i ) {
                _objects[i]->_next = _objects[i + 1];
            }

            // Setup head and tail.
            _next = _objects[0];
            _objects[_size - 1]->_next = nullptr;
        }

        inline virtual ~MemoryPool() override
        {
            for ( int i = 0; i < _size; ++i ) {
                delete _objects[i];
            }

            delete [] _objects;
        }

        inline T* create()
        {
            assert( nullptr != _next );

            T* p = _next;
            assert( false == p->_inUse );
            p->_inUse = true;
            _next = p->_next;

            return p;
        }

        inline void destroy( T* object )
        {
            assert( true == object->_inUse );

            Alloc<T>* alloc = object;

            // Reset object to default.
            alloc->_inUse = false;
            alloc->_reset();

            // Put this object at the front of the list.
            alloc->_next = _next;
            _next = object;
        }

        inline virtual void resize( const size_t newSize ) override
        {
            //...
        }

        inline virtual void destroyAll() override
        {
            for ( int i = 0; i < _size; ++i ) {
                Alloc<T>* alloc = _objects[i];
                alloc->_inUse = false;
                alloc->_reset();
            }
        }

        inline T* getObjectAt( const size_t idx )
        {
            assert( idx < _size );

            return _objects[idx];
        }

        //
        // Information.

        inline size_t getSizeInBytes()
        {
            return sizeof( T ) * _size;
        }

        inline void printUsage()
        {
            printf( "Pool %s usage: \n\n", _name.c_str() );
            for ( int i = 0; i < _size; ++i ) {
                printf( "Object %d \t[%s]\n", i, ( _objects[i]->_inUse ) ? "x" : " " );
            }
        }

    private:

        string _name;
        size_t _size;
        List _objects;

        T* _next;

    };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
