----------------------------
-- LORE2D Premake5 script --
----------------------------

targetdir "../bin/%{cfg.buildcfg}/Run"
objdir "../bin/%{cfg.buildcfg}/Obj"
debugdir "../bin/%{cfg.buildcfg}/Run"

includedirs { ".", "%{prj.location}", "%{sln.location}/_deps" }
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
    flags { "Symbols" }
    
filter "configurations:Release"
    defines { "NDEBUG" }
    optimize "On"
    
    
-- Project(s)

project "LORE2D"
    location "LORE2D"
    kind "SharedLib"
    language "C++"
    defines { "__Lore_Exports__" }
    pchheader "_pch.h"
    pchsource "_pch.cpp"
    buildoptions { "/Yc_pch.h" }
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
    pchsource "_pch.cpp"
    buildoptions { "/Yc_pch.h" }
    forceincludes { "_pch.h" }
    files {
        "Plugins/OpenGL/**.h", "Plugins/OpenGL/**.cpp"
    }
    links { "LORE2D", "glfw3dll" }
    postbuildcommands { "xcopy ..\\..\\..\\lib\\x64\\%{cfg.buildcfg}\\*.dll ..\\..\\..\\bin\\%{cfg.buildcfg}\\Run\\ /Y" }

project "Driver"
    location "Driver"
    kind "ConsoleApp"
    language "C++"
    files {
        "Driver/**.cpp"
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