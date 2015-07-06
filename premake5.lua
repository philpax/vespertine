VENDOR_INCLUDES = { "vendor/glm", "vendor/SDL2-2.0.3/include" }

project "Vespertine"
	kind "StaticLib"
	language "C++"
	targetdir "lib/%{cfg.buildcfg}"

	includedirs(VENDOR_INCLUDES)
	includedirs { "include/" }
	files { "include/**.hpp", "src/**.cpp" }
	flags { "NoExceptions", "NoRTTI", "FatalWarnings", "MultiProcessorCompile" }
	defines { "NOMINMAX", "_USE_MATH_DEFINES" }

	filter "configurations:Debug"
		defines { "DEBUG" }
		flags { "Symbols" }

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"

	configuration { "gmake" }
		buildoptions { "-std=c++11" }