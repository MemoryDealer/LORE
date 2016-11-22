-- Premake5 script 

targetdir "../bin/%{cfg.buildcfg}/Run"
objdir "../bin/%{cfg.buildcfg}/Obj"

includedirs { ".", "%{prj.location}", "%{prj.location}/_deps" }

debugdir "../bin/%{cfg.buildcfg}/Run"

-- Configurations

filter "configurations:Debug"
    defines { "DEBUG" }
    flags { "Symbols" }
    
filter "configurations:Release"
    defines { "NDEBUG" }
    optimize "On"
    
filter "configurations:*32"
    libdirs { "../lib/x86/%{cfg.buildcfg}" }
    
filter "configurations:*64"
    libdirs { "../lib/x64/%{cfg.buildcfg}" }
    
-- Solution

solution "LORE2D"
    configurations { "Debug", "Release" }
    startproject "Driver"
    
-- Project(s)
project "LORE2D"
    location "LORE2D"
    kind "SharedLib"
    language "C++"
    files {
        "LORE2D/**.h", "LORE2D/**.cpp"
    }
	links { "glfw3dll.lib" }	
	postbuildcommands { "xcopy ..\\lib\\SDL\\x86\\*.dll ..\\bin\\%{cfg.buildcfg}\\Run\\ /Y",
                        "if not exist ..\\bin\\%{cfg.buildcfg}\\Run\\res\\nul (xcopy ..\\res\\* ..\\bin\\%{cfg.buildcfg}\\Run\\res\\ /e)" }

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