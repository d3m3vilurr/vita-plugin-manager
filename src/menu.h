#ifndef __MENU_H__
#define __MENU_H__

#include "plugin.h"

enum {
    MAIN_MENU = 1,
    CONFIG_KERNEL_PLUGINS,
    CONFIG_SHELL_PLUGINS,
    CONFIG_USER_APP,
    CONFIG_USER_APP_PLUGINS,
    CONFIG_ALL_USER_APP_PLUGINS,
    EXIT = -1,
};

typedef struct {
    char *main_menu;
    char *kernel_plugins;
    char *shell_plugins;
    char *user_app;
    char *user_app_plugins;
    char *all_user_app_plugins;
    char *select_app;
    char *select_plugin;
    char *for_all_apps;
    char *confirm;
    char *cancel;
    char *back;
    char *exit;
    char *succ_reload_config;
} menu_strings_t;

typedef struct {
    char *text;
    int value;
    uint8_t check;
} list_entry_t;

typedef struct {
    char *title;
    list_entry_t *items;
} menu_t;

int draw_and_wait_input(menu_t menu);
int draw_and_select_plugins(menu_t menu, plugins *plgs);

extern menu_strings_t menu_strings;
#endif
