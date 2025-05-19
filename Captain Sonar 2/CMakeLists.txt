# CMakeList.txt : CMake project for Captain Sonar 2, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (CMakeTarget "SonarCommand.cpp" "Component.h" "GameObject.h" "Grid.h" "Image.h" "Level.h" "MenuStuff.h"
				"Player.h" "Shapes.h" "Text.h" "TextureWrapper.h" "UI.h" "Window.h" "imgui_impl_sdlrenderer2.cpp" "imgui_impl_sdlrenderer2.h" "imgui_impl_sdl2.cpp" "imgui_impl_sdl2.h" "Server.h")

# Set where the ImGui files are stored
set(IMGUI_PATH "libs/imgui")
    
# Compile as static library 
file(GLOB IMGUI_SOURCES ${IMGUI_PATH}/*.cpp) 
add_library("ImGui" STATIC ${IMGUI_SOURCES}  "Server.h")
target_include_directories("ImGui" PUBLIC ${IMGUI_PATH})
target_link_libraries(CMakeTarget PUBLIC ImGui)

add_subdirectory(libs/SDL)
add_subdirectory(libs/SDL_ttf)
add_subdirectory(libs/boost/)
target_link_libraries(CMakeTarget PRIVATE SDL2::SDL2)
target_link_libraries(CMakeTarget PRIVATE SDL2_ttf::SDL2_ttf)
target_link_libraries(CMakeTarget PRIVATE Boost::asio)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CMakeTarget PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
