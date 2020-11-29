#ifndef INTERFACE_H_INCLUDED
#define INTERFCE_H_INCLUDED

#include <curses.h>

#include "form.h"
#include <stdlib.h>
#include <iostream>
#include <vector>
#include "rapidjson/document.h"


#define ENTER 10
#define ESCAPE 27



using namespace std;
struct MenuItems{
    string name;
    WINDOW *win;
};
struct MenuElement{
    string name;

    WINDOW *win;
    int width;
    int start_col;
    vector<MenuItems> menuItems;
};


/*
void init_curses(void);

void init_menu(vector<MenuElement*> &menu);

void init_interface(WINDOW *menubar, WINDOW *main_win, WINDOW *messagebar,vector<MenuElement*> &menu);

void draw_menubar(WINDOW *menubar,vector<MenuElement*> &menu);

void draw_menu(vector<MenuElement*> &menu, int codeMenu);

void delete_menu(MenuElement *menu);

int scroll_menu(vector<MenuElement*> &menu,int codeMenu);*/

using namespace std;

class interface
{
    public:
        interface();
        virtual ~interface(){};

        void launch();
        bool run_menu();
        bool run_addredir();
        void quit_addredir();
        rapidjson::Document* get_formValues(){return this->formValues;};
        int get_menuCode() { return this->menuCode; };
        int get_selectedItem() { return this->selectedItem; };
        void print_message(string message);


    protected:

    private:
        int menuCode;
        int selectedItem;
        rapidjson::Document *formValues;

        WINDOW *menubar;
        WINDOW *main_win;
        WINDOW *messagebar;
        FORM *form;
        FIELD* *fields;
        WINDOW *w;
        WINDOW *s;
        int term_h;
        int term_w;

        vector<MenuElement*> menu;

        void init_menu();
        void init_curses();
        void init_interface();

        void draw_menubar();
        void draw_menu(int codeMenu);
        void delete_menu(MenuElement *menu);
        int scroll_menu(int codeMenu);
};


#endif
