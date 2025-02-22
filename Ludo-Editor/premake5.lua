project "Ludo-Editor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "c++20"

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
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.yaml_cpp}",
        "%{IncludeDir.ImGuizmo}",
        "%{IncludeDir.Box2D}"
    }

    links
    {
        "LudoNarrative"
    }

    postbuildcommands { "{COPYFILE} %{wks.location}LudoNarrative/vendor/dxc/bin/x64/dxcompiler.dll %{cfg.buildtarget.directory}", "{COPYFILE} %{wks.location}LudoNarrative/vendor/dxc/bin/x64/dxil.dll %{cfg.buildtarget.directory}" }
