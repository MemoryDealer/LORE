----------------------------
-- LORE2D Premake5 script --
----------------------------

targetdir "../bin/%{cfg.buildcfg}/Run"
objdir "../bin/%{cfg.buildcfg}/Obj"
debugdir "../bin/%{cfg.buildcfg}/Run"

includedirs { ".", "%{prj.location}", "%{prj.location}/_deps" }
libdirs { "../lib/x64/%{cfg.buildcfg}" }
architecture "x86_64"

    
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
    files {
        "LORE2D/**.h", "LORE2D/**.cpp"
    }
	links { "glfw3dll.lib" }	
	postbuildcommands { "xcopy ..\\..\\lib\\x64\\%{cfg.buildcfg}\\*.dll ..\\..\\bin\\%{cfg.buildcfg}\\Run\\ /Y" }

project "Driver"
    location "Driver"
    kind "ConsoleApp"
    language "C++"
    files {
        "Driver/**.cpp"
    }
    
project "UnitTests"
    location "UnitTests"
    kind "ConsoleApp"
    language "C++"
    files {
        "UnitTests/**.h", "UnitTests/**.hpp", "UnitTests/**.cpp"
    }