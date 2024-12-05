project "CSLab1"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++latest"
	staticruntime "off"

    targetdir (bin_out)
    objdir (int_out)
    targetname("CSLab1")

    location "%{wks.location}/CSLab1/Build/ProjectFiles"

    files
    {
        "%{wks.location}/CSLab1/Source/Application/**.h",
        "%{wks.location}/CSLab1/Source/Application/**.cpp"
    }

    includedirs
    {
        "%{wks.location}/CSLab1/Source/Application",
		"%{wks.location}/CSLab1/ThirdParty",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.ImGui}"
    }

	defines
	{
        "_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}
    

	links 
	{ 
		"Glad",
		"GLFW",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
        symbols "off"

