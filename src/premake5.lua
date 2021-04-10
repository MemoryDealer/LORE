----------------------------
-- LORE Premake5 script --
----------------------------

targetdir "../bin/%{cfg.buildcfg}/Run"
objdir "../bin/%{cfg.buildcfg}/Obj"
debugdir "../bin/%{cfg.buildcfg}/Run"

includedirs { ".",
              "%{prj.location}",
              "%{sln.location}/External",
              "%{sln.location}/External/assimp/include",
              "%{sln.location}/External/freetype2",
              "%{sln.location}/External/glm",
              "%{sln.location}/External/imgui",
              "%{sln.location}/External/rapidjson/include",
              "%{sln.location}/Plugins/ThirdParty"
            }
libdirs { "../lib/x64/%{cfg.buildcfg}" }
architecture "x86_64"
characterset ( "MBCS" )

--
-- Solution

solution "LORE"
    configurations { "Debug", "Release" }
    startproject "Demo3D"

--
-- Configurations

filter "configurations:Debug"
    defines { "DEBUG" }
    symbols "On"
    
filter "configurations:Release"
    defines { "NDEBUG" }
    optimize "On"

--
-- Systems

filter "system:MacOSX"
    cppdialect "C++14"

--
-- Projects

project "LORE"
    location "LORE"
    kind "SharedLib"
    language "C++"
    defines { "__Lore_Exports__", "_HAS_CXX17", "_HAS_CXX20", "LORE_DEBUG_UI" }
    buildoptions { "/std:c++latest" }
    pchheader "_pch.h"
    pchsource "LORE/_pch.cpp"
    forceincludes { "_pch.h" }
    files {
        "LORE/**.h", "LORE/**.cpp", "LORE/**.inl",
        "External/imgui/*.h", "External/imgui/*.cpp"
    }
    links { "assimp-vc140-mt" }
    postbuildcommands { "{COPY} ../../lib/x64/%{cfg.buildcfg}/ ../../bin/%{cfg.buildcfg}/Run/" }

project "Plugin_OpenGL"
    location "Plugins/OpenGL"
    kind "SharedLib"
    language "C++"
    libdirs { "../bin/%{cfg.buildcfg}/Run" } -- Location of LORE.lib after build.
    defines { "__LoreOpenGL_Exports__", "_HAS_CXX17", "_HAS_CXX20", "LORE_DEBUG_UI" }
    buildoptions { "/std:c++latest" }
    pchheader "_pch.h"
    pchsource "Plugins/OpenGL/_pch.cpp"
    forceincludes { "_pch.h" }
    files {
        "Plugins/OpenGL/**.h", "Plugins/OpenGL/**.cpp",
        "External/imgui/*.h", "External/imgui/*.cpp"
    }
    links { "LORE", "glad", "freetype" }

    -- OS-specific options
    filter { "system:Windows" }
        links { "glfw3dll" }

    filter { "system:MacOSX" }
        links { "glfw3" }
        linkoptions { "-framework Cocoa -framework IOKit -framework CoreVideo -framework OpenGL" }

    postbuildcommands { "{COPY} ../../../lib/x64/%{cfg.buildcfg}/ ../../../bin/%{cfg.buildcfg}/Run/" }

project "glad"
    location "Plugins/ThirdParty/glad"
    kind "StaticLib"
    language "C"
    files {
        "Plugins/ThirdParty/glad/*.h", "Plugins/ThirdParty/glad/*.c"
    }

--
-- Examples

project "Demo3D"
    location "Examples/Demo3D"
    kind "ConsoleApp"
    language "C++"
    defines { "_HAS_CXX17", "_HAS_CXX20" }
    buildoptions { "/std:c++latest" }
    files {
        "Examples/Demo3D/**.h",
        "Examples/Demo3D/**.cpp"
    }
    links { "LORE" }

project "Example_ComplexScene2D"
    location "Examples/ComplexScene2D"
    kind "ConsoleApp"
    language "C++"
    defines { "_HAS_CXX17", "_HAS_CXX20" }
    buildoptions { "/std:c++latest" }
    files { 
        "Examples/ComplexScene2D/**.h",
        "Examples/ComplexScene2D/**.cpp"
    }
    links { "LORE" }

project "Example_ComplexScene3D"
    location "Examples/ComplexScene3D"
    kind "ConsoleApp"
    language "C++"
    defines { "_HAS_CXX17", "_HAS_CXX20" }
    buildoptions { "/std:c++latest" }
    files { 
        "Examples/ComplexScene3D/**.h",
        "Examples/ComplexScene3D/**.cpp"
    }
    links { "LORE" }

--
-- Tests

project "Tests"
    location "Tests"
    kind "ConsoleApp"
    language "C++"
    defines { "_HAS_CXX17", "_HAS_CXX20" }
    buildoptions { "/std:c++latest" }
    forceincludes { "TestUtils.h" }
    files {
        "Tests/**.h", "Tests/**.hpp", "Tests/**.cpp", "Tests/**.json"
    }
    links { "LORE" }
