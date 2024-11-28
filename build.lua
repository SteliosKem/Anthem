-- premake5.lua
workspace "Anthem"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "Anthem-Build-Manager"

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

OutputDir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

group "Anthem"
	include "Anthem/build-lib.lua"
group ""

include "Anthem-Build-Manager/build-app.lua"