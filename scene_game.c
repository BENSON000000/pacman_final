#define _CRT_SECURE_NO_WARNINGS

#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <string.h>
#include "game.h"
#include "shared.h"
#include "utility.h"
#include "scene_game.h"
#include "scene_menu.h"
#include "pacman_obj.h"
#include "ghost.h"
#include "map.h"


// [HACKATHON 2-0]
// Just modify the GHOST_NUM to 1
#define GHOST_NUM 4
/* global variables*/
extern const uint32_t GAME_TICK_CD;
extern uint32_t GAME_TICK;
extern ALLEGRO_TIMER* game_tick_timer;
int game_main_Score = 0;
bool game_over = false;
bool game_win = false;

/* Internal variables*/
static ALLEGRO_TIMER* power_up_timer;
static const int power_up_duration = 10;
static Pacman* pman;
static Map* basic_map;
static Ghost** ghosts;
bool debug_mode = false;
bool cheat_mode = false;

/* Declare static function prototypes */
static void init(void);
static void step(void);
static void checkItem(void);
static void status_update(void);
static void update(void);
static void draw(void);
static void w_draw(void); 
static void l_draw(void);
static void printinfo(void);
static void destroy(void);
static void on_key_down(int key_code);
static void on_key_down2(int key_code);
static void on_mouse_down(void);
static void on_mouse_down2(void);
static void render_init_screen(void);
static void draw_hitboxes(void);
static ALLEGRO_SAMPLE_ID hot_Nigga;
extern ALLEGRO_SAMPLE* GAME_MUSIC;

static void init(void) {
	stop_bgm(hot_Nigga);
	hot_Nigga = play_bgm(GAME_MUSIC, music_volume);
	game_over = false;
	game_win = false;
	game_main_Score = 0;
	// create map
	basic_map = create_map("Assets/test.txt");
	// [TODO]
	// Create map from .txt file and design your own map !!
	// basic_map = create_map("Assets/map_nthu.txt");
	if (!basic_map) {
		game_abort("error on creating map");
	}	
	// create pacman
	pman = pacman_create();
	if (!pman) {
		game_abort("error on creating pacamn\n");
	}
	
	// allocate ghost memory
	// [HACKATHON 2-1]
	// TODO: Allocate dynamic memory for ghosts array.
	/*
	ghosts = (...)malloc(sizeof(...) * GHOST_NUM)
	*/
	ghosts = (Ghost**)malloc(sizeof(Ghost*) * GHOST_NUM);
	if(!ghosts){
		game_log("We haven't create any ghosts!\n");
	}
	else {
		// [HACKATHON 2-2]
		// TODO: create a ghost.
		// Try to look the definition of ghost_create and figure out what should be placed here.
		for (int i = 0; i < GHOST_NUM; i++) {
			
			game_log("creating ghost %d\n", i);
			ghosts[i] = ghost_create(i);  
			if (!ghosts[i])
				game_abort("error creating ghost\n");
			
		}
	}
	GAME_TICK = 0;

	render_init_screen();
	power_up_timer = al_create_timer(1.0f); // 1 tick / sec
	if (!power_up_timer)
		game_abort("Error on create timer\n");
	return ;
}

static void step(void) {
	if (pman->objData.moveCD > 0)
		pman->objData.moveCD -= pman->speed;
	for (int i = 0; i < GHOST_NUM; i++) {
		// important for movement
		if (ghosts[i]->objData.moveCD > 0)
			ghosts[i]->objData.moveCD -= ghosts[i]->speed;
	}
}
static void checkItem(void) {
	int Grid_x = pman->objData.Coord.x, Grid_y = pman->objData.Coord.y;
	if (Grid_y >= basic_map->row_num - 1 || Grid_y <= 0 || Grid_x >= basic_map->col_num - 1 || Grid_x <= 0)
		return;
	// [HACKATHON 1-3]
	// TODO: check which item you are going to eat and use `pacman_eatItem` to deal with it.
	
	switch (basic_map->map[Grid_y][Grid_x])
	{
	case '.':

		basic_map->beansCount--;
		pacman_eatItem(pman, '.');
		break;
	case 'P':
		for (int i = 0;i < GHOST_NUM; i++) {
			ghost_toggle_FLEE(ghosts[i], true);
		}
		al_start_timer(power_up_timer);
		pacman_eatItem(pman, 'P');
		break;
	default:
		break;
	}
	
	// [HACKATHON 1-4]
	// erase the item you eat from map
	// be careful no erasing the wall block.
	/*
		basic_map->map...;
	*/
	basic_map->map[Grid_y][Grid_x]=' ';
}
// [TODO]
		// use `getDrawArea(..., GAME_TICK_CD)` and `RecAreaOverlap(..., GAME_TICK_CD)` functions to detect
		// if pacman and ghosts collide with each other.
		// And perform corresponding operations.
		// [NOTE]
		// You should have some branch here if you want to implement power bean mode.
		// Uncomment Following Code
		/*
		if(!cheat_mode and collision of pacman and ghost)
		{
			game_log("collide with ghost\n");
			al_rest(1.0);
			pacman_die();
			game_over = true;
			break;
		}
		*/
static void status_update(void) {
	if (al_get_timer_count(power_up_timer) >= power_up_duration - 2 && al_get_timer_count(power_up_timer) < power_up_duration) {
		for (int i = 0;i < GHOST_NUM;i++) {
			if (ghosts[i]->status == FLEE) {
				ghosts[i]->status = preFREEDOM;
			}
		}
	}

	if (al_get_timer_count(power_up_timer) >= power_up_duration ) {
		for (int i = 0;i < GHOST_NUM;i++) {
			ghost_toggle_FLEE(ghosts[i], false);
		}
		al_stop_timer(power_up_timer);
		al_set_timer_count(power_up_timer, 0);
	}

	for (int i = 0; i < GHOST_NUM; i++) {		
		if (ghosts[i]->status == GO_IN)
			continue;

		if (basic_map->beansCount == 0) {
			game_log("beansCount=0\n");
			al_rest(1.0);
			game_win = true;
			break;
		}
		
		RecArea RA = getDrawArea(pman->objData, GAME_TICK_CD);
		RecArea RB = getDrawArea(ghosts[i]->objData, GAME_TICK_CD);
		
		if (!cheat_mode && RecAreaOverlap(RA, RB)){
			if (ghosts[i]->status == FREEDOM) {
				game_log("collide with ghost %d and die\n",i);
				al_rest(1.0);
				pacman_die();
				game_over = true;
				break;
			}
			else if (ghosts[i]->status == FLEE || ghosts[i]->status == preFREEDOM) {
				game_log("collide with ghost %d and eat it\n", i);
				ghost_collided(ghosts[i]);
			}
		}
	}
}

static void update(void) {
	if (game_win) {
		game_change_scene(game_win_create());	
		stop_bgm(hot_Nigga);
		return;
	}

	if (game_over) {
		/*
			[TODO]
			start pman->death_anim_counter and schedule a game-over event (e.g change scene to menu) after Pacman's death animation finished
			game_change_scene(...);
		*/
		stop_bgm(hot_Nigga);
		al_start_timer(pman->death_anim_counter);

		//write a thorough game_over_event() functiono;

		if (al_get_timer_count(pman->death_anim_counter) > 192) {
			al_stop_timer(pman->death_anim_counter);
			al_set_timer_count(pman->death_anim_counter, 0);
			game_change_scene(game_lose_create());
		}
		return;
	}

	step();
	checkItem();
	status_update();
	pacman_move(pman, basic_map);
	for (int i = 0; i < GHOST_NUM; i++) {
		ghosts[i]->move_script(ghosts[i], basic_map, pman);
	}
}

static void draw(void) {
	al_clear_to_color(al_map_rgb(77, 57, 0));


	//	[TODO]
	//	Draw scoreboard, something your may need is sprinf();
	/*
		al_draw_text(...);
	*/
	char score[100];
	game_main_Score =( (basic_map->beansNum) - (basic_map->beansCount))*10;
	sprintf_s(score, sizeof(score), "SCORE:%4d", game_main_Score);//GAME_MAIN_SCORE is availible

	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 255),
		SCREEN_W / 2 - 75,
		SCREEN_H - 100,
		ALLEGRO_ALIGN_LEFT,
		score
	);

	draw_map(basic_map);

	pacman_draw(pman);
	if (game_win) return;
	if (game_over) return;
	// no drawing below when game over
	for (int i = 0; i < GHOST_NUM; i++)
		ghost_draw(ghosts[i]);

	//debugging mode
	if (debug_mode) {
		draw_hitboxes();
	}
}
static void w_draw(void) {
	al_map_rgb(77, 57, 0);
	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 0),
		250,
		370,
		ALLEGRO_ALIGN_LEFT,
		"CONGRATULTIONS"
	);
	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 0),
		215,
		500,
		ALLEGRO_ALIGN_LEFT,
		"press enter to return menu"
	);
}
static void l_draw(void) {
	al_map_rgb(77, 57, 0);
	al_draw_text(
		menuFont,
		al_map_rgb(77, 57, 0),
		330,
		340,
		ALLEGRO_ALIGN_LEFT,
		"YOU LOSE"
	);
	al_draw_text(
		menuFont,
		al_map_rgb(77, 57, 0),
		220,
		470,
		ALLEGRO_ALIGN_LEFT,
		"press enter to return menu"
	);
}
static void draw_hitboxes(void) {
	RecArea pmanHB = getDrawArea(pman->objData, GAME_TICK_CD);
	al_draw_rectangle(
		pmanHB.x, pmanHB.y,
		pmanHB.x + pmanHB.w, pmanHB.y + pmanHB.h,
		al_map_rgb_f(1.0, 0.0, 0.0), 2
	);

	for (int i = 0; i < GHOST_NUM; i++) {
		RecArea ghostHB = getDrawArea(ghosts[i]->objData, GAME_TICK_CD);
		al_draw_rectangle(
			ghostHB.x, ghostHB.y,
			ghostHB.x + ghostHB.w, ghostHB.y + ghostHB.h,
			al_map_rgb_f(1.0, 0.0, 0.0), 2
		);
	}

}

static void printinfo(void) {
	game_log("pacman:\n");
	game_log("coord: %d, %d\n", pman->objData.Coord.x, pman->objData.Coord.y);
	game_log("PreMove: %d\n", pman->objData.preMove);
	game_log("NextTryMove: %d\n", pman->objData.nextTryMove);
	game_log("Speed: %f\n", pman->speed);
}


static void destroy(void) {
	/*
		[TODO]
		free map array, Pacman and ghosts
	*/
	
	pacman_destroy(pman);
	for (int i = 0; i < GHOST_NUM; i++) {
		ghost_destroy(ghosts[i]);
	}
	
	free(basic_map);
	free(ghosts);
}

static void on_key_down(int key_code) {
	switch (key_code)
	{
		// [HACKATHON 1-1]	
		// TODO: Use allegro pre-defined enum ALLEGRO_KEY_<KEYNAME> to controll pacman movement
		// we provided you a function `pacman_NextMove` to set the pacman's next move direction.
		
		case ALLEGRO_KEY_W:
			pacman_NextMove(pman, UP);
			break;
		case ALLEGRO_KEY_A:
			pacman_NextMove(pman, LEFT);
			break;
		case ALLEGRO_KEY_S:
			pacman_NextMove(pman, DOWN);
			break;
		case ALLEGRO_KEY_D:
			pacman_NextMove(pman, RIGHT);
			break;
		case ALLEGRO_KEY_C:
			cheat_mode = !cheat_mode;
			if (cheat_mode)
				printf("cheat mode on\n");
			else 
				printf("cheat mode off\n");
			break;
		case ALLEGRO_KEY_G:
			debug_mode = !debug_mode;
			break;
		
	default:
		break;
	}

}
static void on_key_down2(int key_code) {
	if (ALLEGRO_KEY_ENTER) {
		game_change_scene(scene_menu_create());
}
}

static void on_mouse_down(void) {
	/*draw*/

}

static void render_init_screen(void) {
	al_clear_to_color(al_map_rgb(77, 57, 0));

	draw_map(basic_map);
	pacman_draw(pman);
	for (int i = 0; i < GHOST_NUM; i++) {
		ghost_draw(ghosts[i]);
	}

	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 0),
		400, 400,
		ALLEGRO_ALIGN_CENTER,
		"READY!"
	);
	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 255),
		SCREEN_W / 2 - 75,
		SCREEN_H - 100,
		ALLEGRO_ALIGN_LEFT,
		"SCORE: "
	);

	al_flip_display();
	al_rest(1.0);

}
// Functions without 'static', 'extern' prefixes is just a normal
// function, they can be accessed by other files using 'extern'.
// Define your normal function prototypes below.

// The only function that is shared across files.
Scene scene_main_create(void) {
	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "Start";
	scene.initialize = &init;
	scene.update = &update;
	scene.draw = &draw;
	scene.destroy = &destroy;
	scene.on_key_down = &on_key_down;
	scene.on_mouse_down = &on_mouse_down;
	// TODO: Register more event callback functions such as keyboard, mouse, ...
	game_log("Start scene created");
	return scene;
}
Scene game_win_create(void) {
	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "win";
	scene.draw = &w_draw;
	scene.on_key_down = &on_key_down2;
	// TODO: Register more event callback functions such as keyboard, mouse, ...
	game_log("Win scene created");
	return scene;
}
Scene game_lose_create(void) {
	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "lose";
	scene.draw = &l_draw;
	scene.on_key_down = &on_key_down2;
	// TODO: Register more event callback functions such as keyboard, mouse, ...
	game_log("Lose scene created");
	return scene;
}


