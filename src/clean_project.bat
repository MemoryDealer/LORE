@echo off

del *.sln *.sdf *.vcxproj *.user *.filters *.ipch /s
rmdir .vs /s /q
rmdir ipch /s /q
rmdir Debug
rmdir Release
