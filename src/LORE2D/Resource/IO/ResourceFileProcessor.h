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

#include <LORE2D/Resource/IO/SerializableResource.h>
#include <LORE2D/Serializer/Serializer.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  class ResourceFileProcessor final
  {

  public:

    static void LoadConfiguration( const string& file, ResourceControllerPtr resourceController );
    static SerializableResource GetResourceFileType( const string& file );

  public:

    ResourceFileProcessor( const string& file, const SerializableResource type );
    ~ResourceFileProcessor() = default;

    void process();

    string getName() const;

    SerializableResource getType() const;

    void load( const string& groupName, ResourceControllerPtr resourceController );

  private:

    void processAnimation( SpriteAnimationSetPtr animationSet, const SerializerValue& animations, ResourceControllerPtr resourceController );
    void processMaterial( MaterialPtr material, const SerializerValue& settings, ResourceControllerPtr resourceController );
    void processSpriteList( const string& groupName, ResourceControllerPtr resourceController );

  private:

    string _file {};
    SerializableResource _type {};
    mutable Serializer _serializer {};

  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
