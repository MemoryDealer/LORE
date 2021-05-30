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

#include <LORE/Serializer/Serializer.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  ///
  /// \class SceneLoader
  /// \brief Loads scene from JSON file format.
  class LORE_EXPORT SceneLoader final
  {

  public:

    SceneLoader() = default;
    ~SceneLoader() = default;

    bool process( const string& sceneFile, ScenePtr scene );

    // If unload is true, all prefabs in scene's resource group will be unloaded first.
    void setUnloadPrefabs( const bool unload );

  private:

    void _loadProperties();
    void _loadPrefabs();
    void _loadLighting();
    void _loadLayout();

    void _processMaterialSettings( const SerializerValue& value, PrefabPtr prefab);
    void _processNode( const string& nodeName, const SerializerValue& nodeData, const NodePtr parent = nullptr );

  private:

    Serializer _serializer {};
    ScenePtr _scene { nullptr };
    string _resourceGroupName {};
    bool _unloadPrefabs { false };

  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
