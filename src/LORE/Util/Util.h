#pragma once
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// The MIT License (MIT)
// This source file is part of LORE
// ( Lightweight Object-oriented Rendering Engine )
//
// Copyright (c) 2017-2021 Jordan Sparks
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

#include <LORE/Util/FileUtils.h>
#include <LORE/Util/StringUtils.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore { namespace Util {

    //
    // Provide unordered_map that supports strongly typed enums as a key.

    // Hash function for strongly typed enums.
    struct EnumClassHash
    {
      template<typename T>
      std::size_t operator()( T t ) const
      {
        return static_cast< std::size_t >( t );
      }
    };

    // Use custom hash function for enums, or default hash function for the rest.
    template<typename Key>
    using HashType = typename std::conditional<std::is_enum<Key>::value, EnumClassHash, std::hash<Key>>::type;

    // The hash table type.
    template<typename Key, typename T, typename ... ExtraParams>
    using HashTable = std::unordered_map<Key, T, HashType<Key>, ExtraParams ...>;

    template<typename Key, typename T, typename ... ExtraParams>
    using MultiHashTable = std::unordered_multimap<Key, T, HashType<Key>, ExtraParams ...>;

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    ///
    /// \brief Returns the address of the function encapsulated in a std::function object.
    template<typename T>
    static size_t GetFPAddress( std::function<void( T& )> function )
    {
      using fType = void( T& );
      fType** fp = function.template target<fType*>();
      return reinterpret_cast< size_t >( fp );
    }

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    // Oscillate between a range of [0, 1].
    inline float LORE_EXPORT Pulse( const float time, const float freq )
    {
      return 0.5f * ( 1.0f + glm::sin( 2.0f * glm::pi<float>() * freq * time ) );
    }

}}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
