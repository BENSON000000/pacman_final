#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>
#include <math.h>
#include "scene_menu_object.h"
#include "scene_settings.h"
#include "scene_game.h"
#include "scene_menu.h"
#include "utility.h"
#include "shared.h"

// [HACKATHON 3-9]
// TODO: Create scene_settings.h and scene_settings.c.
// No need to do anything for this part. We've already done it for
// you, so this 2 files is like the default scene template.


// Variables and functions with 'static' prefix at the top level of a
// source file is only accessible in that file ("file scope", also
// known as "internal linkage"). If other files has the same variable
// name, they'll be different variables.

/* Define your static vars / function prototypes below. */

// TODO: More variables and functions that will only be accessed
// inside this scene. They should all have the 'static' prefix.
static ALLEGRO_SAMPLE_ID menuBGM;
static Button btn2, btn3, btn4, btn5, btn6;
static void s_draw(void) {
	al_clear_to_color(al_map_rgb(77, 57, 0));
	drawButton(btn2);
	char s1[50];
	sprintf_s(s1, sizeof(s1), "MUSIC %.0f%%", music_volume * 200);
	al_draw_text(
		menuFont,
		al_map_rgb(255, 250, 250),
		320,
		280,
		ALLEGRO_ALIGN_LEFT,
		s1
	);
	char s2[50];
	sprintf_s(s2, sizeof(s2), "EFFECT %.0f%%", effect_volume * 200
		);
	al_draw_text(
		menuFont,
		al_map_rgb(255, 250, 250),
		310,
		500,
		ALLEGRO_ALIGN_LEFT,
		s2
	);
	drawButton(btn3);
	drawButton(btn4);
	drawButton(btn5);
	drawButton(btn6);
}



static void on_mouse_move(int a, int mouse_x, int mouse_y, int f) {
	btn2.hovered = buttonHover(btn2, mouse_x, mouse_y);
	btn3.hovered = buttonHover(btn3, mouse_x, mouse_y);
    btn4.hovered = buttonHover(btn4, mouse_x, mouse_y);
	btn5.hovered = buttonHover(btn5, mouse_x, mouse_y);
	btn6.hovered = buttonHover(btn6, mouse_x, mouse_y);

}

static void on_mouse_down() {
	if (btn2.hovered) {
		stop_bgm(menuBGM);
		game_change_scene(scene_menu_create());
	}
	else if (btn3.hovered) {
		printf("%f", music_volume);
		if (music_volume + 0.05 > 0.5) music_volume = 0.5;
		else music_volume += 0.05;
		stop_bgm(menuBGM);
		menuBGM=play_bgm(themeMusic, music_volume);
	}
	else if (btn4.hovered) {
		if (music_volume - 0.05 < 0) music_volume = 0;
		else music_volume -= 0.05;
		stop_bgm(menuBGM);
		menuBGM = play_bgm(themeMusic, music_volume);
	}
	else if (btn5.hovered) {
		if (effect_volume + 0.05 > 0.5) effect_volume = 0.5;
		else effect_volume += 0.05;
		stop_bgm(menuBGM);
		menuBGM = play_bgm(themeMusic, music_volume);
	}
	else if (btn6.hovered) {
		if (effect_volume - 0.05 < 0) effect_volume = 0;
		else effect_volume -= 0.05;
		stop_bgm(menuBGM);
		menuBGM = play_bgm(themeMusic, music_volume);
	}
		
}

static void destroy() {
	al_destroy_bitmap(btn2.default_img);
	al_destroy_bitmap(btn2.hovered_img);
}

static void on_key_down(int keycode) {
	//no use
	/*
	switch (keycode) {
	case ALLEGRO_KEY_ENTER:
		game_change_scene(scene_main_create());
		break;
	default:
		break;
	}
	*/
}

// The only function that is shared across files.
Scene scene_settings_create(void) {
	menuBGM = play_bgm(themeMusic,music_volume);
	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	btn2 = button_create(730, 20, 50, 50, "Assets/settings.png", "Assets/settings2.png");
	btn3 = button_create(550, 260, 60, 60, "Assets/sound1.png", "Assets/sound2.png");
	btn4 = button_create(200, 280, 70, 20, "Assets/sound3.png", "Assets/sound4.png");
	btn5 = button_create(550, 470, 60, 60, "Assets/sound1.png", "Assets/sound2.png");
	btn6 = button_create(200, 500, 70, 20, "Assets/sound3.png", "Assets/sound4.png");
	scene.name = "Settings";
	scene.draw = &s_draw;
	scene.destroy = &destroy;
	scene.on_key_down = &on_key_down;
	scene.on_mouse_move = &on_mouse_move;
	scene.on_mouse_down = &on_mouse_down;

	// TODO: Register more event callback functions such as keyboard, mouse, ...
	game_log("Settings scene created");
	return scene;
}
