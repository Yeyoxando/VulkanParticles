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
			targetdir ("./bin/Debug/x64")
			targetsuffix "_d"
			objdir ("./build/Debug")
			flags { "Symbols", "NoPCH" }

		configuration "Release"
			targetdir ("./bin/Release/x64")
			objdir ("./build/Release")
			flags { "Optimize", "NoPCH" }

	end


project "ParticleEditor"

  language "C++"
	kind "ConsoleApp"
	

	includedirs{
	  "./include/",
	  "./src/engine_internal/",
	  "./external/vulkan/Include/",
	  "./external/glfw/include/",
	  "./external/glm/",
	  "./external/glm/**",
	  "./external/stb_image/",
	  "./external/tiny_obj/",
	}

	--Common files
	files{
		--ParticleEditor
		"./include/**.h",
		"./src/**.cpp",
		"./src/engine_internal/**.h",
		"./src/engine_internal/**.cpp",
		"./tests/**.cpp", 
		"./resources/**.vert", 
		"./resources/**.frag", 

		"./external/stb_image/**.h",
		"./external/tiny_obj/**.h",
		"./external/sokol_time/**.h",

	}

	defines { 	
                "VK_USE_PLATFORM_WIN32_KHR",
		"GLFW_INCLUDE_VULKAN",
		"WIN32",
		"_WIN32",
		"_WINDOWS",
		"GLM_FORCE_RADIANS",
		"GLM_FORCE_DEPTH_ZERO_TO_ONE",
		"GLM_ENABLE_EXPERIMENTAL",
		"TINYOBJLOADER_IMPLEMENTATION",
	}	 
	
	links{
		"./external/vulkan/vulkan-1",
		"./external/glfw/glfw3"
	}

