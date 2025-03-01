project "LudoNarrative"
    kind "StaticLib"
    language "C++"
    cppdialect "c++20"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "ldpch.h"
    pchsource "src/ldpch.cpp"

    files
    {
        "%{prj.location}/src/**.h",
        "%{prj.location}/src/**.cpp",
        "%{prj.location}/vendor/stb_image/**.h",
        "%{prj.location}/vendor/stb_image/**.cpp",
        "%{prj.location}/vendor/ImGuizmo/ImGuizmo.h",
        "%{prj.location}/vendor/ImGuizmo/ImGuizmo.cpp"
    }

    includedirs
    {
        "%{prj.location}/src",
        "%{prj.location}/vendor/spdlog/include",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.stb_image}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.yaml_cpp}",
        "%{IncludeDir.ImGuizmo}",
        "%{IncludeDir.Box2D}"
    }

    links
    {
        "ImGui",
        "yaml-cpp",
        "Box2D"
    }

    filter "files:vendor/ImGuizmo/*.cpp"
        flags { "NoPCH" }