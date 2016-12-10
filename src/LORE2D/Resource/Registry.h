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

    template<typename T>
    class Registry;

    template<typename T>
    class RegistryIterator
    {

    public:

        explicit RegistryIterator( Registry<T>& reg )
        : _registry( reg )
        , _hasMore( true )
        , _itOffset( 0 )
        //, _lock( _registry._mutex )
        {
            
        }

        ~RegistryIterator()
        {
            
        }

        // A hack for now.
        // TODO: Implement a proper iterator wrapper.
        T* getNext()
        {
            std::lock_guard<std::mutex> lock( _registry._mutex );

            auto it = _registry._container.begin();

            for ( int i = 0; i < _itOffset; ++i ) {
                ++it;
            }
            ++_itOffset;
            
            _hasMore = ( it != _registry._container.end() );
            if ( _hasMore ) {
                T* next = it->second.get();

                ++it;
                _hasMore = ( it != _registry._container.end() );

                return next;
            }

            return nullptr;
        }

        bool hasMore() const
        {
            return _hasMore;
        }

    private:

        Registry<T>& _registry;
        bool _hasMore;
        int _itOffset;
        //std::unique_lock<std::mutex> _lock;

    };

    // TODO: Create macro to allow both map and unordered_map as containers (or variadic templates: template<class ...> class C ?).
    // TODO: Make thread-safe and non-thread-safe version.

    template<typename T>
    class Registry
    {

    public:

        using id = string;
        using Container = std::map<id, std::unique_ptr<T>>;

    public:

        constexpr
        explicit Registry()
        : _container()
        , _mutex()
        {
        }

        void insert( const id& id_, std::unique_ptr<T> resource )
        {
            std::lock_guard<std::mutex> lock( _mutex );
            
            if ( _container.find( id_ ) != _container.end() ) {
                throw Lore::Exception( "Resource with id " + id_ + " already exists" );
            }

            auto it = _container.begin();
            _container.insert( it, std::pair<id, std::unique_ptr<T>>( id_, std::move( resource ) ) );
        }

        void remove( const id& id_ )
        {
            std::lock_guard<std::mutex> lock( _mutex );

            auto lookup = _container.find( id_ );
            if ( _container.end() == lookup ) {
                log_warning( "Tried to remove resource with id " + id_ + " which does not exist" );
            }

            _container.erase( id_ );
        }

        T* get( const id& id_ ) const
        {
            std::lock_guard<std::mutex> lock( _mutex );

            auto lookup = _container.find( id_ );
            if ( _container.end() == lookup ) {
                throw Lore::Exception( "Resource with id " + id_ + " does not exist" );
            }

            return lookup->second.get();
        }

        size_t size() const
        {
            std::lock_guard<std::mutex> lock( _mutex );
            return _container.size();
        }

        bool empty() const
        {
            std::lock_guard<std::mutex> lock( _mutex );
            return _container.empty();
        }

        RegistryIterator<T> getIterator()
        {
            RegistryIterator<T> r( *this );
            return r;
        }

        //
        // Deleted functions/operators.

        Registry& operator = ( const Registry& rhs ) = delete;
        Registry( const Registry& rhs ) = delete;

    private:

        Container _container;

        mutable std::mutex _mutex;

        friend class RegistryIterator<T>;

    };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
