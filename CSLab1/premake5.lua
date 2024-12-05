-- Main premake
include "Dependencies.lua"

workspace "CSLab1"
    architecture "x64"
    startproject "CSLab1"
    configurations { "Debug", "Release"}
    platforms {"Win64"}
    buildoptions { "/MP "}

    location "..\\"

    outputdir = "%{cfg.platform}/%{cfg.buildcfg}"
    bin_out = "%{wks.location}/CSLab1/Binaries/Application/" .. outputdir .. ""
    int_out = "%{wks.location}/CSLab1/Intermediate/Application/" .. outputdir .. ""
    tp_bin_out = "%{wks.location}/CSLab1/Binaries/ThirdParty/" .. outputdir .. ""
    tp_int_out = "%{wks.location}/CSLab1/Intermediate/ThirdParty/" .. outputdir .. ""

    debugdir "%{wks.location}/CSLab1"


group "Dependencies"
	include "ThirdParty/GLFW"
	include "ThirdParty/Glad"
	include "ThirdParty/imgui"
group ""

group "Application"
    include "Source"
group ""

