
del ".\build\Debug\" /s /q
del ".\build\Release\" /s /q 
del ".\build\ParticleEditor\" /s /q 
del ".\build\VulkanParticles.sln"

.\tools\genie.exe vs2019

.\build\VulkanParticles.sln

exit