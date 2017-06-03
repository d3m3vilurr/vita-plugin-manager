#ifndef __FILE_H__
#define __FILE_H__

#include "plugin.h"

int exists(const char *path);
int is_dir(const char *path);

int mkdir(const char *path, int mode);
int mvdir(const char *src, const char *dest);
int rmdir(const char *path);
int find_special_plugins(plugins *kernel, plugins *user);
int find_plugins(plugins *kernel, plugins *user);

#define APP_DIR "ux0:data/taiplgmgr"
#endif
