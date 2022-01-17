#define _CRT_SECURE_NO_WARNINGS

// Functions without 'static', 'extern' prefixes is just a normal
// function, they can be accessed by other files using 'extern'.
// Define your normal function prototypes below.

#ifndef SCENE_GAME_H
#define SCENE_GAME_H
#include "game.h"


Scene scene_main_create(void);
Scene game_win_create(void);
Scene game_lose_create(void);

#endif
