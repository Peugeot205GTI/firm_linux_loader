// Copyright 2013 Normmatt
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "font.h"
#include "draw.h"
#include "fs.h"
#include "linux_config.h"

static u8 *top_screen0 = NULL;
static u8 *top_screen1 = NULL;
static u8 *bot_screen0 = NULL;
static u8 *bot_screen1 = NULL;

static char debugstr[DBG_N_CHARS_X * DBG_N_CHARS_Y] = { 0 };
static u32 debugcol[DBG_N_CHARS_Y] = { DBG_COLOR_FONT };

static bool IsTopScreen(const u8 *screen)
{
	return (screen == top_screen0 || screen == top_screen1);
}

static inline u16 Rgb888ToRgb565(int color)
{
	const u16 r = (color & 0xFF);
	const u16 g = (color >> 8) & 0xFF;
	const u16 b = (color >> 16) & 0xFF;

	return (u16) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}

void InitScreenFbs(int _argc, char *_argv[])
{
	// The arm11 start.S remaps the frame buffers to new
	// locations, so we don't use the values which come
	// from args.
	top_screen0 = (u8 *) FB_TOP_LEFT1;
	top_screen1 = (u8 *) FB_TOP_LEFT2;
	bot_screen0 = (u8 *) FB_BOT_1;
	bot_screen1 = (u8 *) FB_BOT_2;
}

void ClearScreen(u8 *screen, int width, int color)
{
	if (color == COLOR_TRANSPARENT)
		color = COLOR_BLACK;

	const bool is_top = IsTopScreen(screen);

	for (int i = 0; i < (width * SCREEN_HEIGHT); i++) {
		if (is_top) {
			u16 px = Rgb888ToRgb565(color);
			*(screen++) = px & 0xFF;
			*(screen++) = px >> 8;
		} else {
			*(screen++) = color >> 16;      // B
			*(screen++) = color >> 8;       // G
			*(screen++) = color & 0xFF;     // R
		}
	}
}

void ClearScreenFull(bool clear_top, bool clear_bottom)
{
	if (clear_top) {
		ClearScreen(top_screen0, SCREEN_WIDTH_TOP, STD_COLOR_BG);
		ClearScreen(top_screen1, SCREEN_WIDTH_TOP, STD_COLOR_BG);
	}
	if (clear_bottom) {
		ClearScreen(bot_screen0, SCREEN_WIDTH_BOT, STD_COLOR_BG);
		ClearScreen(bot_screen1, SCREEN_WIDTH_BOT, STD_COLOR_BG);
	}
}

void DrawCharacter(u8 *screen, int character, int x, int y, int color, int bgcolor)
{
	const bool is_top = IsTopScreen(screen);
	const int bytes_per_pixel = is_top ? TOP_BYTES_PER_PIXEL : BOT_BYTES_PER_PIXEL;
	const u16 fg565 = Rgb888ToRgb565(color);
	const u16 bg565 = Rgb888ToRgb565(bgcolor);

	for (int yy = 0; yy < 8; yy++) {
		int xDisplacement = (x * bytes_per_pixel * SCREEN_HEIGHT);
		int yDisplacement = ((SCREEN_HEIGHT - (y + yy) - 1) * bytes_per_pixel);
		u8 *screenPos = screen + xDisplacement + yDisplacement;

		u8 charPos = font[character * 8 + yy];
		for (int xx = 7; xx >= 0; xx--) {
			if ((charPos >> xx) & 1) {
				if (is_top) {
					*(screenPos + 0) = fg565 & 0xFF;
					*(screenPos + 1) = fg565 >> 8;
				} else {
					*(screenPos + 0) = color >> 16;         // B
					*(screenPos + 1) = color >> 8;          // G
					*(screenPos + 2) = color & 0xFF;        // R
				}
			} else if (bgcolor != COLOR_TRANSPARENT) {
				if (is_top) {
					*(screenPos + 0) = bg565 & 0xFF;
					*(screenPos + 1) = bg565 >> 8;
				} else {
					*(screenPos + 0) = bgcolor >> 16;       // B
					*(screenPos + 1) = bgcolor >> 8;        // G
					*(screenPos + 2) = bgcolor & 0xFF;      // R
				}
			}
			screenPos += bytes_per_pixel * SCREEN_HEIGHT;
		}
	}
}

void DrawString(u8 *screen, const char *str, int x, int y, int color, int bgcolor)
{
	for (size_t i = 0; i < strlen(str); i++)
		DrawCharacter(screen, str[i], x + i * 8, y, color, bgcolor);
}

void DrawStringF(int x, int y, bool use_top, const char *format, ...)
{
	char str[512] = { 0 }; // 512 should be more than enough
	va_list va;

	va_start(va, format);
	vsnprintf(str, 512, format, va);
	va_end(va);

	for (char *text = strtok(str, "\n"); text != NULL; text = strtok(NULL, "\n"), y += 10) {
		if (use_top) {
			DrawString(top_screen0, text, x, y, STD_COLOR_FONT, STD_COLOR_BG);
			DrawString(top_screen1, text, x, y, STD_COLOR_FONT, STD_COLOR_BG);
		} else {
			DrawString(bot_screen0, text, x, y, STD_COLOR_FONT, STD_COLOR_BG);
			DrawString(bot_screen1, text, x, y, STD_COLOR_FONT, STD_COLOR_BG);
		}
	}
}

void DebugClear()
{
	memset(debugstr, 0x00, DBG_N_CHARS_X * DBG_N_CHARS_Y);
	for (u32 y = 0; y < DBG_N_CHARS_Y; y++)
		debugcol[y] = DBG_COLOR_FONT;
	ClearScreen(top_screen0, SCREEN_WIDTH_TOP, DBG_COLOR_BG);
	ClearScreen(top_screen1, SCREEN_WIDTH_TOP, DBG_COLOR_BG);
}

void DebugSet(const char **strs)
{
	if (strs != NULL) {
		for (int y = 0; y < DBG_N_CHARS_Y; y++) {
			int pos_dbgstr = DBG_N_CHARS_X * (DBG_N_CHARS_Y - 1 - y);
			snprintf(debugstr + pos_dbgstr, DBG_N_CHARS_X, "%-*.*s", DBG_N_CHARS_X - 1, DBG_N_CHARS_X - 1, strs[y]);
			debugcol[y] = DBG_COLOR_FONT;
		}
	}

	int pos_y = DBG_START_Y;
	u32 *col = debugcol + (DBG_N_CHARS_Y - 1);
	for (char *str = debugstr + (DBG_N_CHARS_X * (DBG_N_CHARS_Y - 1)); str >= debugstr; str -= DBG_N_CHARS_X, col--) {
		if (*str != '\0') {
			DrawString(top_screen0, str, DBG_START_X, pos_y, *col, DBG_COLOR_BG);
			DrawString(top_screen1, str, DBG_START_X, pos_y, *col, DBG_COLOR_BG);
			pos_y += DBG_STEP_Y;
		}
	}
}

void DebugColor(u32 color, const char *format, ...)
{
	static bool adv_output = true;
	char tempstr[128] = { 0 }; // 128 instead of DBG_N_CHARS_X for log file
	va_list va;

	va_start(va, format);
	vsnprintf(tempstr, 128, format, va);
	va_end(va);

	if (adv_output) {
		memmove(debugstr + DBG_N_CHARS_X, debugstr, DBG_N_CHARS_X * (DBG_N_CHARS_Y - 1));
		memmove(debugcol + 1, debugcol, (DBG_N_CHARS_Y - 1) * sizeof(u32));
	} else {
		adv_output = true;
	}

	*debugcol = color;
	if (*tempstr != '\r') { // not a good way of doing this - improve this later
		snprintf(debugstr, DBG_N_CHARS_X, "%-*.*s", DBG_N_CHARS_X - 1, DBG_N_CHARS_X - 1, tempstr);
	} else {
		snprintf(debugstr, DBG_N_CHARS_X, "%-*.*s", DBG_N_CHARS_X - 1, DBG_N_CHARS_X - 1, tempstr + 1);
		adv_output = false;
	}

	DebugSet(NULL);
}

void Debug(const char *format, ...)
{
	char tempstr[128] = { 0 }; // 128 instead of DBG_N_CHARS_X for log file
	va_list va;

	va_start(va, format);
	vsnprintf(tempstr, 128, format, va);
	DebugColor(DBG_COLOR_FONT, tempstr);
	va_end(va);
}

void ShowProgress(u64 current, u64 total)
{
	const u32 progX = SCREEN_WIDTH_TOP - 40;
	const u32 progY = SCREEN_HEIGHT - 20;

	if (total > 0) {
		char progStr[8];
		snprintf(progStr, 8, "%3llu%%", (current * 100) / total);
		DrawString(top_screen0, progStr, progX, progY, DBG_COLOR_FONT, DBG_COLOR_BG);
		DrawString(top_screen1, progStr, progX, progY, DBG_COLOR_FONT, DBG_COLOR_BG);
	} else {
		DrawString(top_screen0, "    ", progX, progY, DBG_COLOR_FONT, DBG_COLOR_BG);
		DrawString(top_screen1, "    ", progX, progY, DBG_COLOR_FONT, DBG_COLOR_BG);
	}
}
