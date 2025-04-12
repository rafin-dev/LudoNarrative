workspace "LudoNarrative"
    architecture "x64"
    flags { "MultiProcessorCompile" }
    startproject "Ludo-Editor"
    
    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

    defines
	{
		"YAML_CPP_STATIC_DEFINE"
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

        defines { "LD_ENABLE_ASSERTS" }

    filter "configurations:Dist"
        defines "LUDO_DIST"
        optimize "on"
        runtime "Release"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["ImGui"] = "%{wks.location}/LudoNarrative/vendor/imgui"
IncludeDir["stb_image"] = "%{wks.location}/LudoNarrative/vendor/stb_image"
IncludeDir["dxc"] = "%{wks.location}/LudoNarrative/vendor/dxc"
IncludeDir["entt"] = "%{wks.location}/LudoNarrative/vendor/entt/include"
IncludeDir["yaml_cpp"] = "%{wks.location}/LudoNarrative/vendor/yaml-cpp/include"
IncludeDir["ImGuizmo"] = "%{wks.location}/LudoNarrative/vendor/ImGuizmo"
IncludeDir["Box2D"] = "%{wks.location}/LudoNarrative/vendor/Box2D/include"
IncludeDir["Mono"] = "%{wks.location}/LudoNarrative/vendor/MonoLib/include/Mono-2.0"

group "Dependencies"
    include "premake5/imgui_premake5"
    include "premake5/yaml-cpp_premake5"
    include "premake5/Box2D_Premake5"
group ""

include "LudoNarrative"
include "Ludo-Editor"