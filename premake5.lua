VENDOR_INCLUDES = { "vendor/glm", "vendor/LuaJIT/src" }
VENDOR_LINKS = {
	all = {},
	debug = {"vendor/LuaJIT/src/lua51d.lib"},
	release = {"vendor/LuaJIT/src/lua51r.lib"}
}

project "Vespertine"
	kind "StaticLib"
	language "C++"
	targetdir "lib/%{cfg.buildcfg}"

	includedirs(VENDOR_INCLUDES)
	includedirs { "include/" }
	files { "include/**.hpp", "include/**.h", "src/**.cpp" }
	flags { "FatalWarnings", "MultiProcessorCompile" }
	defines { "NOMINMAX", "_USE_MATH_DEFINES", "Symbols", "SOL_NO_EXCEPTIONS", "SOL_NO_RTTI" }
	exceptionhandling "Off"
	rtti "Off"

	filter "configurations:Debug"
		defines { "DEBUG", "VESP_ASSERT_ENABLED", "_ITERATOR_DEBUG_LEVEL=0" }
		flags { "Symbols" }

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"

	configuration { "vs*" }
		editandcontinue "off"
		buildoptions { "/EHsc", "/Ob2" }

		prebuildcommands {
			[[pushd "vespertine/vendor/LuaJIT/src"]],
			[[If Not Exist "lua51d.lib" (call msvcbuild.bat debug   static && copy lua51.lib lua51d.lib)]],
			[[If Not Exist "lua51r.lib" (call msvcbuild.bat release static && copy lua51.lib lua51r.lib)]]
		}

	configuration { "gmake" }
		buildoptions { "-std=c++11" }