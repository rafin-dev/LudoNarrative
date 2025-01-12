workspace "LudoNarrative"
    architecture "x64"

    startproject "Ludo-Editor"
    
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
IncludeDir["stb_image"] = "%{wks.location}/LudoNarrative/vendor/stb_image"
IncludeDir["dxc"] = "%{wks.location}/LudoNarrative/vendor/dxc"
IncludeDir["entt"] = "%{wks.location}/LudoNarrative/vendor/entt/include"

include "imgui_premake5"

project "LudoNarrative"
    location "LudoNarrative"
    kind "StaticLib"
    language "C++"
    cppdialect "c++20"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "ldpch.h"
    pchsource "LudoNarrative/src/ldpch.cpp"

    files
    {
        "%{prj.location}/src/**.h",
        "%{prj.location}/src/**.cpp",
        "%{prj.location}/vendor/stb_image/**.h",
        "%{prj.location}/vendor/stb_image/**.cpp",
        "%{prj.location}/vendor/dxc/inc/**.h"
    }

    includedirs
    {
        "%{prj.location}/src",
        "%{prj.location}/vendor/spdlog/include",
        "%{IncludeDir.Vendor}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.stb_image}",
        "%{IncludeDir.entt}"
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
        optimize "off"
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
        "%{IncludeDir.Vendor}",
        "%{IncludeDir.entt}"
    }

    defines
    {
        "LD_PLATFORM_WINDOWS"
    }

    links
    {
        "LudoNarrative"
    }

    postbuildcommands { "{COPYFILE} %{wks.location}/LudoNarrative/vendor/dxc/bin/x64/dxcompiler.dll %{cfg.buildtarget.directory}", "{COPYFILE} %{wks.location}/LudoNarrative/vendor/dxc/bin/x64/dxil.dll %{cfg.buildtarget.directory}" }

    cppdialect "c++20"
    systemversion "latest"

    filter "configurations:Debug"
        defines "LUDO_DEBUG"
        symbols "on"
        optimize "off"
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
        kind "WindowedApp"
    
project "Ludo-Editor"
    location "Ludo-Editor"
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
        "%{wks.location}/LudoNarrative/vendor/spdlog/include",
        "%{IncludeDir.Vendor}",
        "%{IncludeDir.entt}"
    }

    defines
    {
        "LD_PLATFORM_WINDOWS"
    }

    links
    {
        "LudoNarrative"
    }

    postbuildcommands { "{COPYFILE} %{wks.location}/LudoNarrative/vendor/dxc/bin/x64/dxcompiler.dll %{cfg.buildtarget.directory}", "{COPYFILE} %{wks.location}/LudoNarrative/vendor/dxc/bin/x64/dxil.dll %{cfg.buildtarget.directory}" }

    cppdialect "c++20"
    systemversion "latest"

    filter "configurations:Debug"
        defines "LUDO_DEBUG"
        symbols "on"
        optimize "off"
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
        kind "WindowedApp"