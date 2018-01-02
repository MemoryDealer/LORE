----------------------------
-- LORE2D Premake5 script --
----------------------------

targetdir "../bin/%{cfg.buildcfg}/Run"
objdir "../bin/%{cfg.buildcfg}/Obj"
debugdir "../bin/%{cfg.buildcfg}/Run"

includedirs { ".", "%{prj.location}", "%{sln.location}/Plugins/ThirdParty", "%{sln.location}/External",
               "%{sln.location}/External/freetype2", "%{sln.location}/External/rapidjson/include" }
libdirs { "../lib/x64/%{cfg.buildcfg}" }
architecture "x86_64"
characterset ( "MBCS" )


-- Solution

solution "LORE2D"
    configurations { "Debug", "Release" }
    startproject "Example_ComplexScene"


-- Configurations

filter "configurations:Debug"
    defines { "DEBUG" }
    symbols "On"
    
filter "configurations:Release"
    defines { "NDEBUG" }
    optimize "On"

-- Systems
filter "system:MacOSX"
    cppdialect "C++14"

-- Use latest C++ standard for all projects (using _HAS_CXX17 for now).
--buildoptions { "/std:c++latest" }

--
-- Project(s)
--

project "LORE2D"
    location "LORE2D"
    kind "SharedLib"
    language "C++"
    defines { "__Lore_Exports__", "_HAS_CXX17" }
    pchheader "_pch.h"
    pchsource "LORE2D/_pch.cpp"
    forceincludes { "_pch.h" }
    files {
        "LORE2D/**.h", "LORE2D/**.cpp"
    }
    postbuildcommands { "{COPY} ../../res/ ../../bin/%{cfg.buildcfg}/Run/res/" }

project "Plugin_OpenGL"
    location "Plugins/OpenGL"
    kind "SharedLib"
    language "C++"
    libdirs { "../bin/%{cfg.buildcfg}/Run" } -- Location of LORE2D.lib after build.
    defines { "__LoreOpenGL_Exports__", "_HAS_CXX17" }
    pchheader "_pch.h"
    pchsource "Plugins/OpenGL/_pch.cpp"
    forceincludes { "_pch.h" }
    files {
        "Plugins/OpenGL/**.h", "Plugins/OpenGL/**.cpp"
    }
    links { "LORE2D", "glad", "freetype" }

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
--

project "Example_ComplexScene"
    location "Examples/ComplexScene"
    kind "ConsoleApp"
    language "C++"
    defines { "_HAS_CXX17" }
    files { 
        "Examples/ComplexScene/**.h",
        "Examples/ComplexScene/**.cpp"
    }
    links { "LORE2D" }
    postbuildcommands { "{COPY} ../../../res/ ../../../bin/%{cfg.buildcfg}/Run/res/" }

project "Driver"
    location "Driver"
    kind "ConsoleApp"
    language "C++"
    defines { "_HAS_CXX17" }
    files {
        "Driver/**.cpp",
        "Driver/TODO.TXT"
    }
    links { "LORE2D" }
    postbuildcommands { "{COPY} ../../res/ ../../bin/%{cfg.buildcfg}/Run/res/" }

project "UnitTests"
    location "UnitTests"
    kind "ConsoleApp"
    language "C++"
    defines { "_HAS_CXX17" }
    forceincludes { "UnitTests.h" }
    files {
        "UnitTests/**.h", "UnitTests/**.hpp", "UnitTests/**.cpp", "UnitTests/**.json"
    }
    links { "LORE2D" }
    postbuildcommands { "{COPY} data/ ../../bin/%{cfg.buildcfg}/Run/TestData/" }
