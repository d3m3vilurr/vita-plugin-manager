#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <psp2/io/stat.h>
#include <psp2/io/fcntl.h>
#include <psp2/io/dirent.h>

#include "file.h"

#define SCE_ERROR_ERRNO_ENOENT 0x80010002
#define SCE_ERROR_ERRNO_EEXIST 0x80010011
#define SCE_ERROR_ERRNO_ENODEV 0x80010013
#define SCE_ERROR_ERRNO_EINVAL 0x80010016

char *special_kernel_plugins[] = {
    // auto load
    // "ur0:tai/henkaku.skprx",
    "ux0:pspemu/adrenaline/adrenaline.skprx",
    NULL,
};

char *special_user_plugins[] = {
    "ur0:tai/henkaku.suprx",
    "ux0:app/MLCL00001/henkaku.suprx",
    "vs0:sys/external/libpgf.suprx",
    "ux0:pspemu/adrenaline/adrenaline.suprx",
    NULL,
};

int exists(const char *path) {
    SceIoStat stat = {0};
    int ret = sceIoGetstat(path, &stat);
    return ret != SCE_ERROR_ERRNO_ENOENT &&
        ret != SCE_ERROR_ERRNO_ENODEV;
}

int is_dir(const char *path) {
    SceIoStat stat = {0};
    if (sceIoGetstat(path, &stat) == SCE_ERROR_ERRNO_ENOENT) {
        return 0;
    }
    return SCE_S_ISDIR(stat.st_mode);
}

int mvdir(const char *src, const char *dest) {
    if (strcasecmp(src, dest) == 0) {
        return -1;
    }
    // TODO need we any guards like vitashell?
    return sceIoRename(src, dest);
}

int mkdir(const char *path, int mode) {
    if (exists(path)) {
        if (is_dir(path)) {
            return 0;
        }
        return SCE_ERROR_ERRNO_EEXIST;
    }
    int len = strlen(path);
    char p[len];
    memset(p, 0, len);
    for (int i = 0; i < len; i++) {
        if (path[i] == '/') {
            if (strcmp(p, "ux0:/") == 0) {
                p[i] = path[i];
                continue;

            }
            if (!exists(p)) {
                sceIoMkdir(p, mode);
            } else {
                if (!is_dir(p)) {
                    return SCE_ERROR_ERRNO_EINVAL;
                }
            }
        }
        p[i] = path[i];
    }
    sceIoMkdir(path, mode);
    return 0;
}

int rmdir(const char *path) {
    SceUID dfd = sceIoDopen(path);
    if (dfd < 0) {
        int ret = sceIoRemove(path);
        if (ret < 0)
            return ret;
    }
    int res = 0;

    do {
        SceIoDirent dir;
        memset(&dir, 0, sizeof(SceIoDirent));

        res = sceIoDread(dfd, &dir);
        if (res > 0) {
            if (strcmp(dir.d_name, ".") == 0 || strcmp(dir.d_name, "..") == 0)
                continue;

            char *new_path = malloc(strlen(path) + strlen(dir.d_name) + 2);
            snprintf(new_path, 1024, "%s/%s", path, dir.d_name);

            if (SCE_S_ISDIR(dir.d_stat.st_mode)) {
                int ret = rmdir(new_path);
                if (ret <= 0) {
                    free(new_path);
                    sceIoDclose(dfd);
                    return ret;
                }
            } else {
                int ret = sceIoRemove(new_path);
                if (ret < 0) {
                    free(new_path);
                    sceIoDclose(dfd);
                    return ret;
                }
            }

            free(new_path);
        }
    } while (res > 0);

    sceIoDclose(dfd);

    int ret = sceIoRmdir(path);
    if (ret < 0)
        return ret;

    return 1;
}

int find_special_plugins(plugins *kernel, plugins *user) {
    int i;
    for (i = 0; special_kernel_plugins[i]; i++) {
        if (!exists(special_kernel_plugins[i])) {
            continue;
        }
        append_plugin(kernel, special_kernel_plugins[i]);
    }

    for (i = 0; special_user_plugins[i]; i++) {
        if (!exists(special_user_plugins[i])) {
            continue;
        }
        append_plugin(user, special_user_plugins[i]);
    }
    return 0;
}

int find_plugins(plugins *kernel, plugins *user) {
    int root_len = strlen(APP_DIR);
    SceUID dfd = sceIoDopen(APP_DIR);
    if (dfd < 0) {
        return 1;
    }

    while (1) {
        SceIoDirent dir;
        memset(&dir, 0, sizeof(SceIoDirent));

        int res = sceIoDread(dfd, &dir);
        if (res <= 0) {
            sceIoDclose(dfd);
            return 0;
        }
        if (strcmp(dir.d_name, ".") == 0 || strcmp(dir.d_name, "..") == 0) {
            continue;
        }
        int filename_len = strlen(dir.d_name);
        if (filename_len < 6) {
            continue;
        }
        char *full_path = malloc(root_len + filename_len + 2);
        snprintf(full_path, 1024, "%s/%s", APP_DIR, dir.d_name);

        if (SCE_S_ISDIR(dir.d_stat.st_mode)) {
            // need to recursive?
            free(full_path);
            continue;
        }
        char *ext = dir.d_name + filename_len - 6;
        if (strcmp(ext, ".suprx") == 0) {
            append_plugin(user, full_path);
        } else if (strcmp(ext, ".skprx") == 0) {
            append_plugin(kernel, full_path);
        } else {
            free(full_path);
            continue;
        }
    }
}
