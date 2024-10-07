workspace "PuperEngine"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "PurpleEngine"
    location "PurpleEngine"
    kind "SharedLib"
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
        "%{prj.location}/src"
    }

    defines
    {
        -- Purple Engine is planned to be windows only, but it doenst hurt to be ready just in case
        "PRPL_PLATFORM_WINDOWS",
        "PRPL_BUILD_DLL"
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

    filter "configurations:Release"
        defines "PRPL_RELEASE"
        optimize "On"

    filter "configurations:Dist"
        defines "PRPL_DIST"
        optimize "On"

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
        "%{wks.location}/PurpleEngine/src"
    }

    defines
    {
        "PRPL_PLATFORM_WINDOWS"
    }

    links
    {
        "PurpleEngine"
    }

    cppdialect "c++20"
    staticruntime "on"
    systemversion "latest"

    filter "configurations:Debug"
        defines "PRPL_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "PRPL_RELEASE"
        optimize "On"

    filter "configurations:Dist"
        defines "PRPL_DIST"
        optimize "On"