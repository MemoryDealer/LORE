----------------------------
-- LORE2D Premake5 script --
----------------------------

targetdir "../bin/%{cfg.buildcfg}/Run"
objdir "../bin/%{cfg.buildcfg}/Obj"
debugdir "../bin/%{cfg.buildcfg}/Run"

includedirs { ".", "%{prj.location}", "%{sln.location}/Plugins/ThirdParty", "%{sln.location}/External",
               "%{sln.location}/External/freetype2" }
libdirs { "../lib/x64/%{cfg.buildcfg}" }
architecture "x86_64"
characterset ( "MBCS" )

    
-- Solution

solution "LORE2D"
    configurations { "Debug", "Release" }
    startproject "Driver"
    
    
-- Configurations

filter "configurations:Debug"
    defines { "DEBUG" }
    symbols "On"
    
filter "configurations:Release"
    defines { "NDEBUG" }
    optimize "On"

-- Use latest C++ standard for all projects
buildoptions { "/std:c++latest" }
    
-- Project(s)

project "LORE2D"
    location "LORE2D"
    kind "SharedLib"
    language "C++"
    defines { "__Lore_Exports__" }
    pchheader "_pch.h"
    pchsource "LORE2D/_pch.cpp"
    forceincludes { "_pch.h" }
    files {
        "LORE2D/**.h", "LORE2D/**.cpp"
    }

    
project "Plugin_OpenGL"
    location "Plugins/OpenGL"
    kind "SharedLib"
    language "C++"
    libdirs { "../bin/%{cfg.buildcfg}/Run" } -- Location of LORE2D.lib after build.
    defines { "__LoreOpenGL_Exports__" }
    pchheader "_pch.h"
    pchsource "Plugins/OpenGL/_pch.cpp"
    forceincludes { "_pch.h" }
    files {
        "Plugins/OpenGL/**.h", "Plugins/OpenGL/**.cpp",
    }
    links { "LORE2D", "glfw3dll", "glad", "freetype" }
    postbuildcommands { "xcopy ..\\..\\..\\lib\\x64\\%{cfg.buildcfg}\\*.dll ..\\..\\..\\bin\\%{cfg.buildcfg}\\Run\\ /Y" }
    
project "glad"
    location "Plugins/ThirdParty/glad"
    kind "StaticLib"
    language "C"
    files { 
        "Plugins/ThirdParty/glad/*.h", "Plugins/ThirdParty/glad/*.c"
    }

project "Driver"
    location "Driver"
    kind "ConsoleApp"
    language "C++"
    files {
        "Driver/**.cpp",
        "Driver/TODO.TXT"
    }
    links { "LORE2D" }
    
project "UnitTests"
    location "UnitTests"
    kind "ConsoleApp"
    language "C++"
    forceincludes { "UnitTests.h" }
    files {
        "UnitTests/**.h", "UnitTests/**.hpp", "UnitTests/**.cpp"
    }
    links { "LORE2D" }