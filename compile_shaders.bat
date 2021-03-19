@echo off
for /r %%i in (*.frag, *.vert) do "C:/Program Files/VulkanSDK/1.2.162.0/Bin/glslc.exe" %%i -o %cd%/resources/shaders/shaders_spirv/%%~ni.spv
pause