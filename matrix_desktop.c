#include "Windows.h"
#include "qdbmp.h"
#include "font8x8/font8x8.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>

BMP *global_bmp;
char *bmp_path;

void terminate_handler(int signal_id){
	int temp;

	if(signal_id == SIGTERM || signal_id == SIGINT){
		BMP_Free(global_bmp);
		free(bmp_path);
		exit(0);
	} else {
		BMP_Free(global_bmp);
		free(bmp_path);
		abort();
	}
}

char choose_random_character(unsigned int alpha, unsigned int capital_alpha, unsigned int number, unsigned int symbol){
	const char symbols[] = {'~', '[', ']', '(', ')', '{', '}', ';', ':', '?', '>', '<', '\\', '/', '|', '*', '%', '#', '@', '&'};
	unsigned int choice;

	capital_alpha += alpha;
	number += capital_alpha;
	symbol += number;

	choice = rand()%symbol;
	
	if(choice < alpha){
		return 'a' + rand()%26;
	} else if(choice < capital_alpha){
		return 'A' + rand()%26;
	} else if(choice < number){
		return '0' + rand()%10;
	} else {
		return symbols[rand()%sizeof(symbols)];
	}
}

unsigned char get_font_pix(char c, unsigned char x, unsigned char y){
	return (font8x8_basic[c][y]>>x)&1;
}

void render_character(BMP *bmp, char c, unsigned int x, unsigned int y, unsigned char r, unsigned char g, unsigned char b, unsigned char r2, unsigned char g2, unsigned char b2){
	unsigned char font_x;
	unsigned char font_y;

	for(font_x = 0; font_x < 8; font_x++){
		for(font_y = 0; font_y < 8; font_y++){
			if(get_font_pix(c, font_x, font_y)){
				BMP_SetPixelRGB(bmp, x + font_x, y + font_y, r, g, b);
			} else {
				BMP_SetPixelRGB(bmp, x + font_x, y + font_y, r2, g2, b2);
			}
		}
	}
}

char *replace_path(char *path, const char *file_name){
	char *current_char;
	char *output;
	unsigned int num_chars;

	current_char = path + strlen(path) - 1;
	num_chars = strlen(path) + strlen(file_name) + 1;
	while(*current_char != '\\'){
		current_char--;
		num_chars--;
	}

	output = malloc(sizeof(char)*num_chars);
	memcpy(output, path, num_chars - strlen(file_name) - 1);
	memcpy(output + num_chars - strlen(file_name) - 1, file_name, strlen(file_name));
	output[num_chars - 1] = (char) 0;
	return output;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstace, LPSTR lpCmdLine, int nCmdShow){
	char characters[75][60];
	unsigned int heights[75];
	int i;
	int j;
	int temp;
	int diff;
	FILE *foutput;
	unsigned char not_full_screen;
	HWND top_window;
	WINDOWPLACEMENT top_window_placement;
	WINDOWINFO top_window_info;
	RECT top_window_rect;
	HWND tray_window;
	char background_path[256];

	GetModuleFileName(NULL, background_path, 256);
	bmp_path = replace_path(background_path, "background.bmp");
	srand(time(NULL));
	for(i = 0; i < 75; i++){
		heights[i] = rand()%60;
		for(j = 0; j < 60; j++){
			characters[i][j] = ' ';
		}
	}

	global_bmp = BMP_Create(600, 480, 24);
	signal(SIGTERM, terminate_handler);
	signal(SIGINT, terminate_handler);
	while(1){
		Sleep(750);
		top_window = GetAncestor(GetForegroundWindow(), GA_ROOTOWNER);
		top_window_placement.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement(top_window, &top_window_placement);
		GetWindowRect(top_window, &top_window_rect);
		top_window_info.cbSize = sizeof(WINDOWINFO);
		GetWindowInfo(top_window, &top_window_info);
		not_full_screen = top_window_rect.top == top_window_placement.rcNormalPosition.top && top_window_rect.left == top_window_placement.rcNormalPosition.left && top_window_rect.bottom == top_window_placement.rcNormalPosition.bottom && top_window_rect.right == top_window_placement.rcNormalPosition.right;
		if(top_window == GetDesktopWindow() || top_window == GetShellWindow() || !GetClassLongPtrA(top_window, GCLP_HICON) || (not_full_screen && (GetWindowLongA(top_window, GWL_STYLE)&WS_CAPTION))){
			for(i = 0; i < 75; i++){
				characters[i][heights[i]] = choose_random_character(10, 10, 2, 2);
				for(j = 0; j < 60; j++){
					temp = heights[i] - j;
					if(temp < 0){
						temp += 60;
					}
					diff = temp%60;
					render_character(global_bmp, characters[i][j], i<<3, j<<3, 0, 255*(60 - diff)*(60 - diff)/3600, 0, 0, 0, 0);
				}
				heights[i] = (heights[i] + 1)%60;
			}
			BMP_WriteFile(global_bmp, "background.bmp");
			SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, bmp_path, SPIF_UPDATEINIFILE);
		}

	}
	BMP_Free(global_bmp);
}
