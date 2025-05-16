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

include "Game"
include "Engine"