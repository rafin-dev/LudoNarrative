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
IncludeDir["ImGui"] = "%{wks.location}/LudoNarrative/vendor/imgui"

include "imgui_premake5"

project "LudoNarrative"
    location "LudoNarrative"
    kind "SharedLib"
    language "C++"

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
        "%{IncludeDir.ImGui}"
    }

    defines
    {
        -- Purple Engine is planned to be windows only, but it doenst hurt to be ready just in case
        "LD_PLATFORM_WINDOWS",
        "LD_BUILD_DLL"
    }

    links
    {
        "ImGui"
    }

    postbuildcommands
    {
        ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
    }

    cppdialect "c++20"
    staticruntime "on"
    systemversion "latest"

    filter "configurations:Debug"
        defines "PRPL_DEBUG"
        symbols "On"
        staticruntime "off"
        runtime "Debug"

    filter "configurations:Release"
        defines "PRPL_RELEASE"
        optimize "On"
        staticruntime "off"
        runtime "Release"

    filter "configurations:Dist"
        defines "PRPL_DIST"
        optimize "On"
        staticruntime "off"
        runtime "Release"

project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"

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
        "%{wks.location}/LudoNarrative/vendor/spdlog/include"
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
        defines "PRPL_DEBUG"
        symbols "On"
        staticruntime "off"
        runtime "Debug"

    filter "configurations:Release"
        defines "PRPL_RELEASE"
        optimize "On"
        staticruntime "off"
        runtime "Release"

    filter "configurations:Dist"
        defines "PRPL_DIST"
        optimize "On"
        staticruntime "off"
        runtime "Release"