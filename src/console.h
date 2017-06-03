#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <vita2d.h>
#include <psp2/types.h>

#define BLACK   RGBA8(0x00, 0x00, 0x00, 0xFF)
#define GRAY    RGBA8(0xA0, 0xA0, 0xA0, 0xFF)
#define WHITE   RGBA8(0xFF, 0xFF, 0xFF, 0xFF)
#define GREEN   RGBA8(0x00, 0xFF, 0x00, 0xFF)
#define RED     RGBA8(0xFF, 0x00, 0x00, 0xFF)
#define ORANGE  RGBA8(0xFF, 0xA5, 0x00, 0xFF)

#define ICON_CIRCLE   "\xe2\x97\x8b"
#define ICON_CROSS    "\xe2\x95\xb3"
#define ICON_SQUARE   "\xe2\x96\xa1"
#define ICON_TRIANGLE "\xe2\x96\xb3"
#define ICON_UPDOWN   "\xe2\x86\x95"
#define ICON_CHECK    "\xe2\x98\x92"
#define ICON_UNCHECK  "\xe2\x96\xa1"

typedef enum {
    NONE,
    SIMPLE,
    WARNING,
    ERROR,
    CONFIRM_AND_CANCEL,
} popup_type;

typedef enum {
    LEFT,
    RIGHT,
    CENTER,
} line_align;

typedef struct popup_line {
    const char *string;
    line_align align;
    int padding[4];
    int color;
    int width;
} popup_line;

void init_console();
void draw_start();
void draw_end();
void draw_text(uint32_t y, const char* text, uint32_t color);
void draw_loop_text(uint32_t y, const char *text, uint32_t color);
void clear_screen();

void draw_popup(popup_type type, popup_line lines[]);
void open_popup(popup_type type, popup_line lines[]);
void close_popup();

#endif
