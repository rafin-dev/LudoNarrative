workspace "LudoNarrative"
    architecture "x64"

    startproject "Sandbox"
    
    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["Vendor"] = "%{wks.location}/LudoNarrative/vendor" 
IncludeDir["ImGui"] = "%{wks.location}/LudoNarrative/vendor/imgui"

include "imgui_premake5"

project "LudoNarrative"
    location "LudoNarrative"
    kind "StaticLib"
    language "C++"
    cppdialect "c++20"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "ldpch.h"
    pchsource "LudoNarrative/src/ldpch.cpp"

    files
    {
        "%{prj.location}/src/**.h",
        "%{prj.location}/src/**.cpp"
    }

    includedirs
    {
        "%{prj.location}/src",
        "%{prj.location}/vendor/spdlog/include",
        "%{IncludeDir.Vendor}",
        "%{IncludeDir.ImGui}"
    }

    links
    {
        "ImGui"
    }

    filter "system:windows"
        systemversion "latest"
        defines "LD_PLATFORM_WINDOWS"

    filter "configurations:Debug"
        defines "LUDO_DEBUG"
        symbols "on"
        runtime "Debug"

        defines { "LD_ENABLE_ASSERTS" }

    filter "configurations:Release"
        defines "LUDO_RELEASE"
        optimize "on"
        runtime "Release"

    filter "configurations:Dist"
        defines "LUDO_DIST"
        optimize "on"
        runtime "Release"

project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.location}/src/**.h",
        "%{prj.location}/src/**.cpp"
    }

    includedirs
    {
        "%{prj.location}/src",
        "%{wks.location}/LudoNarrative/src",
        "%{wks.location}/LudoNarrative/vendor/spdlog/include",
        "%{IncludeDir.Vendor}"
    }

    defines
    {
        "LD_PLATFORM_WINDOWS"
    }

    links
    {
        "LudoNarrative"
    }

    cppdialect "c++20"
    staticruntime "on"
    systemversion "latest"

    filter "configurations:Debug"
        defines "LUDO_DEBUG"
        symbols "On"
        runtime "Debug"
        defines { "LD_ENABLE_ASSERTS" }

    filter "configurations:Release"
        defines "LUDO_RELEASE"
        optimize "On"
        runtime "Release"

    filter "configurations:Dist"
        defines "LUDO_DIST"
        optimize "On"
        runtime "Release"