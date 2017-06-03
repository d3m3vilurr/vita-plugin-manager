#include <stdlib.h>
#include <string.h>
#include "plugin.h"

int exists_plugin(plugins *plgs, char *path) {
    if (!plgs->plugin) {
        return 0;
    }
    plugin_t *plg = plgs->plugin;
    while (plg) {
        if (strcmp(plg->path, path) == 0) {
            return 1;
        }
        plg = plg->next;
    }
    return 0;

}
int append_plugin(plugins *plgs, char *path) {
    plugin_t *plg;
    if (!plgs->plugin) {
        plg = plgs->plugin = calloc(1, sizeof(plugin_t));
    } else {
        plg = plgs->plugin;
        while (1) {
            if (strcmp(plg->path, path) == 0) {
                return -1;
            }
            if (!plg->next) {
                break;
            }
            plg = plg->next;
        }
        plg = plg->next = calloc(1, sizeof(plugin_t));
    }
    plgs->count += 1;
    plg->path = path;
    return 0;
}

int clear_plugins(plugins *plgs) {
    plgs->count = 0;
    plugin_t *plg = plgs->plugin;
    while (plg) {
        plugin_t *curr = plg;
        plg = plg->next;
        free(curr);
    }
    plgs->plugin = NULL;
    return 0;
}
