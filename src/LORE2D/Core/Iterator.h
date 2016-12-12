#pragma once
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// The MIT License (MIT)
// This source file is part of LORE2D
// ( Lightweight Object-oriented Rendering Engine )
//
// Copyright (c) 2016 Jordan Sparks
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

    ///
    /// \class Iterator
    /// \brief Wrapper for iterators of containers.
    template<typename T, typename ItType, typename ValType>
    class IteratorWrapper
    {

    public:

        using ValueType = ValType;
        using PointerType = ValType*;

    public:

        IteratorWrapper( ItType begin, ItType end )
        : _begin( begin ), _current( begin ), _end( end )
        { }

        bool hasMore() const
        {
            return ( _current != _end );
        }

        void moveNext()
        {
            ++_current;
        }

        const ItType& begin()
        {
            return _begin;
        }

        ItType& current()
        {
            return _current;
        }

        const ItType& end()
        {
            return _end;
        }

        //
        // Deleted functions/operators.

        IteratorWrapper() = delete;

    protected:

        ItType _begin;
        ItType _current;
        ItType _end;

    };

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    template<typename T, typename ItType>
    class MapIteratorWrapper : public IteratorWrapper<T, ItType, typename T::mapped_type>
    {

    public:

        using ValueType = typename IteratorWrapper<T, ItType, typename T::mapped_type>::ValueType;
        using PointerType = typename IteratorWrapper<T, ItType, typename T::mapped_type>::PointerType;

        using KeyType = typename T::key_type;

    public:

        MapIteratorWrapper( ItType begin, ItType end )
        : IteratorWrapper<T, ItType, typename T::mapped_type>( begin, end )
        { }

        KeyType peekNextKey() const
        {
            return _current->first;
        }

        ValueType peekNextValue() const
        {
            return _current->second;
        }

        const PointerType peekNextValuePtr() const
        {
            return &( _current->second );
        }

        ValueType getNext()
        {
            return ( _current++ )->second;
        }

    };

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    template<typename T>
    class MapIterator : public MapIteratorWrapper<T, typename T::iterator>
    {

    public:

        MapIterator( typename T::iterator begin, typename T::iterator end )
        : MapIteratorWrapper<T, typename T::iterator>( begin, end )
        { }

        explicit MapIterator( T& c )
        : MapIteratorWrapper<T, typename T::iterator>( c.begin(), c.end() )
        { }

    };

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    // For iterating a map with value of unique_ptr<T>.
    template<typename T>
    class UniqueMapIterator : public MapIteratorWrapper<T, typename T::iterator>
    {

    public:

        using UniqueType = typename ValueType::element_type*;

    public:

        UniqueMapIterator( typename T::iterator begin, typename T::iterator end )
        : MapIteratorWrapper<T, typename T::iterator>( begin, end )
        {
        }

        explicit UniqueMapIterator( T& c )
        : MapIteratorWrapper<T, typename T::iterator>( c.begin(), c.end() )
        {
        }

        UniqueType getNext()
        {
            return ( _current++ )->second.get();
        }

    };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
