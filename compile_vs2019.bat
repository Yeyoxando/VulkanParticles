rmdir /s /q build/ParticleEditor
rm build/VulkanParticles.sln

.\tools\genie.exe vs2019

.\build\VulkanParticles.sln

pause