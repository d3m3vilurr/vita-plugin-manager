#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/ctrl.h>

#include "console.h"
#include "menu.h"
#include "appdb.h"
#include "plugin.h"
#include "file.h"
#include "config.h"
#include "const.h"
#include "button.h"

applist apps = {0};

plugins kernel = {0};
// specialized
plugins sce_shell = {0};
plugins all = {0};
plugins user = {0};
config_t config = {0};

int load_plugins() {
    find_special_plugins(&kernel, &user);
    find_plugins(&kernel, &user);
    return 0;
}

// TODO extract menu
// TODO initial index
int config_kernel_plugins() {
    list_entry_t *entry = calloc(kernel.count + 1, sizeof(list_entry_t));
    if (entry == NULL) {
        // TODO error
        return -1;
    }
    config_group *group = find_group(&config, "KERNEL");
    uint8_t is_new = 0;
    if (!group) {
        group = calloc(1, sizeof(config_group));
        group->plugins = calloc(1, sizeof(plugins));
        is_new = 1;
    }
    plugin_t *plg = kernel.plugin;
    int i = 0;
    while (plg) {
        entry[i].text = plg->path;
        entry[i].value = (int)plg;
        entry[i].check = exists_plugin(group->plugins, plg->path);
        plg = plg->next;
        i += 1;
    }
    char buf[256];
    snprintf(buf, 256, "%s: KERNEL", menu_strings.select_plugin);
    menu_t menu = {
        buf,
        entry,
    };
    clear_plugins(group->plugins);
    draw_and_select_plugins(menu, group->plugins);
    if (group->plugins->count) {
        if (is_new)
            append_group(&config, group);
    }
    free(entry);
    store_config(&config);
    return 0;
}

int config_user_plugins(appinfo *info) {
    list_entry_t *entry = calloc(user.count + 1, sizeof(list_entry_t));
    if (entry == NULL) {
        // TODO error
        return -1;
    }
    config_group *group = find_group(&config, info->title_id);
    uint8_t is_new = 0;
    if (!group) {
        group = calloc(1, sizeof(config_group));
        group->plugins = calloc(1, sizeof(plugins));
        group->appid = strdup(info->title_id);
        is_new = 1;
    }
    plugin_t *plg = user.plugin;
    int i = 0;
    while (plg) {
        entry[i].text = plg->path;
        entry[i].value = (int)plg;
        entry[i].check = exists_plugin(group->plugins, plg->path);
        plg = plg->next;
        i += 1;
    }
    char buf[256];
    snprintf(buf, 256, "%s: %s - %s",
             menu_strings.select_plugin,
             info->title_id,
             info->title);
    menu_t menu = {
        buf,
        entry,
    };
    clear_plugins(group->plugins);
    draw_and_select_plugins(menu, group->plugins);

    if (group->plugins->count) {
        if (is_new)
            append_group(&config, group);
    }

    free(entry);
    store_config(&config);
    return 0;
}

int config_user_app() {
    list_entry_t *entry = calloc(apps.count + 1, sizeof(list_entry_t));
    if (entry == NULL) {
        // TODO error
        return -1;
    }
    appinfo *info = apps.items;
    int i = 0;
    while (info) {
        entry[i].text = info->title;
        entry[i].value = (int)info;
        info = info->next;
        i += 1;
    }
    menu_t menu = {
        menu_strings.select_app,
        entry,
    };

    while (1) {
        int input = draw_and_wait_input(menu);
        if (input == EXIT) {
            free(entry);
            return EXIT;
        }
        config_user_plugins((appinfo*)input);
    }
}

int config_special_user_app(char *virtual_id, char *title) {
    appinfo info = {0};
    strcpy(info.title_id, virtual_id);
    strcpy(info.title, title);
    return config_user_plugins(&info);
}

int main_menu() {
    menu_t menu = {
        menu_strings.main_menu,
        (list_entry_t[]) {
            { menu_strings.kernel_plugins, CONFIG_KERNEL_PLUGINS },
            { menu_strings.shell_plugins, CONFIG_SHELL_PLUGINS },
            { menu_strings.all_user_app_plugins, CONFIG_ALL_USER_APP_PLUGINS},
            { menu_strings.user_app, CONFIG_USER_APP },
            {0},
        }
    };

    switch (draw_and_wait_input(menu)) {
        case CONFIG_KERNEL_PLUGINS:
            config_kernel_plugins();
            return 0;
        case CONFIG_SHELL_PLUGINS:
            config_special_user_app("main", "SceShell");
            return 0;
        case CONFIG_ALL_USER_APP_PLUGINS:
            config_special_user_app("ALL", menu_strings.for_all_apps);
            return 0;
        case CONFIG_USER_APP:
            config_user_app();
            return 0;
        case EXIT:
            return EXIT;
        default:
            return 0;
    }
}

#define POPUP(msg) \
    do { \
        popup_line lines[] = { \
            {.string=(msg), .padding={20, 0, 20, 0}, .color=ORANGE}, \
            {0}, \
        }; \
        open_popup(WARNING, lines); \
    } while (0)

int main() {
    vita2d_init();
    init_console();

    // backup current setting?
    mkdir(APP_DIR, 0777);

    // TODO print warning popup
    load_plugins();
    parse_config(&config);
    get_applist(&apps);
    // backup orig setting

    while (main_menu() != EXIT);
    if (reload_config() < 0) {
        // something wrong
        POPUP("RELOAD ERROR!");
    } else {
        POPUP(menu_strings.succ_reload_config);
    }
    sceKernelDelayThread(1000000);
    sceKernelExitProcess(0);
}
