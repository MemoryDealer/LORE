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
    /// \typedef Colori Represents color as 4D vector (r, g, b, a).
    /// Uses ints with range of [0, 255].
    using Color4i = glm::tvec4<uint32_t>;


    ///
    /// \typedef Color4f Represents color as 4D vector (r, g, b, a).
    /// Uses floats with range of [0.f, 1.f].
    using Color4f = glm::vec4;

    ///
    /// \typedef Color Represents color as 4D vector (r, g, b, a).
    /// Uses floats with range of [0.f, 1.f].
    using Color = Color4f; // Default colors to floats.

    // Various common colors.
    namespace StockColor {

        static const Color Black    = Color( 0.f, 0.f, 0.f, 1.f );
        static const Color White    = Color( 1.f, 1.f, 1.f, 1.f );
        static const Color Red      = Color( 1.f, 0.f, 0.f, 1.f );
        static const Color Green    = Color( 0.f, 1.f, 0.f, 1.f );
        static const Color Blue     = Color( 0.f, 0.f, 1.f, 1.f );

    }

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
