project "Engine"
	kind "None"
	language "C++"
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")
	
	
	files 
    { 
        "src/**.h",
        "src/**.cpp",
        "src/**.c",
		"Platform/WebGL/**.h",
		"Platform/WebGL/**.cpp",
		"vendor/glm/glm/**.hpp",
		"vendor/glm/glm/**.inl",
		
		"%{wks.location}/Engine/vendor/glad/src/glad.c",
    }
	
	-- Include external directories
    includedirs 
    {
		"src",
		"./",
		"%{wks.location}/Engine/vendor/sdl3/include",
		"%{wks.location}/Engine/vendor/glad/include"
    }
	
	-- Link dependencies
    links
    {
    }
	
    filter { "platforms:Static" }
		kind "StaticLib"

	filter { "platforms:DLL" }
		kind "SharedLib"
		defines { "DLL_EXPORTS", "USE_DLL" }
		
    -- Handle platform-specific settings
    filter "system:Windows"
        system "windows"
        links
        {
            "vendor/sdl3/Windows/lib/x64/SDL3.lib"
        }
		
		files
		{
			"Platform/Windows/**.h",
			"Platform/Windows/**.cpp"
		}
		
		postbuildcommands
		{
		
			("{MKDIR} ../bin/" .. outputdir .. "/Game"),
			
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Game"),
		
		}

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