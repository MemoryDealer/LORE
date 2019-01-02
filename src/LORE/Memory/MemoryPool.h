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

#include <LORE/Memory/Alloc.h>

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

      delete[] _objects;
    }

    inline T* create()
    {
      if ( !_next ) {
        throw MemoryException( "Pool " + _name + " has reached maximum capacity" );
        // TODO: Resize...
      }

      T* p = _next;
      assert( false == p->_inUse );
      p->_inUse = true;
      _next = p->_next;

      ++_activeObjectCount;

      return p;
    }

    inline void destroy( T* object )
    {
      if ( !object->_inUse ) {
        //throw Exception( "Attempting to destroy object not in use" );
        log_warning( "Attempted to destroy object not in use" );
        return;
      }

      Alloc<T>* alloc = object;

      // Destruct and re-construct the object in-place at its known address.
      object->~T();
      new ( object ) T();

      assert( false == alloc->_inUse );

      // Put this object at the front of the list.
      alloc->_next = _next;
      _next = object;

      --_activeObjectCount;

      // TODO: Pass double pointer or ref to assign object to null.
    }

    inline virtual void resize( const size_t newSize ) override
    {
      //...
    }

    inline virtual void destroyAll() override
    {
      for ( int i = 0; i < _size; ++i ) {
        if ( _objects[i]->_inUse ) {
          destroy( _objects[i] );
        }
      }
    }

    inline T* getObjectAt( const size_t idx )
    {
      if ( idx >= _size ) {
        throw MemoryException( "Invalid index " + std::to_string( idx ) );
      }

      return _objects[idx];
    }

    //
    // Information.

    inline size_t getSizeInBytes() const
    {
      return sizeof( T ) * _size;
    }

    inline size_t getActiveObjectCount() const
    {
      return _activeObjectCount;
    }

    inline size_t getTotalObjectCount() const
    {
      return _size;
    }

    inline void printUsage()
    {
      printf( "Pool %s usage: \n\n", _name.c_str() );
      for ( int i = 0; i < _size; ++i ) {
        printf( "Object %d \t[%s]\n", i, ( _objects[i]->_inUse ) ? "x" : " " );
      }
    }

  private:

    string _name {};
    size_t _size { 0 };
    size_t _activeObjectCount { 0 };
    List _objects {};

    T* _next { nullptr };

  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
