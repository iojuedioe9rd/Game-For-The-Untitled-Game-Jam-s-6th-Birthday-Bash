project "Game"
    kind "ConsoleApp"
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")
    language "C++"
    files 
    { 
        "src/**.h",
        "src/**.cpp",
        "src/**.c",
    }
    
    -- Include external directories
    includedirs 
    {
		"src",
		"%{wks.location}/Engine/src",
		"%{wks.location}/Engine/vendor/glm",
		"%{wks.location}/Engine/vendor/sdl3/include",
		"%{wks.location}/Engine/vendor/ecs/include",
		"%{wks.location}/Engine/vendor/box2d/include"
    }
    
    -- Link dependencies
    links
    {
		"Engine"
    }

    
    -- Handle platform-specific settings
    filter "system:Windows"
        system "windows"
        links
        {
            "%{wks.location}/Engine/vendor/sdl3/Windows/lib/x64/SDL3.lib"
        }

	filter { "platforms:DLL" }
		defines { "USE_DLL" }

    -- Debug configuration settings
    filter { "configurations:Debug" }
        defines { "DEBUG" }
        symbols "On"

    -- Release configuration settings
    filter { "configurations:Release" }
        defines { "NDEBUG" }
        optimize "On"
        symbols "On"

    -- Distribution configuration settings
    filter { "configurations:Dist" }
        defines { "NDEBUG" }
        optimize "On"
        symbols "Off"
