#include <stdio.h>
#include <psp2/ctrl.h>

#include "console.h"
#include "button.h"
#include "menu.h"

int draw_and_wait_input(menu_t menu) {
    char buf[256];
    int curr = 0;
    int start = 0;

    int count = 0;
    while (menu.items[count].text) count += 1;

    clear_screen();
    while (1) {
        draw_start();

        draw_loop_text(1, menu.title, WHITE);
        for (int i = start; i < count && i < start + 22; i += 1) {
            snprintf(buf, 256, "%s %s",
                     curr == i ? ICON_CHECK : ICON_UNCHECK,
                     menu.items[i].text);
            draw_loop_text(i - start + 3, buf, curr == i ? WHITE : GRAY);
        }

        int btn = read_btn();
        draw_end();
        if (btn & SCE_CTRL_UP) {
            curr -= 1;
            if (curr < 0) {
                curr = 0;
            }
            if (curr < start) {
                start -= 1;
            }
        } else if (btn & SCE_CTRL_DOWN) {
            curr += 1;
            if (curr >= count) {
                curr = count - 1;
            }
            if (curr == start + 22) {
                start += 1;
            }
        } else if (!(btn & SCE_CTRL_HOLD) && (btn & SCE_CTRL_ENTER)) {
            return menu.items[curr].value;
        } else if (!(btn & SCE_CTRL_HOLD) && (btn & SCE_CTRL_CANCEL)) {
            return EXIT;
        }
    }
}

int draw_and_select_plugins(menu_t menu, plugins *plgs) {
    char buf[256];
    int curr = 0;
    int start = 0;

    int count = 0;
    while (menu.items[count].text) count += 1;

    // if count == 0 just print popup

    clear_screen();
    while (1) {
        draw_start();

        draw_loop_text(1, menu.title, WHITE);
        for (int i = start; i < count && i < start + 22; i += 1) {
            //uint8_t exist = 0;
            //plugin_t *t = plgs->plugin;
            //while (t) {
            //    if (t == (plugin_t*)menu.items[i].value) {
            //        exist = 1;
            //        break;
            //    }
            //    t = t->next;
            //}

            snprintf(buf, 256, "%s %s",
                     menu.items[i].check ? ICON_CHECK : ICON_UNCHECK,
                     menu.items[i].text);
            draw_loop_text(i - start + 3, buf, curr == i ? WHITE : GRAY);
        }

        int btn = read_btn();
        draw_end();
        if (btn & SCE_CTRL_UP) {
            curr -= 1;
            if (curr < 0) {
                curr = 0;
            }
            if (curr < start) {
                start -= 1;
            }
        } else if (btn & SCE_CTRL_DOWN) {
            curr += 1;
            if (curr >= count) {
                curr = count - 1;
            }
            if (curr == start + 22) {
                start += 1;
            }
        } else if (!(btn & SCE_CTRL_HOLD) && (btn & SCE_CTRL_ENTER)) {
            menu.items[curr].check = menu.items[curr].check ? 0 : 1;
        } else if (!(btn & SCE_CTRL_HOLD) && (btn & SCE_CTRL_CANCEL)) {
            for (int i = 0; i < count; i += 1) {
                if (!menu.items[i].check) {
                    continue;
                }
                append_plugin(plgs, ((plugin_t*)menu.items[i].value)->path);
            }
            return EXIT;
        }
    }
}


