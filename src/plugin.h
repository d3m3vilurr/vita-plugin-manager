#ifndef __PLUGIN_H__
#define __PLUGIN_H__

typedef struct _plugin_t plugin_t;
struct _plugin_t {
    char *path;
    plugin_t *next;
};

typedef struct {
    int count;
    plugin_t *plugin;
} plugins;

int exists_plugin(plugins *plgs, char *path);
int append_plugin(plugins *plgs, char *path);
int clear_plugins(plugins *plgs);
#endif
