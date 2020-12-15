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
	  "D:/ProgramFiles/VulkanSDK/1.2.154.1/Include/",
	  "D:/VSLibraries/glfw-3.3.2.bin.WIN64/include/",
	  "D:/VSLibraries/glm/",
	  "D:/VSLibraries/SingleHeaderFiles/stb_image/",
	}

	--Common files
	files{
		--ParticleEditor
		"./include/**.h",
		"./src/**.cpp",
		"./tests/**.cpp", 
		"./resources/**.vert", 
		"./resources/**.frag", 

		-- Vulkan
		--"D:/ProgramFiles/VulkanSDK/1.2.154.1/Include/",
		
		--GLFW
		--"./src/deps/GLFW/src/context.c",
		--"./src/deps/GLFW/src/init.c",
		--"./src/deps/GLFW/src/input.c",
		--"./src/deps/GLFW/src/monitor.c",
		--"./src/deps/GLFW/src/vulkan.c",
		--"./src/deps/GLFW/src/window.c",
		--"./src/deps/GLFW/src/internal.h",
		--"./src/deps/GLFW/src/mappings.h",
		--"./src/deps/GLFW/include/GLFW/glfw3.h",
		--"./src/deps/GLFW/include/GLFW/glfw3native.h",

		--GLM
		--"./deps/glm/*.h",
		--"./deps/glm/*.hpp",

	}

	defines { 	
		"GLFW_INCLUDE_VULKAN",
		"WIN32",
		"_WIN32",
		"_WINDOWS",
		"GLM_FORCE_RADIANS",
		"STB_IMAGE_IMPLEMENTATION ",
	}	 
	
	links{
		"D:/ProgramFiles/VulkanSDK/1.2.154.1/Lib/vulkan-1",
		"D:/VSLibraries/glfw-3.3.2.bin.WIN64/lib-vc2019/glfw3"
	}

