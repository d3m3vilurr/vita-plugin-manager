#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getline.h>
#include <trim.h>

#include "file.h"
#include "config.h"

int taiReloadConfig();

const char *config_filepath = "ux0:tai/config.txt";
const char *config_bak_filepath = "ux0:tai/config.txt.bak";

config_group* find_group(config_t *config, char *appid) {
    config_group *group = config->group;
    while (group) {
        if (strcmp(group->appid, appid) == 0) {
            return group;
        }
        group = group->next;
    }
    return NULL;
}

int append_group(config_t *config, config_group *group) {
    config_group *temp = config->group;
    if (!temp) {
        config->group = group;
        return 0;
    }
    while(temp->next) temp = temp->next;
    temp->next = group;
    return 1;
}

void free_group(config_group *group) {
    if (!group) {
        return;
    }
    if (group->plugins) {
        clear_plugins(group->plugins);
        free(group->plugins);
    }
    free(group->appid);
    free(group);
}

int remove_group(config_t *config, config_group *group) {
    config_group *temp = config->group;
    config_group *prev = config->group;
    while (temp) {
        if (temp == group) {
            break;
        }
        prev = temp;
        temp = temp->next;
    }
    // wft?
    if (!temp || temp != group) {
        free_group(group);
        return 0;
    }
    // changed head
    if (temp == config->group) {
        config->group = temp->next;
        free_group(temp);
        return 0;
    }
    prev->next = temp->next;
    free_group(temp);
    return 0;
}

int parse_config(config_t *config) {
    if (!exists(config_filepath)) {
        return 0;
    }

    FILE *fp = fopen(config_filepath, "r");
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    config_group *tail = config->group;
    while(tail && tail->next) tail = tail->next;

    plugin_t *plg = NULL;
    config_group *group = NULL;
    while ((read = getline(&line, &len, fp)) != -1) {
        trim(line);
        if (!strlen(line)) {
            continue;
        }

        switch(line[0]) {
            case '*':
                group = find_group(config, line + 1);
                if (!group) {
                    if (tail == NULL) {
                        group = tail = config->group =
                            calloc(1, sizeof(config_group));
                    } else {
                        group = tail = tail->next =
                            calloc(1, sizeof(config_group));
                    }
                    group->plugins = calloc(1, sizeof(plugins));
                    group->appid = strdup(line + 1);
                    plg = NULL;
                } else {
                    plg = group->plugins->plugin;
                    while (plg && plg->next) plg = plg->next;
                }
                continue;
            case '#':
                continue;
            default:
                // guard
                if (!group) {
                    continue;
                }
                append_plugin(group->plugins, strdup(line));
        }

    }
    if (line) free(line);
    fclose(fp);
    return 1;
}

int store_config(config_t *config) {
    FILE *fp = fopen(config_filepath, "w");

    config_group *temp = config->group;
    while (temp) {
        fprintf(fp, "*%s\n", temp->appid);
        plugin_t *plg = NULL;
        if (temp->plugins && temp->plugins->plugin) {
            plg = temp->plugins->plugin;
        }
        while(plg) {
            fprintf(fp, "%s\n", plg->path);
            plg = plg->next;
        }
        temp = temp->next;
    }
    fclose(fp);
    return 0;
}
