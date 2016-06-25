VENDOR_INCLUDES = { "vendor/glm" }
VENDOR_LINKS = { }

project "Vespertine"
	kind "StaticLib"
	language "C++"
	targetdir "lib/%{cfg.buildcfg}"

	includedirs(VENDOR_INCLUDES)
	includedirs { "include/" }
	files { "include/**.hpp", "include/**.h", "src/**.cpp" }
	flags { "FatalWarnings", "MultiProcessorCompile" }
	defines { "NOMINMAX", "_USE_MATH_DEFINES", "Symbols" }
	exceptionhandling "Off"
	rtti "Off"

	filter "configurations:Debug"
		defines { "DEBUG", "VESP_ASSERT_ENABLED" }
		flags { "Symbols" }

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"

	configuration { "vs*" }
		editandcontinue "off"
		buildoptions { "/EHsc", "/Ob2" }

	configuration { "gmake" }
		buildoptions { "-std=c++11" }