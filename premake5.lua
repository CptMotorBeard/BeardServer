function defaultConfigurations()
	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

	filter "configurations:Debug"
		defines "BS_DEBUG"
		symbols "On"
		
	filter "configurations:OptDebug"
		defines "BS_OPTDEBUG"
		optimize "On"
		
	filter "configurations:Distribution"
		defines "BS_DIST"
		optimize "On"
end

workspace "BeardServer"
	architecture "x64"
	
	configurations
	{
		"Debug",
		"OptDebug",
		"Distribution"
	}
	
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
vendorPath = "BeardServer/vendor"

project "BeardServer"
	location "BeardServer/project"
	kind "StaticLib"
	language "C++"
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/src/**.cs"
	}
	
	includedirs
	{
		vendorPath .. "/nlohmann/json/include",
		"%{prj.name}/src/"
	}
	
	pchheader "pch.h"
	pchsource "%{prj.name}/src/pch.cpp"
	
	defaultConfigurations()