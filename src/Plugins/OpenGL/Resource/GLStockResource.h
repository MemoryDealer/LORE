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

#include <LORE/Resource/StockResource.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore { namespace OpenGL {

  class GLStockResource2DFactory final : public Lore::StockResourceFactory
  {

  public:

    GLStockResource2DFactory( ResourceControllerPtr controller );
    ~GLStockResource2DFactory() override = default;

    GPUProgramPtr createUberProgram( const string& name, const Lore::UberProgramParameters& params ) override;
    GPUProgramPtr createSkyboxProgram( const string& name, const SkyboxProgramParameters& params ) override;
    GPUProgramPtr createEnvironmentMappingProgram( const string& name, const EnvironmentMappingProgramParameters& params ) override;
    GPUProgramPtr createBoxProgram( const string& name ) override;

  };

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  class GLStockResource3DFactory final : public Lore::StockResourceFactory
  {

  public:

    GLStockResource3DFactory( ResourceControllerPtr controller );
    ~GLStockResource3DFactory() override = default;

    GPUProgramPtr createUberProgram( const string& name, const Lore::UberProgramParameters& params ) override;
    GPUProgramPtr createSkyboxProgram( const string& name, const SkyboxProgramParameters& params ) override;
    GPUProgramPtr createEnvironmentMappingProgram( const string& name, const EnvironmentMappingProgramParameters& params ) override;
    GPUProgramPtr createBoxProgram( const string& name ) override;

  };

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  class GLStockResourceController final : public Lore::StockResourceController
  {

  public:

    GLStockResourceController();
    ~GLStockResourceController() override = default;

    virtual GPUProgramPtr createTextProgram( const string& name ) override;

  };

}}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
