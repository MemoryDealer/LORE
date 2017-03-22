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

    class MemoryPoolBase
    {

    public:

        virtual ~MemoryPoolBase() { }

        virtual void resize( const size_t newSize ) = 0;

        virtual void resetAll() = 0;

    };

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    template<typename T>
    class MemoryPool final : public MemoryPoolBase
    {

    public:

        using List = T**;

    public:

        MemoryPool( const string& name, const size_t size )
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

        ~MemoryPool()
        {
            for ( int i = 0; i < _size; ++i ) {
                delete _objects[i];
            }

            delete [] _objects;
        }

        T* create()
        {
            assert( nullptr != _next );

            T* p = _next;
            assert( false == p->_inUse );
            p->_inUse = true;
            _next = p->_next;

            return p;
        }

        void destroy( T* object )
        {
            assert( true == object->_inUse );

            // Reset object to default.
            object->_inUse = false;
            object->_reset();

            // Put this object at the front of the list.
            object->_next = _next;
            _next = object;
        }

        virtual void resize( const size_t newSize ) override
        {
            // ...
        }

        virtual void resetAll() override
        {
            for ( int i = 0; i < _size; ++i ) {
                _objects[i]->_inUse = false;
                _objects[i]->_reset();
            }
        }

        //
        // Information.

        size_t getSizeInBytes()
        {
            return sizeof( T ) * _size;
        }

        void printUsage()
        {
            printf( "Pool %s usage: \n\n", _name.c_str() );
            for ( int i = 0; i < _size; ++i ) {
                printf( "Object %d %s\n", i, ( _objects[i]->_inUse ) ? "[x]" : "[ ]" );
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
