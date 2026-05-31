#pragma once
#ifndef __MAIN_HPP
#define __MAIN_HPP
#include <iostream>
#include "include\glad\glad.h"
#include "include\SDL3\SDL.h"
#include "include/glm/glm.hpp"
#include <fstream>
#include <string>



using uint = unsigned int;
int main();
int ReadFileContents(const char* filename, int binaryMode, std::string& contentOut);
int loop(SDL_Window* window, uint shaderProgram, uint VAO);







#endif // __MAIN_HPP