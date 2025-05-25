workspace "Game"
    configurations { "Debug", "Release", "Dist" }
    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
	architecture "x86_64"
	platforms { "Static", "DLL" }
	cppdialect "C++20"
	defines
	{
		"STBIDEF=extern ENGINE_API"
	}

project "ImGui"
	location "Engine/vendor/ImGui"
	kind "StaticLib"
	language "C++"
    staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"Engine/vendor/ImGui/imconfig.h",
		"Engine/vendor/ImGui/imgui.h",
		"Engine/vendor/ImGui/imgui.cpp",
		"Engine/vendor/ImGui/imgui_draw.cpp",
		"Engine/vendor/ImGui/imgui_internal.h",
		"Engine/vendor/ImGui/imgui_tables.cpp",
		"Engine/vendor/ImGui/imgui_widgets.cpp",
		"Engine/vendor/ImGui/imstb_rectpack.h",
		"Engine/vendor/ImGui/imstb_textedit.h",
		"Engine/vendor/ImGui/imstb_truetype.h",
		"Engine/vendor/ImGui/imgui_demo.cpp"
	}

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"

	filter "system:linux"
		pic "On"
		systemversion "latest"
		cppdialect "C++17"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

    filter "configurations:Dist"
		runtime "Release"
		optimize "on"
        symbols "off"

include "Engine/vendor/box2d"
include "Game"
include "Engine"