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

// Platform define:

#define LORE_WINDOWS 0
#define LORE_LINUX 1
#define LORE_APPLE 2

#if defined( WIN32 ) || defined( _WIN32 )
#define LORE_PLATFORM LORE_WINDOWS
#elif defined( LINUX ) || defined( _LINUX )
#define LORE_PLATFORM LORE_LINUX
#elif defined( __APPLE__ ) || defined( __OSX__ )
#define LORE_PLATFORM LORE_APPLE
#endif

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

// C/C++/STL.
#include <algorithm>
#include <atomic>
#include <cassert>
#include <cctype>
#include <chrono>
#include <condition_variable>
#include <ctime>
#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <sstream>
#include <stack>
#include <string>
#include <thread>
#include <typeindex>
#include <typeinfo>
#include <queue>
#include <unordered_map>

#if LORE_PLATFORM == LORE_APPLE
#include "../External/variant/include/mpark/variant.hpp"
#else
#include <variant>
#endif

#include <vector>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

// Windows.
#if LORE_PLATFORM == LORE_WINDOWS
#define NOMINMAX
#include <Windows.h>
#endif

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

// Lore.
#include "Exports.h"
#include "Types.h"
#include "Core/ClassMacros.h"
#include "Core/Logging/Log.h"
#include "Core/Exception.h"
#include "Memory/MemoryAccess.h"

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
