-- Premake5 script 

targetdir "../bin/%{cfg.buildcfg}/Run"
objdir "../bin/%{cfg.buildcfg}/Obj"

includedirs { ".", "%{prj.location}", "%{prj.location}/ThirdParty" }

debugdir "../bin/%{cfg.buildcfg}/Run"

-- Configurations

defines { "DEBUG_TOOLS" }

filter "configurations:Debug"
    defines { "DEBUG" }
    flags { "Symbols" }
	libdirs { "../lib/SDL/x86" }
    
filter "configurations:Release"
    defines { "NDEBUG" }
    optimize "On"
	libdirs { "../lib/SDL/x86" }
    
-- Solution

solution "LORE2D"
    configurations { "Debug", "Release" }
    startproject "LORE2D"
    
-- Project(s)
project "LORE2D"
    location "."
    kind "SharedLib"
    language "C++"
    files {
    "*.h", "*.hpp"
    }
	links { "SDL2.lib", "SDL2main.lib", "SDL2_image.lib", "SDL2_ttf.lib", "SDL2_mixer.lib" }	
	postbuildcommands { "xcopy ..\\lib\\SDL\\x86\\*.dll ..\\bin\\%{cfg.buildcfg}\\Run\\ /Y",
                        "if not exist ..\\bin\\%{cfg.buildcfg}\\Run\\res\\nul (xcopy ..\\res\\* ..\\bin\\%{cfg.buildcfg}\\Run\\res\\ /e)" }
