"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" /t:ClangTidy ./Vulkan/Vulkan.sln
move /Y .\Vulkan\Vulkan\x64\Debug\Vulkan.ClangTidy\compile_commands.json ./Vulkan
