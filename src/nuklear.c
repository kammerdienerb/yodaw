#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>


#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT

#define NK_INCLUDE_DEFAULT_ALLOCATOR

#define NK_IMPLEMENTATION
#define NK_SDL_GL2_IMPLEMENTATION
#include "nuklear.h"
#include "nuklear_sdl_gl2.h"
