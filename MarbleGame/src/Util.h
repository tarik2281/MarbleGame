//
//  Util.h
//  MarbleGame
//
//  Created by Tarik Karaca on 09.02.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#ifndef MarbleGame_Util_h
#define MarbleGame_Util_h

#ifndef EDITOR
#define RENDERER_H "main.h"
#else
#define RENDERER_H "../Editor/Renderer.h"
#endif

#ifndef EDITOR
#define GAME_FUNC(expression) expression;
#else
#define GAME_FUNC(expression)
#endif

#ifdef _WIN32
#define PRINT_OUTPUT OutputDebugStringA
#else
#define PRINT_OUTPUT printf
#endif

#define DEBUG_MESSAGE(string) MessageBoxA(NULL, string, "Info", MB_OK)
#define DEBUG_PRINT(string) OutputDebugStringA(string)

#endif
