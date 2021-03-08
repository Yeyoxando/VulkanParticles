solution ("VulkanParticles")
	configurations { "Debug", "Release" }
	platforms { "x64" }
	location ("build")	
	
	projects = { "ParticleEditor" }

	for i, prj in ipairs(projects) do 
		project (prj)
		targetname (prj)
		
		prj_path = "./build/" .. prj

		location (prj_path .. "/" .. _ACTION)
	
        flags { "ExtraWarnings", "Cpp17" }

		defines { 	
			"_CRT_SECURE_NO_WARNINGS",
			}
		configuration "vs2019"
			windowstargetplatformversion "10.0.19041.0"

		configuration "Debug"
			defines { "DEBUG", "ASSERT" }
			targetdir ("./bin")
			targetsuffix "_d"
			objdir ("./build/Debug")
			flags { "Symbols", "NoPCH" }

		configuration "Release"
			targetdir ("./bin")
			objdir ("./build/Release")
			flags { "Optimize", "NoPCH" }

	end


project "ParticleEditor"

  language "C++"
	kind "ConsoleApp"
	

	includedirs{
	  "./include/",
	  "C:/Program Files/VulkanSDK/1.2.154.1/Include/",
	  "C:/VSLibraries/glfw-3.3.2.bin.WIN64/include/",
	  "C:/VSLibraries/glm/",
	  "C:/VSLibraries/SingleHeaderFiles/stb_image/",
	  "C:/VSLibraries/SingleHeaderFiles/tiny_obj_loader/",
	}

	--Common files
	files{
		--ParticleEditor
		"./include/**.h",
		"./src/**.cpp",
		"./src/internal/**.h",
		"./tests/**.cpp", 
		"./resources/**.vert", 
		"./resources/**.frag", 

		-- Vulkan
		--"C:/Program Files/VulkanSDK/1.2.154.1/Include/",
		
		--GLM
		--"./deps/glm/*.h",
		--"./deps/glm/*.hpp",

		"C:/VSLibraries/SingleHeaderFiles/tiny_obj_loader/**.h"

	}

	defines { 	
		"GLFW_INCLUDE_VULKAN",
		"WIN32",
		"_WIN32",
		"_WINDOWS",
		"GLM_FORCE_RADIANS",
		"GLM_FORCE_DEPTH_ZERO_TO_ONE",
		"GLM_ENABLE_EXPERIMENTAL",
		"STB_IMAGE_IMPLEMENTATION",
		"TINYOBJLOADER_IMPLEMENTATION",
	}	 
	
	links{
		"C:/Program Files/VulkanSDK/1.2.154.1/Lib/vulkan-1",
		"C:/VSLibraries/glfw-3.3.2.bin.WIN64/lib-vc2019/glfw3"
	}

