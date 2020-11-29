#include"interface.h"
const char *key_UP="^[[A";
const char *key_DOWN="^[[B";
const char *key_LEFT="^[[D";
const char *key_RIGHT="^[[C";
const char *key_BACKSPACE="^?";
const char *key_DEL="^[[3~";
const char *key_ENTER="^J";
const char *key_ORIGIN="^[[H";
const char *key_END="^[[F";
const char *key_PAGEUP="^[[5~";
const char *key_PAGEDOWN="^[[6~";
const char *key_ESC="^[";
const char *key_F1="^[[OP~";
const char *key_F2="^[[OQ~";
const char *key_F3="^[[OR~";
const char *key_F4="^[[OS~";
const char *key_F5="^[[15~";
const char *key_F6="^[[17~";
const char *key_F7="^[[18~";
const char *key_F8="^[[19~";
const char *key_F9="^[[20~";
const char *key_F12="^[[24~";



struct field{
    string caption;
    string value="";
};

static void input_key(char *key){
    int i=0;
    i=0;
    key[i]=getch();
    if (key[i]==27){
        key[i]='^';
        i++;
        key[i]='[';
    }
    else if(key[i]==10){
        key[i]='^';
        i++;
        key[i]='J';
    }
    else if(key[i]==127){
        key[i]='^';
        i++;
        key[i]='?';
    }
    nodelay(stdscr,true);
    while( key[i] !=-1){
        i++;
        key[i]=getch();
    }
    key[i]=0x0;
};

interface::interface(){


}
void interface::launch(){
    this->init_curses();
    this->init_menu();

    getmaxyx(stdscr, this->term_h, this->term_w);
    this->init_interface();
}

void interface::init_curses(){
    initscr();

    start_color();
    init_pair(1,COLOR_RED,COLOR_BLUE);
    init_pair(2,COLOR_BLUE,COLOR_BLUE);
    init_pair(3,COLOR_RED,COLOR_WHITE);
    init_pair(4,COLOR_WHITE,COLOR_BLUE);
    init_pair(5,COLOR_BLUE,COLOR_WHITE);
    curs_set(0);
    noecho();
    keypad(stdscr,TRUE);
}

void interface::init_menu(){
    MenuElement *sessionMenu = (MenuElement*)malloc((int)sizeof(MenuElement));
    sessionMenu->name = "Session";
    sessionMenu->start_col=1;
    sessionMenu->width=18;
    sessionMenu->menuItems.push_back({"Ouvrir",NULL});
    sessionMenu->menuItems.push_back({"Fermer",NULL});
    sessionMenu->menuItems.push_back({"Redemarrer Fbx",NULL});

    MenuElement *redirMenu = (MenuElement*)malloc((int)sizeof(MenuElement));
    redirMenu->name = "Redirection";
    redirMenu->start_col=(int)sessionMenu->name.size() +6;
    redirMenu->width=18;
    redirMenu->menuItems.push_back({"Lister",NULL});
    redirMenu->menuItems.push_back({"Ajouter",NULL});
    redirMenu->menuItems.push_back({"Supprimer",NULL});
    redirMenu->menuItems.push_back({"Voir port X",NULL});
    redirMenu->menuItems.push_back({"Update port X",NULL});
    redirMenu->menuItems.push_back({"Sauvegarder",NULL});
    redirMenu->menuItems.push_back({"Charger",NULL});
    this->menu.push_back(sessionMenu);
    this->menu.push_back(redirMenu);
}


void interface::init_interface(){
    bkgd(COLOR_PAIR(2));
    move(2,1);
    wattron(stdscr,COLOR_PAIR(4));

    this->menubar=subwin(stdscr,3,this->term_w,0,0);
    this->main_win=subwin(stdscr,this->term_h-4,this->term_w,3,0);
    wattron(this->menubar,COLOR_PAIR(4));
    box(this->menubar, ACS_VLINE, ACS_HLINE);
    wattroff(this->menubar,COLOR_PAIR(4));
    wattron(this->main_win,COLOR_PAIR(4));
    box(this->main_win, ACS_VLINE, ACS_HLINE);
    wmove(this->main_win,5,1);
    wprintw(this->main_win,"Press F(X) to open the menus. ");
    wprintw(this->main_win,"ESC quits.");
    wattroff(this->main_win,COLOR_PAIR(4));
    this->draw_menubar();
    this->messagebar=subwin(stdscr,1,this->term_w-1,this->term_h-1,1);
    this->w=NULL;
    this->s=NULL;
    this->form=NULL;
    this->fields=NULL;
    this->formValues = new rapidjson::Document;
    this->formValues->SetObject();
    refresh();

}

bool interface::run_menu(){
    int key,selected_item,codeMenu;
    key=0;
    selected_item=0;
    codeMenu=-1;
    do {
        if(codeMenu<0){
            key=getch();
            if (key==KEY_F(1)) {
                codeMenu=0;
            }
            else if (key==KEY_F(2)) {
                codeMenu=1;
            }
            else if (key==ESCAPE){
                break;
            }
        }
        else{
            draw_menu(codeMenu);
            selected_item=scroll_menu(codeMenu);
            delete_menu(menu[codeMenu]);
            touchwin(menubar);
            touchwin(main_win);
            wrefresh(menubar);
            wrefresh(main_win);
            if (selected_item>=0){
                this->menuCode = codeMenu;
                this->selectedItem = selected_item;
                return true;
                }
                else{
                    if(selected_item==-2){
                        if(codeMenu+1<=(int)menu.size()-1){
                            codeMenu=codeMenu+1;
                        }
                        else{
                            codeMenu=0;
                        }
                    }
                    else if(selected_item==-3){
                        if(codeMenu-1>=0){
                            codeMenu=codeMenu-1;
                        }
                        else{
                            codeMenu=(int)menu.size()-1;
                        }
                    }
                    else{
                        codeMenu=-1;
                    }
                }
            }
        } while (key!=ESCAPE);
        delwin(messagebar);
        delwin(menubar);
        delwin(main_win);
        endwin();
        return false;
    }

void interface::draw_menubar()
{
    int i=1;
    for(auto it=this->menu.begin();it<this->menu.end();it++)
    {
        wattron(this->menubar,COLOR_PAIR(4));
        wmove(this->menubar,1,(*it)->start_col);
        wprintw(this->menubar,(*it)->name.c_str());
        wattroff(this->menubar,COLOR_PAIR(4));
        wattron(this->menubar,COLOR_PAIR(4));
        wprintw(this->menubar,"(F%d)",i);
        wattroff(this->menubar,COLOR_PAIR(4));
        i++;
    }
}

void interface::draw_menu(int codeMenu)
{
    this->menu[codeMenu]->win = newwin(this->menu[codeMenu]->menuItems.size()+2+1,this->menu[codeMenu]->width+2,0,this->menu[codeMenu]->start_col-1); //todo :max de titre et items
    //werase(this->menu[codeMenu]->win);
    wbkgd(this->menu[codeMenu]->win,COLOR_PAIR(2));

    wattron(this->menu[codeMenu]->win,COLOR_PAIR(1)|A_BOLD|A_UNDERLINE);
    wmove(this->menu[codeMenu]->win,1,2);
    waddstr(this->menu[codeMenu]->win,this->menu[codeMenu]->name.c_str());
    wattroff(this->menu[codeMenu]->win,COLOR_PAIR(1)|A_BOLD|A_UNDERLINE);

    wattron(this->menu[codeMenu]->win,COLOR_PAIR(4));
    box(this->menu[codeMenu]->win,ACS_VLINE,ACS_HLINE);
    wattroff(this->menu[codeMenu]->win,COLOR_PAIR(4));
    int ligne=1;
    for(auto it=this->menu[codeMenu]->menuItems.begin();it<this->menu[codeMenu]->menuItems.end();it++){
        it->win=subwin(this->menu[codeMenu]->win,1,it->name.size(),ligne+1,this->menu[codeMenu]->start_col+2);
        if(ligne==1){
            wbkgd(it->win,COLOR_PAIR(5));
            mvwprintw(it->win,0,0,it->name.c_str());
        }
        else{
        wbkgd(it->win,COLOR_PAIR(4));
        mvwprintw(it->win,0,0,it->name.c_str());
        }
        ligne++;
    }
    wrefresh(this->menu[codeMenu]->win);
}

void interface::delete_menu(MenuElement *menu)
{
    for (auto it=menu->menuItems.begin();it<menu->menuItems.end();it++){
        delwin(it->win);
    }
    delwin(menu->win);
}

int interface::scroll_menu(int codeMenu)
{
    int key;
    int selected=0;
    while (1) {
        key=getch();
        if (key==KEY_DOWN || key==KEY_UP) {
            wbkgd(this->menu[codeMenu]->menuItems[selected].win,COLOR_PAIR(4));
            wnoutrefresh(this->menu[codeMenu]->menuItems[selected].win);
            if (key==KEY_DOWN) {
                if(selected+1 < (int)this->menu[codeMenu]->menuItems.size()){
                    selected=(selected+1);
                }
                else{
                    selected=0;
                }
            }
            else {
                if(selected-1 <0){
                    selected=this->menu[codeMenu]->menuItems.size()-1;
                }
                else{
                    selected=selected-1;
                }
            }
            wbkgd(this->menu[codeMenu]->menuItems[selected].win,COLOR_PAIR(5));
            wnoutrefresh(this->menu[codeMenu]->menuItems[selected].win);
            doupdate();
        }
        else if (key==KEY_LEFT || key==KEY_RIGHT) {
            if (key==KEY_RIGHT) {
                return -2;
            }
            else {
                return -3;
            }
        }
        else if (key==ESCAPE) {
            return -1;
        }
        else if (key==ENTER) {
            return selected;
        }
    }
}

void interface::print_message(string message){
    wclear(this->messagebar);
    mvwprintw(this->messagebar,0,0,message.c_str(),COLOR_PAIR(5));
    wrefresh(this->messagebar);
}

void interface::quit_addredir(){
    curs_set(0);
    unpost_form(this->form);
    free_form(this->form);

    int i=0;
    while(this->fields[i] != 0){
        free_field(this->fields[i]);
        i++;
    }
    delete[] this->fields;
    delwin(this->w);
    delwin(this->s);
    wrefresh(this->w);
    w=NULL;
    s=NULL;
    wrefresh(this->main_win);
    keypad(stdscr,true);

}

bool interface::run_addredir(){
    curs_set(1);
    this->fields=new FIELD*[20];

    vector<field> fieldsLabels;
    fieldsLabels.push_back({"IP source","0.0.0.0"});
    fieldsLabels.push_back({"Port local"});
    fieldsLabels.push_back({"IP destination"});
    fieldsLabels.push_back({"Debut plage de port"});
    fieldsLabels.push_back({"Fin plage de port"});
    fieldsLabels.push_back({"Protocol IP (TCP/UDP)","TCP"});
    fieldsLabels.push_back({"Commentaire"});
    fieldsLabels.push_back({"Actif (O/N)","O"});
    int num=0;
    for(auto it=fieldsLabels.begin();it<fieldsLabels.end();it++){
        this->fields[num]=new_field(1, 20, 1+num, 1,0,0);
        set_field_buffer(this->fields[num], 0, it->caption.c_str());
        set_field_fore(this->fields[num],COLOR_PAIR(4));
        set_field_back(this->fields[num],COLOR_PAIR(4));
        field_opts_off(this->fields[num], O_ACTIVE);
        num++;
        this->fields[num]=new_field(1, 20, num, 22,0,0);
        set_field_fore(this->fields[num],COLOR_PAIR(4));
            set_field_back(this->fields[num],COLOR_PAIR(4)|A_UNDERLINE);
        if(it->value != ""){
            set_field_buffer(this->fields[num], 0, it->value.c_str());
        }
        num++;
    }


    //set_field_fore(fields[1],A_REVERSE);
    this->fields[num] = (FIELD*)0;
    set_field_type(fields[1], TYPE_REGEXP, "[0-9]{0,3}[.][0-9]{0,3}[.][0-9]{0,3}[.][0-9]{0,3}");
    set_field_type(fields[3], TYPE_INTEGER, 0, 0, 0);
    set_field_type(fields[5], TYPE_REGEXP, "[0-9]{0,3}[.][0-9]{0,3}[.][0-9]{0,3}[.][0-9]{0,3}");
    set_field_type(fields[7], TYPE_INTEGER, 0, 0, 0);
    set_field_type(fields[9], TYPE_INTEGER, 0, 0, 0);
    char **liste=(char**)malloc(sizeof(char*)*3);
    liste[0]=(char*)"TCP"; liste[1]=(char*)"UDP"; liste[2]=NULL;
    set_field_type(fields[11], TYPE_ENUM, liste, 0,0);
    set_field_type(fields[13], TYPE_ALNUM, 20);
    char **list2 = (char**)malloc(sizeof(char*)*3);
    list2[0]=(char*)"O"; list2[1]=(char*)"N"; list2[2]=NULL;
    set_field_type(fields[15], TYPE_ENUM, list2, 0,0);
    set_field_status(fields[0],1);
    this->form=new_form(this->fields);
//  int r=4;int c=40;
    //scale_form(form,&r,&c);

    //box(this->main_win,0, 0);
/*     vector<FIELD*> test;
    test.push_back(new_field(1, 20, 1, 22,0,0));
    test.push_back(new_field(2, 20, 1, 22,0,0));
    test.push_back(NULL);
    this->form=new_form(&(test.front())); */

    this->w = derwin(this->main_win,this->term_h-6,this->term_w-2,1,1);
    set_form_win(form,this->w);
    this->s = derwin(w,this->term_h-6,this->term_w-4,0,0);
    set_form_sub(form, this->s);
    wbkgd(this->w,COLOR_PAIR(4));
    wbkgd(this->s,COLOR_PAIR(4));
    //box(w,0, 0);
    post_form(form);
    mvwprintw(this->w,this->term_h-7,1,"(F12) pour valider");
    wrefresh(this->w);
//box(w,0, 0);
//box(this->main_win,0, 0);
wrefresh(this->main_win);
keypad(stdscr,false);
char* key;
key = (char*)malloc(sizeof(char)*7);
    while (1) {
        input_key(key);
            if (!strcmp(key,key_UP)){
                form_driver(form, REQ_PREV_FIELD);
                form_driver(form, REQ_END_LINE);
            }
            else if (!strcmp(key,key_DOWN)){
                    form_driver(form, REQ_NEXT_FIELD);
                    form_driver(form, REQ_END_LINE);
            }
            else if (!strcmp(key,key_LEFT)){
                    form_driver(form, REQ_PREV_CHAR);
            }
            else if (!strcmp(key,key_RIGHT)){
                    form_driver(form, REQ_NEXT_CHAR);
            }
            else if (!strcmp(key,key_BACKSPACE)){
                form_driver(form, REQ_DEL_PREV);
            }
            else if (!strcmp(key,key_DEL)){
                form_driver(form, REQ_DEL_CHAR);
            }
            else if (!strcmp(key,key_F8)){
                form_driver(form, REQ_DEL_LINE);
            }
            else if (!strcmp(key,key_ESC)){
                this->quit_addredir();
                break;
                return false;
            }
            else if (!strcmp(key,key_ENTER)){
                form_driver(form, REQ_NEXT_FIELD);
            }
            else if (!strcmp(key,key_F12)){
                form_driver(form, REQ_VALIDATION);
//                bool OK=true;
//                int i=0;
//                while(this->fields[i] != 0){
//
//                    if(!field_buffer(this->fields[i],0)){
//                        OK=false;
//                    }
//                    i+=2;
//                }

                formValues->RemoveAllMembers();
                formValues->AddMember("src_ip",rapidjson::StringRef(field_buffer(this->fields[1],0)),formValues->GetAllocator());
                formValues->AddMember("lan_port",rapidjson::StringRef(field_buffer(this->fields[3],0)),formValues->GetAllocator());
                formValues->AddMember("lan_ip",rapidjson::StringRef(field_buffer(this->fields[5],0)),formValues->GetAllocator());
                formValues->AddMember("wan_port_start",rapidjson::StringRef(field_buffer(this->fields[7],0)),formValues->GetAllocator());
                formValues->AddMember("wan_port_end",rapidjson::StringRef(field_buffer(this->fields[9],0)),formValues->GetAllocator());
                formValues->AddMember("ip_proto",rapidjson::StringRef(field_buffer(this->fields[11],0)),formValues->GetAllocator());
                formValues->AddMember("comment",rapidjson::StringRef(field_buffer(this->fields[13],0)),formValues->GetAllocator());
                formValues->AddMember("enabled",rapidjson::StringRef(field_buffer(this->fields[15],0)),formValues->GetAllocator());

                return true;
            }
            else{
                if(strlen(key)==1)
                form_driver(form, (int)key[0]);
            }

    }
    keypad(stdscr,true);
return false;

}
/*
void init_curses(){
    initscr();

    start_color();
    init_pair(1,COLOR_RED,COLOR_BLUE);
    init_pair(2,COLOR_BLUE,COLOR_BLUE);
    init_pair(3,COLOR_RED,COLOR_WHITE);
    init_pair(4,COLOR_WHITE,COLOR_BLUE);
    init_pair(5,COLOR_BLUE,COLOR_WHITE);
    curs_set(0);
    noecho();
    keypad(stdscr,TRUE);
}

void draw_menubar(WINDOW *menubar,vector<MenuElement*> &menu)
{
    int i=1;
    for(auto it=menu.begin();it<menu.end();it++)
    {
        wattron(menubar,COLOR_PAIR(4));
        wmove(menubar,1,(*it)->start_col);
        wprintw(menubar,(*it)->name.c_str());
        wattroff(menubar,COLOR_PAIR(4));
        wattron(menubar,COLOR_PAIR(4));
        wprintw(menubar,"(F%d)",i);
        wattroff(menubar,COLOR_PAIR(4));
        i++;
    }
}

void draw_menu(vector<MenuElement*> &menu, int codeMenu)
{
    menu[codeMenu]->win = newwin(menu[codeMenu]->menuItems.size()+2+1,menu[codeMenu]->width+2,0,menu[codeMenu]->start_col-1); //todo :max de titre et items
    //werase(menu[codeMenu]->win);
    wbkgd(menu[codeMenu]->win,COLOR_PAIR(2));

    wattron(menu[codeMenu]->win,COLOR_PAIR(1)|A_BOLD|A_UNDERLINE);
    wmove(menu[codeMenu]->win,1,2);
    waddstr(menu[codeMenu]->win,menu[codeMenu]->name.c_str());
    wattroff(menu[codeMenu]->win,COLOR_PAIR(1)|A_BOLD|A_UNDERLINE);

    wattron(menu[codeMenu]->win,COLOR_PAIR(4));
    box(menu[codeMenu]->win,ACS_VLINE,ACS_HLINE);
    wattroff(menu[codeMenu]->win,COLOR_PAIR(4));
    int ligne=1;
    for(auto it=menu[codeMenu]->menuItems.begin();it<menu[codeMenu]->menuItems.end();it++){
        it->win=subwin(menu[codeMenu]->win,1,it->name.size(),ligne+1,menu[codeMenu]->start_col+2);
        if(ligne==1){
            wbkgd(it->win,COLOR_PAIR(5));
            mvwprintw(it->win,0,0,it->name.c_str());
        }
        else{
        wbkgd(it->win,COLOR_PAIR(4));
        mvwprintw(it->win,0,0,it->name.c_str());
        }
        ligne++;
    }
    wrefresh(menu[codeMenu]->win);
}

void delete_menu(MenuElement *menu)
{
    for (auto it=menu->menuItems.begin();it<menu->menuItems.end();it++){
        delwin(it->win);
    }
    delwin(menu->win);
}

int scroll_menu(vector<MenuElement*> &menu,int codeMenu)
{
    int key;
    int selected=0;
    while (1) {
        key=getch();
        if (key==KEY_DOWN || key==KEY_UP) {
            wbkgd(menu[codeMenu]->menuItems[selected].win,COLOR_PAIR(4));
            wnoutrefresh(menu[codeMenu]->menuItems[selected].win);
            if (key==KEY_DOWN) {
                if(selected+1 < (int)menu[codeMenu]->menuItems.size()){
                    selected=(selected+1);
                }
                else{
                    selected=0;
                }
            }
            else {
                if(selected-1 <0){
                    selected=menu[codeMenu]->menuItems.size()-1;
                }
                else{
                    selected=selected-1;
                }
            }
            wbkgd(menu[codeMenu]->menuItems[selected].win,COLOR_PAIR(5));
            wnoutrefresh(menu[codeMenu]->menuItems[selected].win);
            doupdate();
        }
        else if (key==KEY_LEFT || key==KEY_RIGHT) {
            if (key==KEY_RIGHT) {
                return -2;
            }
            else {
                return -3;
            }
        }
        else if (key==ESCAPE) {
            return -1;
        }
        else if (key==ENTER) {
            return selected;
        }
    }
}

void init_menu(vector<MenuElement*> &menu){
    MenuElement *sessionMenu = (MenuElement*)malloc((int)sizeof(MenuElement));
    sessionMenu->name = "Session";
    sessionMenu->start_col=1;
    sessionMenu->width=18;
    sessionMenu->menuItems.push_back({"Ouvrir",NULL});
    sessionMenu->menuItems.push_back({"Fermer",NULL});
    sessionMenu->menuItems.push_back({"Redemarrer Fbx",NULL});

    MenuElement *redirMenu = (MenuElement*)malloc((int)sizeof(MenuElement));
    redirMenu->name = "Redirection";
    redirMenu->start_col=(int)sessionMenu->name.size() +6;
    redirMenu->width=18;
    redirMenu->menuItems.push_back({"Lister",NULL});
    redirMenu->menuItems.push_back({"Ajouter",NULL});
    redirMenu->menuItems.push_back({"Supprimer",NULL});
    redirMenu->menuItems.push_back({"Voir port X",NULL});
    redirMenu->menuItems.push_back({"Update port X",NULL});
    redirMenu->menuItems.push_back({"Sauvegarder",NULL});
    redirMenu->menuItems.push_back({"Charger",NULL});
    menu.push_back(sessionMenu);
    menu.push_back(redirMenu);
}

void init_interface(WINDOW *menubar, WINDOW *main_win, WINDOW *messagebar,vector<MenuElement*> &menu){
    int term_h,term_w;
    getmaxyx(stdscr, term_h, term_w);

    bkgd(COLOR_PAIR(2));
    move(2,1);
    wattron(stdscr,COLOR_PAIR(4));

    menubar=subwin(stdscr,3,term_w,0,0);
    main_win=subwin(stdscr,term_h-4,term_w,3,0);
    wattron(menubar,COLOR_PAIR(4));
    box(menubar, ACS_VLINE, ACS_HLINE);
    wattroff(menubar,COLOR_PAIR(4));
    wattron(main_win,COLOR_PAIR(4));
    box(main_win, ACS_VLINE, ACS_HLINE);
    wmove(main_win,5,1);
    wprintw(main_win,"Press F(X) to open the menus. ");
    wprintw(main_win,"ESC quits.");
    wattroff(main_win,COLOR_PAIR(4));
    draw_menubar(menubar,menu);
    messagebar=subwin(stdscr,1,term_w-1,LINES-1,1);
    refresh();
}

*/

