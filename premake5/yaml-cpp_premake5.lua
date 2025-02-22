project "yaml-cpp"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	location "%{wks.location}/LudoNarrative/vendor/yaml-cpp"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.location}/include/**.hpp",
        "%{prj.location}/include/**.h",
        "%{prj.location}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.location}/include"
	}