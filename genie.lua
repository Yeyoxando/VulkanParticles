solution ("VulkanParticleEditor")
	configurations { "Debug", "Release" }
	platforms { "x64" }
	location ("./")
	
	projects = { "ParticleEditor", "ParticlesScene", "ParticlesPerformance", "VPECore" }

	for i, prj in ipairs(projects) do 
		project (prj)
		targetname (prj)
		
		prj_path = "./build/" .. prj

		location (prj_path .. "/")
	
        flags { "ExtraWarnings", "Cpp17" }

		defines { "_CRT_SECURE_NO_WARNINGS", }
		
		configuration "vs2019"
			windowstargetplatformversion "10.0.19041.0"

		configuration "Debug"
			defines { "DEBUG", "ASSERT" }
			targetdir ("./bin/Debug/")
			targetsuffix "_d"
			objdir (prj_path .. "/Debug_obj/")
			flags { "Symbols", "NoPCH" }

		configuration "Release"
			targetdir ("./bin/Release/")
			objdir (prj_path .. "/Release_obj/")
			flags { "Optimize", "NoPCH" }

	end



project "VPECore"

  language "C++"
	kind "StaticLib"

	includedirs{
	  "./include/",
	  "./src/engine_internal/",
	  "./external/vulkan/Include/vulkan/",
	  "./external/glfw/include/",
	  "./external/glm/",
	  "./external/glm/**",
	  "./external/stb_image/",
	  "./external/tiny_obj/",
	}

	--Common files
	files{
		"./include/**.h",
		"./src/**.cpp",
		"./src/engine_internal/**.h",
		"./src/engine_internal/**.cpp",
		"./resources/**.vert", 
		"./resources/**.frag", 

		"./external/stb_image/**.h",
		"./external/tiny_obj/**.h",
		"./external/sokol_time/**.h",
		"./external/vulkan/Include/vulkan/**.h",

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
		"./external/glfw/glfw3",
	}
		
		
		
		
		
		
		

	







project "ParticleEditor"

  language "C++"
	kind "ConsoleApp"

	includedirs{
		"./include/",
		--"./src/engine_internal/",
		"./external/glm/",
		"./external/glm/**",
		--"./external/glfw/include/",
		--"./external/vulkan/Include/vulkan/",
	}

	--Common files
	files{
		--ParticleEditor
		"./include/**.h",
		--"./src/**.cpp",
		"./src/engine_internal/**.h",
		--"./src/engine_internal/**.cpp",
		"./tests/main_particle_editor.cpp", 
		"./resources/**.vert", 
		"./resources/**.frag", 

	}

	defines { 	
		"WIN32",
		"_WIN32",
		"_WINDOWS",
	}	 
	
	links{ 
		"./external/vulkan/vulkan-1",
		"./external/glfw/glfw3",
		"VPECore"
	}
	







project "ParticlesScene"

  language "C++"
	kind "ConsoleApp"

	includedirs{
		"./include/",
		--"./src/engine_internal/",
		"./external/glm/",
		"./external/glm/**",
		--"./external/glfw/include/",
		--"./external/vulkan/Include/vulkan/",
	}

	--Common files
	files{
		--ParticleEditor
		"./include/**.h",
		--"./src/**.cpp",
		--"./src/engine_internal/**.h",
		--"./src/engine_internal/**.cpp",
		"./tests/main_particles_scene.cpp", 
		"./resources/**.vert", 
		"./resources/**.frag", 

		"./external/stb_image/**.h",
		"./external/tiny_obj/**.h",
		"./external/sokol_time/**.h",

	}

	defines { 	
		"WIN32",
		"_WIN32",
		"_WINDOWS",
	}		 
	
	links{ 
		"./external/vulkan/vulkan-1",
		"./external/glfw/glfw3",
		"VPECore"
	}






project "ParticlesPerformance" 

  language "C++"
	kind "ConsoleApp"

	includedirs{
		"./include/",
		--"./src/engine_internal/",
		"./external/glm/",
		"./external/glm/**",
		--"./external/glfw/include/",
		--"./external/vulkan/Include/vulkan/",
	}

	--Common files
	files{
		--ParticleEditor
		"./include/**.h",
		--"./src/**.cpp",
		--"./src/engine_internal/**.h",
		--"./src/engine_internal/**.cpp",
		"./tests/main_particles_performance.cpp", 
		"./resources/**.vert", 
		"./resources/**.frag", 

		"./external/stb_image/**.h",
		"./external/tiny_obj/**.h",
		"./external/sokol_time/**.h",

	}

	defines { 	
		"WIN32",
		"_WIN32",
		"_WINDOWS",
	}	 	 
	
	links{ 
		"./external/vulkan/vulkan-1",
		"./external/glfw/glfw3",
		"VPECore"
	}





