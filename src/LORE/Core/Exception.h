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

namespace Lore {

    ///
    /// \class Exception
    /// \brief The core Lore exception object. Specialized exceptions should
    /// derive from this class.
    class Exception : public std::exception
    {

    protected:

        string _what = "Unknown exception";
        
    public:

        explicit Exception( const string& what )
        : _what( what )
        {
          log_error( "[EXCEPTION] " + what );
        }

        virtual string getDescription() const
        {
          return _what;
        }

    };

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    ///
    /// \class ItemIdentityException
    /// \brief Thrown when attempting to access an item by name that doesn't exist.
    class ItemIdentityException : public Exception
    {

    public:

        explicit ItemIdentityException( const string& what )
        : Exception( what )
        {
        }

        virtual string getDescription() const override
        {
          return string( "ItemIdentityException: " + _what );
        }

    };

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    /// 
    /// \class MemoryException 
    /// \brief Thrown when an error occurs in LORE's memory management system. 
    class MemoryException : public Exception
    {

    public:

      explicit MemoryException( const string& what )
        : Exception( what )
      {
      }

      virtual string getDescription() const override
      {
        return string( "MemoryException: " + _what );
      }

    };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //