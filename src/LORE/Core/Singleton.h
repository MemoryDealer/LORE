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

#pragma warning( disable: 4251 )
#pragma warning( disable: 4311 )
#pragma warning( disable: 4312 )
#pragma warning( disable: 4661 )

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

    ///
    /// \class Singleton
    /// \brief Generic singleton class for any singletons to inherit from.
    template<typename T>
    class LORE_EXPORT Singleton
    {

    protected:

      static std::unique_ptr<T> _instance;

    public:

      Singleton() = default;
      virtual ~Singleton() = default;

      ///
      /// \brief Allocates the singleton.
      static void Initialize()
      {
          _instance = std::make_unique<T>();
      }

      ///
      /// \brief Frees the singleton from memory.
      static void Destroy()
      {
          _instance.reset();
      }

      static T& Get()
      {
          return *_instance;
      }

      static T* GetPtr()
      {
          return _instance;
      }

    };

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

#ifdef LORE_PLATFORM_POSIX
    template<typename T>
    std::unique_ptr<T> Singleton<T>::_instance = nullptr;
#endif

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
