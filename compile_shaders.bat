@echo off
for /r %%i in (*.frag, *.vert) do D:/ProgramFiles/VulkanSDK/1.2.154.1/Bin/glslc.exe %%i -o %cd%/resources/shaders/shaders_spirv/%%~ni.spv
pause