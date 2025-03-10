workspace "DIM"
	configurations { "DEBUG", "RELEASE" }
	startproject "DIM-Lang"

project "DIM-Lang"
	language "C++"
	kind "ConsoleApp"
	location "build/"
	
	files {
		"src/main.cpp",
		"src/dim/**/*.cpp",
		"include/dim/**/*.hpp",
	}

	includedirs {
		"include/"
	}

	filter { "configurations:DEBUG" }
		defines { "DEBUG" }
