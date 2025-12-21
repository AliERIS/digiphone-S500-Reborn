/*
 * Simple Menu System for DZ09
 */
#ifndef _MENU_H_
#define _MENU_H_

#include "systemconfig.h"

typedef enum {
    MENU_ACTION_NONE = 0,
    MENU_ACTION_SUBMENU,
    MENU_ACTION_FUNCTION
} TMenuActionType;

typedef struct menu_item MenuItem;
typedef struct menu Menu;

typedef void (*MenuActionFunc)(void);

struct menu_item {
    const char *title;
    TMenuActionType actionType;
    union {
        Menu *submenu;
        MenuActionFunc function;
    } action;
};

struct menu {
    const char *title;
    MenuItem *items;
    uint8_t itemCount;
    uint8_t selectedIndex;
    Menu *parent;
};

/* Menu Functions */
void Menu_Initialize(void);
void Menu_Render(Menu *menu);
void Menu_NavigateUp(Menu *menu);
void Menu_NavigateDown(Menu *menu);
void Menu_NavigateLeft(Menu *menu);
void Menu_NavigateRight(Menu *menu);
Menu* Menu_Select(Menu *menu);
Menu* Menu_Back(Menu *menu);
Menu* Menu_GetRoot(void);
void Menu_InvalidateCache(void);
int ShowOptionsMenu(Menu *menu);

#endif /* _MENU_H_ */
