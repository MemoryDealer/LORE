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
    startproject "Example_ComplexScene"

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

-- Use latest C++ standard for all projects (using _HAS_CXX17 for now).
--buildoptions { "/std:c++latest" }

--
-- Projects

startproject "Demo3D"

project "LORE"
    location "LORE"
    kind "SharedLib"
    language "C++"
    defines { "__Lore_Exports__", "_HAS_CXX17" }
    pchheader "_pch.h"
    pchsource "LORE/_pch.cpp"
    forceincludes { "_pch.h" }
    files {
        "LORE/**.h", "LORE/**.cpp", "LORE/**.inl"
    }
    postbuildcommands { "{COPY} ../../res/ ../../bin/%{cfg.buildcfg}/Run/res/" }

project "Plugin_OpenGL"
    location "Plugins/OpenGL"
    kind "SharedLib"
    language "C++"
    libdirs { "../bin/%{cfg.buildcfg}/Run" } -- Location of LORE.lib after build.
    defines { "__LoreOpenGL_Exports__", "_HAS_CXX17" }
    pchheader "_pch.h"
    pchsource "Plugins/OpenGL/_pch.cpp"
    forceincludes { "_pch.h" }
    files {
        "Plugins/OpenGL/**.h", "Plugins/OpenGL/**.cpp"
    }
    links { "LORE", "glad", "freetype" }

    -- OS-specific options
    filter { "system:Windows" }
        links { "glfw3dll" }

    filter { "system:MacOSX" }
        links { "glfw3" }
        linkoptions { "-framework Cocoa -framework IOKit -framework CoreVideo -framework OpenGL" }

    postbuildcommands { "{COPY} ../../../lib/x64/%{cfg.buildcfg}/ ../../../bin/%{cfg.buildcfg}/Run/" }
    postbuildcommands { "{COPY} ../../../res/ ../../../bin/%{cfg.buildcfg}/Run/res/" }

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
    defines { "_HAS_CXX17" }
    files {
        "Examples/Demo3D/**.h",
        "Examples/Demo3D/**.cpp"
    }
    links { "LORE" }
    postbuildcommands { "{COPY} ../../../res/ ../../../bin/%{cfg.buildcfg}/Run/res/" }

project "Example_ComplexScene2D"
    location "Examples/ComplexScene2D"
    kind "ConsoleApp"
    language "C++"
    defines { "_HAS_CXX17" }
    files { 
        "Examples/ComplexScene2D/**.h",
        "Examples/ComplexScene2D/**.cpp"
    }
    links { "LORE" }
    postbuildcommands { "{COPY} ../../../res/ ../../../bin/%{cfg.buildcfg}/Run/res/" }

project "Example_ComplexScene3D"
    location "Examples/ComplexScene3D"
    kind "ConsoleApp"
    language "C++"
    defines { "_HAS_CXX17" }
    files { 
        "Examples/ComplexScene3D/**.h",
        "Examples/ComplexScene3D/**.cpp"
    }
    links { "LORE" }
    postbuildcommands { "{COPY} ../../../res/ ../../../bin/%{cfg.buildcfg}/Run/res/" }

--
-- Tests

project "Tests"
    location "Tests"
    kind "ConsoleApp"
    language "C++"
    defines { "_HAS_CXX17" }
    forceincludes { "TestUtils.h" }
    files {
        "Tests/**.h", "Tests/**.hpp", "Tests/**.cpp", "Tests/**.json"
    }
    links { "LORE" }
    postbuildcommands { "{COPY} data/ ../../bin/%{cfg.buildcfg}/Run/TestData/" }
