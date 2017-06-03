#ifndef __CONFIG_H__
#define __CONFIG_H__

typedef struct _config_group config_group;
struct _config_group{
    char *appid;
    plugins *plugins;
    config_group *next;
};

typedef struct {
    config_group *group;
} config_t;

config_group* find_group(config_t *config, char *appid);
int append_group(config_t *config, config_group *group);
int remove_group(config_t *config, config_group *group);
int parse_config(config_t *config);
int store_config(config_t *config);
int reload_config();
#endif
