#include<stdio.h>
#include<string.h>
#include<ncurses.h>
#include<stdlib.h>
#include<ctype.h>
#include<time.h>
#include<locale.h>
#include<wchar.h>
#define FILENAME "users.txt"
#define height_map 50
#define width_map 100
#define min_andazeh_otagh 4
#define max_andazeh_otagh 10
#define max_otagh 20
#define min_otagh 6
#define mahdoodeh_did 5
#define player_character "\u2687"
#define BLACK_GOLD_VALUE 5 


#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <locale.h>
#include <wchar.h>



typedef struct{
    char username[100];
    char password[100];
    char email[100];
} User;

typedef struct{
    int score,gold,tedad_bazi;
    time_t first_play;
} amar_player;

typedef struct{
    char savename[100],username[100];
    time_t save_time;
} savegame;

typedef struct{
    int sath;
    int rang_shakhsiat;
}gamesetting;

typedef enum{
    otagh_sadeh,
    otagh_ganj,
    otagh_telesm
} type_otagh;

typedef enum{
    khali_otagh = ' ',
    divar1_otagh = '|',
    divar2_otagh = '_',
    kaf_otagh ='.',
    dar_otagh ='+',
    rahro ='#',
    sotoon = 'O',
    panjereh = '=',
    tala = '$',
    talaye_siah = 'G' ,
    peleh = '>',
    taleh = '^',
    dar_makhfi = '?',
    dar_ramzdar = '@',
    dokmeh_ramz = '&',
    ghaza = '%',
    aslahe = '/',
    telesm = '*'
} dakhel_otagh; 

typedef struct{
    int x , y , height , width , kashf_shodeh,rah_peleh,password_dar,password,ramz_makoos,password2,do_ramzeh;
    time_t zaman_akharin_ramz;
    type_otagh type;
} otagh;


typedef struct{
    char esm[50];
    char namad;
    int khatar;
    int tedad;
} Selah;

typedef struct{
    char esm[50];
    char namad;
    int noe;
    int tedad;
} Telesm;


typedef struct{
    dakhel_otagh cell[height_map][width_map];
    dakhel_otagh did[height_map][width_map];
    otagh room[max_otagh];
    Selah selah[5];
    Telesm telesm[3];
    int shomareh_room , makan_x , makan_y , tabagheh_alan , map_kol , gold , tedad_ghaza ,noe_selah , health , gorosnegi;
    char payam[256];
    char username[100];
} Map;




void save_amar_player(const char *username , amar_player  amar){
    FILE *file = fopen("amar.txt" , "a");
    if(file){
        fprintf(file,"%s %d %d %ld %d\n" , username , amar.score,amar.gold,amar.first_play,amar.tedad_bazi);
        fclose(file);
    }
}


typedef enum {
    up,
    down,
    left,
    right,
    up_right,
    up_left,
    down_right,
    down_left
} jahat;

jahat tabdil_jahat_be_kelid(int kelid);
int harkat_kardan(Map *map, jahat direct, WINDOW *window, const char *username);
void harkat_sari(Map *map, jahat direct, WINDOW *window, const char *username);


amar_player get_amar_player(const char *username){
    amar_player amar = {0};
    FILE *file=fopen("amar.txt","r");
    if(file){
        char khat[256];
        while(fgets(khat , sizeof(khat) , file)){
            char username1[100];
            if( sscanf(khat,"%s %d %d %ld %d",username1,&amar.score , &amar.gold , &amar.first_play , &amar.tedad_bazi) == 5){
                if (strcmp(username1 , username) == 0){
                    break;
                }
            }
        }
        fclose(file);
    }
    return amar;
}

void save_game( const char *username , const char *savename){
savegame save = {0};
strcpy(save.username , username);
strcpy(save.savename , savename);
save.save_time = time(NULL);
FILE *file = fopen("saves.txt" , "a");
if (file){
    fprintf(file,"%s %s %ld\n" , save.username,save.savename,save.save_time);
    fclose(file);
}
}

void save_user(User *user){
    FILE *file = fopen(FILENAME , "a");
    if (file){
        fprintf(file , "%s %s %s\n" , user ->username , user ->password , user ->email);
        fclose(file);
    }
}

int tekrar_uesrname( const char *username){
    FILE *file = fopen(FILENAME , "r");
    if ( file == NULL){
        return 1;
    }
    char khat[200];
    while ( fgets(khat , sizeof(khat) , file)){
        char save_username[100];
        sscanf(khat,"%s",save_username);
        if(strcmp(save_username , username) == 0){
            fclose(file);
            return 0;
        }
    }
    fclose(file);
    return 1;
}

int password_sahih(const char *password){
    if (strlen(password) < 7){
        return 0;
    }
    int adad = 0;
    int harf_bozorg = 0 ;
    int harf_koochak =0 ;
    for ( int i=0; password[i] != '\0' ; i++){
        if ( password[i] >= '0' && password[i] <= '9'){ adad = 1 ;}
        if ( password[i] >= 'A' && password[i] <= 'Z'){ harf_bozorg = 1 ;}
        if ( password[i] >= 'a' && password[i] <= 'z'){ harf_koochak = 1 ;}
    }
    return (adad && harf_bozorg && harf_koochak);
}

int email_sahih( const char *email){
    int tedad_atsign=0;
    int tedad_dot =0 ;
    int i=0;
    while ( email[i] != '\0'){
        if ( email[i] == '@'){
        tedad_atsign++;
        }
        if(email[i] == '.'){
            tedad_dot++;
        }
        i++;
    }
    if( tedad_atsign !=1 || tedad_dot<1 ){
        return 0;
    }
    char *makan_atsign = strchr(email , '@');
    char *makan_dot = strchr(email , '.');
    if (makan_dot == NULL || makan_atsign == NULL) {
        return 0;
        }  
    if (makan_atsign == email) {
        return 0;
        }                  
    if (*(makan_atsign + 1) == '.') {
        return 0;
    }             
    if (*(makan_atsign + 1) == '\0') {
        return 0; 
    }           
    if (*(makan_dot + 1) == '\0') {
        return 0; 
    }          
    return 1;                                        
}

void error(WINDOW *window , const char *payam){
    mvwprintw(window , 10 , 0 , "error : %s" ,payam);
    wrefresh(window);
    getch() ;
    wclear(window);
}

void neshan_dadan_payam(Map *map, WINDOW *window) {
    mvwprintw(window, height_map - 1, 1, "%s", map->payam);
    wrefresh(window);
    napms(1000);  
    map->payam[0] = '\0';
}

void sabt_user(WINDOW *window){
wclear(window);
box(window,0,0);
wrefresh(window);
User new_user;
mvwprintw(window,1,1,"enter username : ");
wrefresh(window);
echo();
wgetstr(window,new_user.username);
noecho();
if(!tekrar_uesrname(new_user.username)){
    error(window , "username exists");
    wclear(window);
    return ;
}
mvwprintw(window,2,1,"enter password :");
wrefresh(window);
echo();
wgetstr(window,new_user.password);
noecho();
if (!password_sahih(new_user.password)){
error(window,"password is invalid(Password must be at least 7 characters and contain 1 lowercaes, 1 uppercase, and 1 digit");
wclear(window);
return;
}
mvwprintw(window,3,1,"enter email : ");
wrefresh(window);
echo();
wgetstr(window,new_user.email);
noecho();
if(!email_sahih(new_user.email)){
    error(window,"invalid email");
    wclear(window);
    return;
}
save_user(&new_user);
mvwprintw(window,5,1,"user created successfully");
wrefresh(window);
getch();
wclear(window);
}

int valid_login(const char *username , const char *password){
     FILE *file = fopen(FILENAME , "r");
    if ( file == NULL){
        return -1;
    }
    char khat[200];
    while ( fgets(khat , sizeof(khat) , file)){
        char save_username[100] , save_password[100];
        sscanf(khat,"%s %s",save_username , save_password);
        if(strcmp(save_username , username) == 0){
            if(strcmp(save_password,password) == 0){
            fclose(file);
            return 1;}
            else{
                fclose(file);
                return 0;
            }
        }
    }
    fclose(file);
    return -2;
}

void selah_init(Map *map) {
    strcpy(map->selah[0].esm, "Mace");
    map->selah[0].namad = 'M';
    map->selah[0].khatar = 10;
    map->selah[0].tedad = 1;
    
    strcpy(map->selah[1].esm, "Dagger");
    map->selah[1].namad = 'D';
    map->selah[1].khatar = 7;
    map->selah[1].tedad = 0;
    
    strcpy(map->selah[2].esm, "Magic Wand");
    map->selah[2].namad = 'W';
    map->selah[2].khatar = 15;
    map->selah[2].tedad = 0;
    
    strcpy(map->selah[3].esm, "Normal Arrow");
    map->selah[3].namad = 'A';
    map->selah[3].khatar = 5;
    map->selah[3].tedad = 0;
    
    strcpy(map->selah[4].esm, "Sword");
    map->selah[4].namad = 'S';
    map->selah[4].khatar = 12;
    map->selah[4].tedad = 0;
    
    map->noe_selah = 0;
}

void telesm_init(Map *map) {
    strcpy(map->telesm[0].esm, "Health");
    map->telesm[0].namad = 'H';
    map->telesm[0].noe = 0;
    map->telesm[0].tedad = 0;

    strcpy(map->telesm[1].esm, "Speed");
    map->telesm[1].namad = 'S';
    map->telesm[1].noe = 1;
    map->telesm[1].tedad = 0;

    strcpy(map->telesm[2].esm, "Damage");
    map->telesm[2].namad = 'D';
    map->telesm[2].noe = 2;
    map->telesm[2].tedad = 0;
}

void map1(Map *map) {
    memset(map->cell, khali_otagh, sizeof(map->cell));
    memset(map->did, khali_otagh, sizeof(map->did));
    map->shomareh_room = 0;
    map->tabagheh_alan = 1;
    map->map_kol = 0;
    map->health = 100;
    map->gorosnegi = 0;
    map->tedad_ghaza = 0;
    map->gold = 0;
    selah_init(map);
    telesm_init(map);
    strcpy(map->payam, "welcome to the map");
}
 int hampooshani(Map *map , otagh new_room){
    for( int y  = new_room.y - 2 ; y < new_room.y + new_room.height+2 ; y++){
    for( int x  = new_room.x - 2 ; x < new_room.x + new_room.width+2 ; x++){
        if( y < 0 || y>= height_map || x<0 || x >= width_map){
            return 1;
        }
        if ( map ->cell[y][x] != khali_otagh){
            return 1;
        }
    }    
    }
    return 0;
 }

 void add_option(Map *map , otagh room){
    for(int y = room.y +1 ; y < room.y + room.height - 1 ; y++){
    for(int x = room.x +1 ; x < room.x + room.width - 1 ; x++){
        if((rand() % 100)<10 && (map->cell[y][x] == kaf_otagh)){
            map -> cell[y][x] = sotoon;
        }
    }    
    }
    for(int y = room.y ; y < room.y + room.height  ; y++){
    for(int x = room.x ; x < room.x + room.width  ; x++){
        if (( y == room.y || y == room.y + room.height - 1 || x == room.x || x == room.x + room.width - 1) && (rand()%100)<15 && map->cell[y][x] != dar_otagh ){
            map->cell[y][x] = panjereh;
        }         
    }
 }
 }

void ezafe_kardan_talaye_siah(Map *map, otagh *room) {
    if ((rand() % 100) < 10) {
        int x = room->x + 1 + (rand() % (room->width - 2));
        int y = room->y + 1 + (rand() % (room->height - 2));
        if (map->cell[y][x] == kaf_otagh) {
            map->cell[y][x] = talaye_siah;
        }
    }
}


 void ezafe_kardan_ghaza(Map *map , otagh *room){
    if(map ->tedad_ghaza >= 5 ){
        return ; 
    }
    for(int i=0 ; i<2 ; i++){
        int x = room -> x + 1 + (rand()%(room ->width - 2));
        int y = room -> y + 1 + (rand()%(room ->height - 2));
        if(map ->cell[y][x] == kaf_otagh){
            map ->cell[y][x] = ghaza;
            map->tedad_ghaza++ ;
            }
        }
    }
 

void masraf_ghaza(Map *map){
    if ( map ->tedad_ghaza > 0 ){
        map->gorosnegi = (map ->gorosnegi > 20 ) ? map->gorosnegi - 20 : 0;
        map ->tedad_ghaza--;
         if(map ->health <100){
                map->health +=10;
                if(map->health > 100){
                    map->health =100;
                }
         }
    }
}
void neshan_dadan_navar_gorosnegi(Map *map, WINDOW *window) {
    int navar_tool = 20;
    int por_shodeh = (navar_tool * (100 - map->gorosnegi)) / 100;
    mvwprintw(window, height_map - 2, 1, "Hunger: [");
    for(int i = 0; i < navar_tool; i++) {
        if(i < por_shodeh)
            waddch(window, '#');
        else
            waddch(window, ' ');
    }
    wprintw(window, "] %d%%", map->gorosnegi);
    wrefresh(window);
}

void namayesh_menu_ghaza(Map *map, WINDOW *window) {
    wclear(window);
    box(window, 0, 0);
    mvwprintw(window, 1, 1, "Food Menu - Press number to eat");
    mvwprintw(window, 2, 1, "Current Hunger: %d%%", map->gorosnegi);
    mvwprintw(window, 3, 1, "You have %d food items", map->tedad_ghaza);
    
    if(map->tedad_ghaza > 0) {
        mvwprintw(window, 5, 1, "Press 1 to eat food");
    } else {
        mvwprintw(window, 5, 1, "No food available");
    }
    
    wrefresh(window);
    
    int ch = wgetch(window);
    if(ch == '1' && map->tedad_ghaza > 0) {
        masraf_ghaza(map);
        mvwprintw(window, 7, 1, "Food consumed! Press any key to continue...");
        wrefresh(window);
        wgetch(window);
    }
}

void afzayesh_gorosnegi(Map *map) {
    static time_t zaman_ghabl = 0;
    time_t zaman_alan = time(NULL);
    if(zaman_alan - zaman_ghabl >= 30) {
        map->gorosnegi += 5;
        if(map->gorosnegi > 100) {
            map->gorosnegi = 100;
            map->health -= 2; 
        }
        zaman_ghabl = zaman_alan;
    }
}

void taghir_selah(Map *map){
    map ->noe_selah = (map->noe_selah + 1) %5;
    while(map->selah[map->noe_selah].tedad == 0){
        map->noe_selah = (map->noe_selah+1)%5;
    }
}



void ezafe_kardan_peleh(Map *map) {
    if(map->tabagheh_alan < 4) {  
        int peleh_added = 0;
        while(!peleh_added) {
            int room_index = rand() % map->shomareh_room;
            otagh *room = &map->room[room_index];
            int x = room->x + 1 + (rand() % (room->width - 2));
            int y = room->y + 1 + (rand() % (room->height - 2));
            if(map->cell[y][x] == kaf_otagh) {
                map->cell[y][x] = peleh;
                peleh_added = 1;
                room->rah_peleh = 1;
                strcpy(map->payam, "Stairs to next floor created");
            }
        }
    }
}
void show_inventory(Map *map , WINDOW *window){
    wclear(window);
    box(window,0,0);
    mvwprintw(window,1,1,"Weapons:");
    for(int i =0 ; i<5;i++){
        if(map->selah[i].tedad > 0){
            mvwprintw(window,i+2,2,"%s (%c) x%d" , map->selah[i].esm , map->selah[i].namad , map->selah[i].tedad);
            if(i == map->noe_selah){
                wprintw(window,"EQUIPPED");
            }
        }
    }
    mvwprintw(window,8,1,"Spells:");
    for(int i = 0 ; i < 3 ; i++){
        if(map -> telesm[i].tedad > 0){
           mvwprintw(window,i+9,2,"%s (%c) x%d" ,map->telesm[i].esm , map->telesm[i].namad , map->telesm[i].tedad );
         }
    }
    wrefresh(window);
    wgetch(window);
    wclear(window);
}

void neshan_dadan_vaziat(Map *map , WINDOW *window){
    mvwprintw(window,0,1,"Health: %d%% | Hunger: %d%% | Floor: %d" , map->health , map ->gorosnegi , map ->tabagheh_alan);
}

void collect_item(Map *map, int x, int y) {
    amar_player amar = get_amar_player(map->username);
    
    switch (map->cell[y][x]) {
        case tala:
            map->gold++;
            amar.gold++;
            strcpy(map->payam, "Found gold!");
            break;
            
        case talaye_siah:
            map->gold += BLACK_GOLD_VALUE;
            amar.gold += BLACK_GOLD_VALUE;
            strcpy(map->payam, "Found black gold!");
            break;
            
        case aslahe:
            for(int i = 0; i < 5; i++) {
                if (map->selah[i].tedad == 0) {
                    map->selah[i].tedad++;
                    strcpy(map->payam, "Found new weapon!");
                    break;
                }
            }
            break;
            
        case telesm:
            for(int i = 0; i < 3; i++) {
                if (map->telesm[i].tedad == 0) {
                    map->telesm[i].tedad++;
                    strcpy(map->payam, "Found new spell!");
                    break;
                }
            }
            break;
            
        case ghaza:
            if (map->tedad_ghaza < 5) {
                map->tedad_ghaza++;
                strcpy(map->payam, "Found food!");
            }
            break;
    }

    map->cell[y][x] = kaf_otagh;
    save_amar_player(map->username, amar);
}

void display_stats(Map *map, WINDOW *window) {
    mvwprintw(window, 0, 1, "Health: %d%% | Food: %d/5 | Gold: %d | Floor: %d", 
    map->health, map->tedad_ghaza, map->gold, map->tabagheh_alan);
    if (strlen(map->payam) > 0) {
        mvwprintw(window, height_map - 1, 1, "%s", map->payam);
        wrefresh(window);
        napms(1000); 
        map->payam[0] = '\0';
    }
}

int check_game_over(Map *map, WINDOW *window, const char *username) {
    if (map->tabagheh_alan == 4) {
        for (int i = 0; i < map->shomareh_room; i++) {
            otagh room = map->room[i];
            if (room.type == otagh_ganj && 
                map->makan_x >= room.x && map->makan_x < room.x + room.width &&
                map->makan_y >= room.y && map->makan_y < room.y + room.height) {
                amar_player amar = get_amar_player(username);
                amar.score += map->gold; 
                save_amar_player(username, amar);
                wclear(window);
                box(window, 0, 0);
                mvwprintw(window, 2, 2, "Congratulations! You've completed the game!");
                mvwprintw(window, 3, 2, "Final Score: %d", amar.score);
                mvwprintw(window, 4, 2, "Total Gold: %d", amar.gold);
                mvwprintw(window, 6, 2, "Press any key to continue...");
                wrefresh(window);
                wgetch(window);
                return 1;
            }
        }
    }
    return 0;
}

 void sakht_otagh(Map *map , otagh room){
    for(int y = room.y ; y < room.y + room.height ; y++){
        for(int x = room.x ; x < room.x + room.width ; x++){
            if ( y == room.y  || y == room.y + room.height - 1){
                map ->cell[y][x] = divar2_otagh;
            }
            else if(x == room.x || x == room.x+room.width-1){
                map ->cell[y][x] = divar1_otagh;
            }
            else{
                map->cell[y][x] = kaf_otagh;
            }
        }
    }
    add_option(map,room);
    map->room[map->shomareh_room++] = room;
 }

 void sakht_rahro(Map *map, int x1, int x2, int y1, int y2) {
    int current_x = x1;
    int current_y = y1;
    while(current_x != x2 || current_y != y2) {
        if(map->cell[current_y][current_x] != divar1_otagh &&  map->cell[current_y][current_x] != divar2_otagh) {
            map->cell[current_y][current_x] = rahro;
        } 
        else {
            map->cell[current_y][current_x] = dar_otagh;
        }
        if(rand() % 4 == 0 && current_x != x2 && current_y != y2) {
            if(rand() % 2 == 0) {
                current_x += (rand() % 2) * 2 - 1; 
            } 
            else {
                current_y += (rand() % 2) * 2 - 1;  
            }
            current_x = (current_x < 0) ? 0 : ((current_x >= width_map) ? width_map - 1 : current_x);
            current_y = (current_y < 0) ? 0 : ((current_y >= height_map) ? height_map - 1 : current_y);
        }
        else {
            if(current_x != x2) {
                current_x += (x2 > current_x) ? 1 : -1;
            }
            if(current_y != y2) {
                current_y += (y2 > current_y) ? 1 : -1;
            }
        }
    }
}
 void up_did(Map *map) {

    for(int y = 0; y < height_map; y++) {
        for(int x = 0; x < width_map; x++) {
            map->did[y][x] = khali_otagh;
        }
    }

    for(int i = 0; i < map->shomareh_room; i++) {
        otagh *room = &map->room[i];
        
        if((map->makan_x >= room->x && map->makan_x < room->x + room->width &&
            map->makan_y >= room->y && map->makan_y < room->y + room->height) || 
           room->kashf_shodeh) {
            for(int y = room->y; y < room->y + room->height; y++) {
                for(int x = room->x; x < room->x + room->width; x++) {
                    map->did[y][x] = map->cell[y][x];
                }
            }
            if(map->makan_x >= room->x && map->makan_x < room->x + room->width &&
               map->makan_y >= room->y && map->makan_y < room->y + room->height) {
                room->kashf_shodeh = 1;
            }
        }
    }
    if(map->cell[map->makan_y][map->makan_x] == rahro) {
        for(int dy = -mahdoodeh_did; dy <= mahdoodeh_did; dy++) {
            for(int dx = -mahdoodeh_did; dx <= mahdoodeh_did; dx++) {
                int x = map->makan_x + dx;
                int y = map->makan_y + dy;
                
                if(x >= 0 && x < width_map && y >= 0 && y < height_map) {
                    if(dx*dx + dy*dy <= mahdoodeh_did*mahdoodeh_did) {
                        map->did[y][x] = map->cell[y][x];
                    }
                }
            }
        }
    }
}

otagh sakht_otagh_jadid(){
    otagh new_room;
    new_room.width = min_andazeh_otagh + (rand() % (max_andazeh_otagh-min_andazeh_otagh+1));
    new_room.height = min_andazeh_otagh + (rand() % (max_andazeh_otagh-min_andazeh_otagh+1));
    new_room.x = 1 + (rand() % (width_map - new_room.width -2));
    new_room.y = 1 + (rand() % (height_map - new_room.height -2));
    new_room.kashf_shodeh=0;
    new_room.ramz_makoos = rand() % 2;
    new_room.do_ramzeh=0;
    new_room.password2=0;
    return new_room;
}

void sakht_otagh_ganj(Map *map, otagh *room) {
    if (map->tabagheh_alan != 4) {
        return;
    }
    room->type = otagh_ganj;
    for (int y = room->y + 1; y < room->y + room->height - 1; y++) {
        for (int x = room->x + 1; x < room->x + room->width - 1; x++) {
            if ((rand() % 100) < 30 && map->cell[y][x] == kaf_otagh) {
                map->cell[y][x] = taleh;
            }
        }
    }
    for (int i = 0; i < 3; i++) {
        int x = room->x + 1 + (rand() % (room->width - 2));
        int y = room->y + 1 + (rand() % (room->height - 2));
        if (map->cell[y][x] == kaf_otagh) {
            map->cell[y][x] = tala;
        }
    }
    if (map->makan_x >= room->x && map->makan_x < room->x + room->width &&
        map->makan_y >= room->y && map->makan_y < room->y + room->height) {
        
        amar_player amar = get_amar_player(map->username);
        amar.score += map->gold; 
        save_amar_player(map->username, amar);
        
        strcpy(map->payam, "You reached the treasure room! Game Over!");
    }
}

void sakht_otagh_telesm(Map *map, otagh *room) {
    room->type = otagh_telesm;
    if (!room->rah_peleh && !room->kashf_shodeh) {
        return;
    }
    for (int i = 0; i < 2; i++) {
        int x = room->x + 1 + (rand() % (room->width - 2));
        int y = room->y + 1 + (rand() % (room->height - 2));
        if (map->cell[y][x] == kaf_otagh) {
            map->cell[y][x] = telesm;
        }
    }
    if (map->makan_x >= room->x && map->makan_x < room->x + room->width &&
        map->makan_y >= room->y && map->makan_y < room->y + room->height) {
        map->health -= 5;
        strcpy(map->payam, "The enchanted room is draining your health!");
    }
}

void vasl_otagh(Map *map , otagh room1,otagh room2){
    int x1 = room1.x + room1.width/2;
    int y1 = room1.y + room1.height/2;
    int x2 = room2.x + room2.width/2;
    int y2 = room2.y + room2.height/2;
    int x =x1;
    while( x != x2){
        if(map->cell[y1][x] != divar1_otagh && map->cell[y1][x] != divar2_otagh){
            map->cell[y1][x] = rahro;
        }
        x += ((x2 >x1) ? 1 : -1);
    }
    int y =y1;
    while( y != y2){
        if(map->cell[y][x2] != divar1_otagh && map->cell[y][x2] != divar2_otagh){
            map->cell[y][x2] = rahro;
        }
        y += ((y2 > y1) ? 1 : -1);
    }
}



void map_kamel(Map *map) {
    map1(map);
    srand(time(NULL));
    int attempts = 0;
    const int MAX_ATTEMPTS = 1000;

    while(map->shomareh_room < min_otagh && attempts < MAX_ATTEMPTS) {
        otagh new_room = sakht_otagh_jadid();
        
        if(!hampooshani(map, new_room)) {
            sakht_otagh(map, new_room);
            if(map->tabagheh_alan == 4 && map->shomareh_room == min_otagh - 1) {
                sakht_otagh_ganj(map, &map->room[map->shomareh_room - 1]);
            }
            else if((rand() % 100) < 30) {
                sakht_otagh_telesm(map, &map->room[map->shomareh_room - 1]);
            }
            if(map->shomareh_room > 1) {
                vasl_otagh(map, map->room[map->shomareh_room - 2], new_room);
            }
            ezafe_kardan_ghaza(map, &map->room[map->shomareh_room - 1]);
            ezafe_kardan_talaye_siah(map, &map->room[map->shomareh_room - 1]);
        }
        attempts++;
    }
    if(map->shomareh_room > 0) {
        otagh first_room = map->room[0];
        map->makan_x = first_room.x + first_room.width / 2;
        map->makan_y = first_room.y + first_room.height / 2;
        ezafe_kardan_peleh(map);
        up_did(map);
    }
}

void ezafe_kardan_dar_ramzdar(Map *map , otagh *room){
    int samt = rand() % 4;
    int halat;
    switch(samt){
        case 0:
        halat = room->x + 1 + (rand() % (room->width -2));
        map ->cell[room ->y][halat] = dar_ramzdar;
        break;
        case 1:
        halat = room->y + 1 + (rand() % (room->height -2));
        map ->cell[halat][room ->x + room->width - 1] = dar_ramzdar;
        break;
        case 2:
        halat = room->x + 1 + (rand() % (room->width -2));
        map ->cell[room ->y + room->height -1][halat] = dar_ramzdar;
        break;
        case 3:
        halat = room->x + 1 + (rand() % (room->width -2));
        map ->cell[halat][room ->x] = dar_ramzdar;
        break;
    }
    room->do_ramzeh = (map->tabagheh_alan >= 3) && (rand() % 2);
    room->password_dar = 1;
    room->password = 1000 + (rand() % 9000);
    room->zaman_akharin_ramz = time(NULL);
    if(room->do_ramzeh) {
        room->password2 = 1000 + (rand() % 9000);
        int dokmeh_x = room->x + 1 + (rand() % (room->width - 2));
        int dokmeh_y = room->y + 1 + (rand() % (room->height - 2));
        map->cell[dokmeh_y][dokmeh_x] = dokmeh_ramz;
        int dokmeh2_x, dokmeh2_y;
        do {
            dokmeh2_x = room->x + 1 + (rand() % (room->width - 2));
            dokmeh2_y = room->y + 1 + (rand() % (room->height - 2));
        } while(dokmeh2_x == dokmeh_x && dokmeh2_y == dokmeh_y);
        map->cell[dokmeh2_y][dokmeh2_x] = dokmeh_ramz;
    } else {
        int dokmeh_x = room->x + 1 + (rand() % (room->width - 2));
        int dokmeh_y = room->y + 1 + (rand() % (room->height - 2));
        map->cell[dokmeh_y][dokmeh_x] = dokmeh_ramz;
    }
}


void chap_map(Map *map, WINDOW *window) {
    gamesetting setting_user = {2, COLOR_WHITE};
    FILE *file = fopen("setting.txt", "r");
    if (file) {
        fscanf(file, "%d %d", &setting_user.sath, &setting_user.rang_shakhsiat);
        fclose(file);
    }

    for (int y = 0; y < height_map; y++) {
        for (int x = 0; x < width_map; x++) {
            if (map->did[y][x] != khali_otagh) {
                int color_pair = 1; 
                for (int i = 0; i < map->shomareh_room; i++) {
                    if (x >= map->room[i].x && x < map->room[i].x + map->room[i].width &&
                        y >= map->room[i].y && y < map->room[i].y + map->room[i].height) {
                        
                        switch (map->room[i].type) {
                            case otagh_ganj:
                                color_pair = 2;
                                break;
                            case otagh_telesm:
                                color_pair = 3; 
                                break;
                            default:
                                color_pair = 1; 
                        }
                        break;
                    }
                }
                
                wattron(window, COLOR_PAIR(color_pair));
                mvwaddch(window, y, x, map->did[y][x]);
                wattroff(window, COLOR_PAIR(color_pair));
            } else {
                mvwaddch(window, y, x, ' ');
            }
        }
    }
    wattron(window, COLOR_PAIR(setting_user.rang_shakhsiat + 1));
    mvwprintw(window, map->makan_y, map->makan_x, player_character);
    wattroff(window, COLOR_PAIR(setting_user.rang_shakhsiat + 1));
    
    wrefresh(window);
}

void atraf(Map *map,WINDOW *window){
    int taghir = 0;
    for( int did_y=-1 ; did_y<=1 ; did_y ++){
        for( int did_x=-1 ; did_x<=1 ; did_x ++){
            int x = map ->makan_x + did_x;
            int y = map ->makan_y + did_y;
            if( x >= 0 && x < width_map && y>= 0 && y < height_map){
                if( map ->cell[y][x] == taleh){
                    map->did[y][x] = taleh;
                    taghir = 1;
                }
                if( map ->cell[y][x] == dar_makhfi){
                    map->did[y][x] = dar_makhfi;
                    taghir = 1;
                }
            }
        }
    }
    if(taghir){
        chap_map(map , window);
        mvwprintw(window,height_map - 1 , 1 , "found hidden elements");
        wrefresh(window);
        napms(1000);
    }
}

void save_map(Map *map , const char *username , const char *savename){
    char filename[256];
    sprintf(filename,"saves/%s_%s_map.txt",username,savename);
    FILE *file = fopen(filename,"w");
    if(file){
        fprintf(file,"%d %d\n" , map ->makan_x , map -> makan_y);
        fprintf(file,"%d\n" , map ->shomareh_room);
        for(int i = 0 ; i < map ->shomareh_room ; i++){
            fprintf(file,"%d %d %d %d %d\n", map->room[i].x , map -> room[i].y , map ->room[i].width , map -> room[i].height , map ->room[i].kashf_shodeh);
        }
        for(int y=0 ; y < height_map ; y++){
            for(int x=0 ; x < height_map ; x++){
                fprintf(file,"%c",map ->cell[y][x]);
            }
            fprintf(file,"\n");
        }
        for(int y=0 ; y < height_map ; y++){
            for(int x=0 ; x < height_map ; x++){
                fprintf(file,"%c",map ->did[y][x]);
            }
            fprintf(file,"\n");
        }
        fclose(file);
    }
}

int load_map(Map *map, const char *username , const char *savename){
    char filename[256];
    sprintf(filename,"saves/%s_%s_map.txt",username,savename);
    FILE *file = fopen(filename , "r");
    if(file){
        fscanf(file,"%d %d\n" , &map->makan_x , &map->makan_y);
        fscanf(file,"%d\n" , &map ->shomareh_room);
        for(int i = 0 ; i < map ->shomareh_room ; i++){
            fscanf(file,"%d %d %d %d %d\n", &map->room[i].x , &map -> room[i].y , &map ->room[i].width , &map -> room[i].height , &map ->room[i].kashf_shodeh);
        }
        char line[width_map + 2];
        for(int y = 0 ; y < height_map ; y++){
            fgets(line,sizeof(line) , file);
            for(int x =0 ; x < width_map ; x++){
                map ->cell[y][x] = line[x];
            }
        }
        for(int y = 0 ; y < height_map ; y++){
            fgets(line,sizeof(line),file);
            for( int x =0 ; x < width_map ; x++){
                map ->did[y][x] = line[x];
            }
        }
        fclose(file);
        return 1;
    }
    return 0;
}



void create_new_game(WINDOW *window, const char *username) {
    amar_player amar = {0};
    amar.first_play = time(NULL);
    amar.tedad_bazi = 1;
    save_amar_player(username, amar);

    Map game_map;
    strcpy(game_map.username, username);
    map_kamel(&game_map);
    
    while(1) {
        display_stats(&game_map, window);
        
        chap_map(&game_map, window);
        
        if (check_game_over(&game_map, window, username)) {
            break;
        }

        int c = wgetch(window);
        
        if(c == 27) { 
            char savename[100];
            wclear(window);
            box(window, 0, 0);
            mvwprintw(window, 1, 1, "Enter save name: ");
            echo();
            wgetstr(window, savename);
            noecho();
            save_game(username, savename);
            save_map(&game_map, username, savename);
            break;
        }
        if(c == 'E' || c == 'e') {
            namayesh_menu_ghaza(&game_map, window);
            continue;
        }
        jahat direct = tabdil_jahat_be_kelid(c);
        if(direct != -1) {
            harkat_kardan(&game_map, direct, window, username);
            continue;
        }
        if(c == 'f') {
            c = wgetch(window);
            direct = tabdil_jahat_be_kelid(c);
            if(direct != -1) {
                harkat_sari(&game_map, direct, window, username);
            }
        }
        if(c == 's') {
            atraf(&game_map, window);
        }
    }
}

void continue_game(WINDOW *window , const char *username){
    wclear(window);
    box(window,0,0);
    savegame saves[20];
    int shomareh_save =0;
    int entekhab =0;
    FILE *file = fopen("saves.txt","r");
    if(!file){
        mvwprintw(window,1,1,"no save game found");
        wrefresh(window);
        getch();
        return;
    }
    char khat[256];
    while(fgets(khat,sizeof(khat),file) && shomareh_save<20){
        if (sscanf(khat,"%s %s %ld" , saves[shomareh_save].username , saves[shomareh_save].savename ,&saves[shomareh_save].save_time) == 3){
           if(strcmp(saves[shomareh_save].username , username) == 0){
            shomareh_save++ ;
           }
        }
    }
    fclose(file);
    if(shomareh_save == 0 ){
        mvwprintw(window,1,1," no save found");
        wrefresh(window);
        getch();
        return;
    }
    while(1){
        wclear(window);
        box(window,0,0);
        mvwprintw(window,1,1,"select save : ");
        for(int i=0 ; i<shomareh_save ; i++){
            if( i == entekhab){
                wattron(window,A_REVERSE);
            }
            mvwprintw(window,i+3,1,"%s - %s" , saves[i].savename , ctime(&saves[i].save_time));
            if( i == entekhab){
                wattroff(window,A_REVERSE);
            }
        }
        wrefresh(window);
        int c = getch();
        switch(c){
            case KEY_UP: entekhab = (entekhab - 1 + shomareh_save) % shomareh_save;
            break;
            case KEY_DOWN: entekhab = (entekhab + 1) % shomareh_save;
            break;
            case'\n':{
                Map game_map;
                if(load_map(&game_map,username,saves[entekhab].savename)){
                    while(1){
                        chap_map(&game_map,window);
                        int c = wgetch(window);
                        if(c == 27){
                            char savename[100];
                            wclear(window);
                            box(window,0,0);
                            mvwprintw(window,1,1,"enter save name :");
                            echo();
                            wgetstr(window,savename);
                            noecho();
                            save_game(username,savename);
                            save_map(&game_map,username,savename);
                            break;
                        }
                    }
                }
            return;
            }
            case 27:
            return;
        }
    }
}

int harkat_momken(Map *map , int new_x , int new_y){
    if(new_x < 0 || new_x> width_map || new_y < 0 || new_y>= height_map){
        return 0 ;
    }
    dakhel_otagh cell = map ->cell[new_y][new_x];
    return(cell != divar1_otagh && cell != divar2_otagh && cell != sotoon);
}

void taviz_tabagheh(Map *map , WINDOW *window , int direction){
    if( (direction == 1) && (map -> tabagheh_alan<4)){
        map -> tabagheh_alan++;
        map1(map);
        map_kamel(map);
    }
    else if( (direction == -1) && (map ->tabagheh_alan > 1)){
        map ->tabagheh_alan --;
    }
}

int harkat_kardan(Map *map, jahat direct, WINDOW *window, const char *username) {
    int new_x = map->makan_x;
    int new_y = map->makan_y;
    
    switch(direct) {
        case up: new_y--; break;
        case down: new_y++; break;
        case left: new_x--; break;
        case right: new_x++; break;
        case up_left: new_y--; new_x--; break;
        case up_right: new_y--; new_x++; break;
        case down_left: new_y++; new_x--; break;
        case down_right: new_y++; new_x++; break;
        default: return 0;
    }

    if(!harkat_momken(map, new_x, new_y)) {
        return 0;
    }

    
    map->makan_x = new_x;
    map->makan_y = new_y;

   
    if(map->cell[new_y][new_x] == tala || map->cell[new_y][new_x] == talaye_siah || 
       map->cell[new_y][new_x] == aslahe || map->cell[new_y][new_x] == telesm || 
       map->cell[new_y][new_x] == ghaza) {
        collect_item(map, new_x, new_y);
    }

    if(map->cell[new_y][new_x] == peleh) {
        taviz_tabagheh(map, window, 1); 
    }
    up_did(map);
    if(map->cell[new_y][new_x] == rahro) {
        for(int i = 0; i < map->shomareh_room; i++) {
            if(new_x >= map->room[i].x && new_x < map->room[i].x + map->room[i].width &&
               new_y >= map->room[i].y && new_y < map->room[i].y + map->room[i].height) {
                map->room[i].kashf_shodeh = 1;
                break;
            }
        }
    }

    return 1;
}
jahat tabdil_jahat_be_kelid(int kelid){
    switch(kelid){
        case '1': return down_left;
        case '2': return down;
        case '3': return down_right;
        case '4': return left;
        case '6': return right;
        case '7': return up_left;
        case '8': return up;
        case '9': return up_right;
        default:return -1;
    }
}


void harkat_sari(Map *map , jahat direct , WINDOW *window , const char *username ){
    int did_x = 0;
    int did_y = 0;
    switch(direct){
        case up : did_y = -1; break;
        case down : did_y = 1; break;
        case left : did_x = -1; break;
        case right : did_x = 1; break;
        case up_left : did_y = -1 ; did_x = -1; break;
        case up_right : did_y = -1; did_x=1 ; break;
        case down_left : did_y = 1;did_x=-1; break;
        case down_right : did_y = 1;did_x=1; break;
    }
    while(harkat_momken(map , map ->makan_x + did_x , map ->makan_y + did_y)){
        map ->makan_x += did_x;
        map -> makan_y += did_y;
        up_did(map);
        chap_map(map,window);
        napms(50);
        dakhel_otagh makan = map->cell[map->makan_y][map ->makan_x];
        if(makan==tala || makan == ghaza || makan ==aslahe || makan == telesm || makan == taleh){
            break;
        } 
    }
}

int reverse_password(int password) {
    int reversed = 0;
    while (password > 0) {
        reversed = reversed * 10 + password % 10;
        password /= 10;
    }
    return reversed;
}

void modiriat_ramz_dar(Map *map, WINDOW *window, int dar_x, int dar_y) {
    char password_voroodi[5];
    int talash = 0;
    int dar_otagh = -1;
    for(int i = 0; i < map->shomareh_room; i++) {
        otagh room = map->room[i];
        if(dar_x >= room.x && dar_x < room.x + room.width && 
           dar_y >= room.y && dar_y < room.y + room.height) {
            dar_otagh = i;
            break;
        }
    }
    
    if(dar_otagh == -1) return;
    otagh *room = &map->room[dar_otagh];
    if(room->do_ramzeh) {
        int password1_correct = 0, password2_correct = 0;
        mvwprintw(window, 2, 2, "Enter first password:");
        wrefresh(window);
        echo();
        wgetnstr(window, password_voroodi, 4);
        noecho();
        
        int pass1 = atoi(password_voroodi);
        int target_pass1 = room->ramz_makoos ? reverse_password(room->password) : room->password;
        if(pass1 == target_pass1) {
            password1_correct = 1;
            mvwprintw(window, 3, 2, "First password correct! Enter second password:");
            wrefresh(window);
            echo();
            wgetnstr(window, password_voroodi, 4);
            noecho();
            
            int pass2 = atoi(password_voroodi);
            if(pass2 == room->password2) {
                password2_correct = 1;
                map->cell[dar_y][dar_x] = dar_otagh;
                strcpy(map->payam, "Door unlocked with both passwords!");
            }
        }
        
        if(!password1_correct || !password2_correct) {
            strcpy(map->payam, "Wrong password combination!");
        }
    } else {
        while(talash < 3) {
            mvwprintw(window, 2, 2, "enter password(%d attempts left):", 3 - talash);
            wrefresh(window);
            echo();
            wgetnstr(window, password_voroodi, 4);
            noecho();
            
            int password1 = atoi(password_voroodi);
            int target_password = room->ramz_makoos ? reverse_password(room->password) : room->password;
            
            if(password1 == target_password) {
                map->cell[dar_y][dar_x] = dar_otagh;
                strcpy(map->payam, "door unlocked");
                break;
            } else {
                talash++;
                if(talash == 1) {
                    wattron(window, COLOR_PAIR(2));
                } else if(talash == 2) {
                    wattron(window, COLOR_PAIR(4));
                }
                sprintf(map->payam, "wrong password(%d attempts left)", 3 - talash);
                wattroff(window, COLOR_PAIR(2));
                wattroff(window, COLOR_PAIR(4));
            }
        }
        
        if(talash == 3) {
            strcpy(map->payam, "door locked(security mode activated)");
        }
    }
}
void sakht_ramz(Map *map,WINDOW *window){
    for(int i = 0 ; i < map->shomareh_room;i++){
        otagh *room = &map ->room[i];
        if(room ->password_dar){
            room ->password = 1000 + (rand() % 9000);
            room ->zaman_akharin_ramz = time(NULL);
            mvwprintw(window,2,2,"new password generated : %04d" , room ->password);
            wrefresh(window);
            napms(3000);
        }
    }
}



void namayesh_map(Map *map){
    if(map ->map_kol){
        for(int y=0 ; y < height_map ; y++){
            for(int x = 0 ; x < width_map ;x++){
                map -> did[y][x] = map->cell[y][x];
            }
        }
    }
    else{
        up_did(map);
    }
}

void scoreboard(WINDOW *window , const char *username_varedshodeh){
    wclear(window);
    box(window,0,0);
    mvwprintw(window,1,1,"scoreboard");
    mvwprintw(window,2,1," rank  username             score gold   games  playtime");
    FILE *file = fopen("amar.txt","r");
    if(file){
        amar_player amar[100];
        char username[100][100];
        int tedad = 0;
        char khat[256];
        while(fgets(khat,sizeof(khat),file) && tedad<100){
            if ( sscanf(khat,"%s %d %d %ld %d" , username[tedad] , &amar[tedad].score,&amar[tedad].gold,&amar[tedad].first_play , &amar[tedad].tedad_bazi) == 5){
                tedad++;
            }
        }
        fclose(file);
        for(int i=0 ; i < tedad - 1 ; i++){
            for(int j=0 ; j< tedad-i-1 ; j++){
                if(amar[j].score < amar[j+1].score){
                    amar_player x = amar[j];
                    amar[j] = amar[j+1];
                    amar[j+1] = x;
                    char x_name[100];
                    strcpy(x_name,username[j]);
                    strcpy(username[j],username[j+1]);
                    strcpy(username[j+1],x_name);
                }
            }
        }
        for(int i = 0 ; i<tedad && i<20 ; i++){
            time_t play_time = time(NULL) - amar[i].first_play;
            if(strcmp(username[i] , username_varedshodeh) == 0){
                wattron(window,COLOR_PAIR(3));
                mvwprintw(window,i+3,1,"%2d     %-10s(legend)%5d  %4d    %4d     %d days", i+1,username[i],amar[i].score , amar[i].gold,amar[i].tedad_bazi, (int)(play_time/86400));
                wattroff(window,COLOR_PAIR(3));
            }
            else if(i<3){
                wattron(window,A_BOLD);
                wattron(window , COLOR_PAIR(2));
                mvwprintw(window,i+3,1,"%2d     %-10s(legend)%5d  %4d    %4d     %d days", i+1,username[i],amar[i].score , amar[i].gold,amar[i].tedad_bazi, (int)(play_time/86400));
                wattroff(window,A_BOLD);
                wattroff(window,COLOR_PAIR(2));
            }
            else{
            mvwprintw(window,i+3,1,"%2d     %-10s(legend)%5d  %4d    %4d     %d days", i+1,username[i],amar[i].score , amar[i].gold,amar[i].tedad_bazi, (int)(play_time/86400));
                }
        }
    }
    wrefresh(window);
    getch();
}

void save_setting(const gamesetting *setting){
    FILE *file = fopen("setting.txt" , "w");
    if(file){
        fprintf(file,"%d %d\n" , setting ->sath , setting ->rang_shakhsiat);
        fclose(file);
    }
}


void setting( WINDOW *window){
    gamesetting setting = {2,COLOR_WHITE};
    FILE *file = fopen("setting.txt","r");
    if(file){
        fscanf(file,"%d %d" , &setting.sath , &setting.rang_shakhsiat);
        fclose(file);
    }
    int entekhab=0;
    const char *sath[] = {"easy","normal","hard"};
    const int colors[] ={COLOR_WHITE,COLOR_RED,COLOR_GREEN,COLOR_BLUE};
    const char *color_names[]={"white","red","green","blue"};
    while(1){
        wclear(window);
        box(window,0,0);
        mvwprintw(window,1,1,"setting");
        mvwprintw(window,3,1,"difficulty: %s",sath[setting.sath - 1]);
        mvwprintw(window,4,1,"character color:%s",color_names[setting.rang_shakhsiat]);
        mvwprintw(window,entekhab+3,40,"<<");
        wrefresh(window);
        int c = wgetch(window);
        switch(c){
            case KEY_UP: entekhab = (entekhab - 1 + 2) % 2;
            break;
            case KEY_DOWN: entekhab=(entekhab+1)%2;
            break;
            case KEY_LEFT:
            case KEY_RIGHT: if(entekhab == 0){
                setting.sath = ((setting.sath - 1 + (c == KEY_RIGHT ? 1 : 2 )) % 3) +1;
            }
            else{
                setting.rang_shakhsiat = (setting.rang_shakhsiat + (c == KEY_RIGHT ? 1 : 3)) % 4;
            }
            break;
            case '\n': save_setting(&setting);
            return;
        }
    }
}

void profile(WINDOW *window , const char *username){
    wclear(window);
    box(window,0,0);
    User user;
    FILE *file = fopen(FILENAME,"r");
    if (file){
        char khat[256];
        while(fgets(khat,sizeof(khat),file)){
            char username1[100] , password[100],email[100];
            sscanf(khat,"%s %s %s",username1,password,email);
            if(strcmp(username1,username) == 0){
                strcpy(user.username , username1);
                strcpy(user.password , password);
                strcpy(user.email , email);
                break;
            }
        }
        fclose(file);
    }
    amar_player amar = get_amar_player(username);
    mvwprintw(window,1,1,"profile");
    mvwprintw(window,2,1,"username : %s" , user.username);
    mvwprintw(window,3,1,"email : %s" , user.email);
    wrefresh(window); 
    getch();
}


void menu2(WINDOW *window , const char *username){
    int entekhab = 0;
    const char *menu[] = {
        "create new game",
        "continue game",
        "scoreboard",
        "setting",
        "profile",
        "logout"
    } ;
    int menu_size = sizeof(menu) / sizeof(menu[0]);
    while(1){
        wclear(window);
        box(window,0,0);
        for(int i=0 ; i<menu_size;i++){
            if( i == entekhab){
                wattron(window,A_REVERSE);
                mvwprintw(window,3+i,2,"%s" , menu[i]);
                wattroff(window,A_REVERSE);
            }
            else{
                mvwprintw(window,3+i,2,"%s",menu[i]);
            }
        }
        wrefresh(window);
        int c=wgetch(window);
        switch(c){
            case KEY_UP : entekhab = (entekhab - 1 + menu_size) % menu_size;
            break;
            case KEY_DOWN: entekhab = (entekhab + 1) % menu_size;
            break;
            case '\n' : {
                if(entekhab == 0){
                    create_new_game(window,username);
                }
                else if(entekhab == 1){
                    continue_game(window , username);
                }
                else if(entekhab == 2){
                    scoreboard(window , username);
                }
                else if(entekhab == 3){
                    setting(window);
                }
                else if(entekhab == 4){
                 profile(window,username);   
                }
                else if( entekhab == 5 ){
                    endwin();
                    return;
                }
                wclear(window);
                break;
            }

        }
    }
}

void login_user(WINDOW *window){
    wclear(window);
    box(window,0,0);
    wrefresh(window);
    char username[100],password[100];
    mvwprintw(window,1,1,"enter username : ");
    wrefresh(window);
    echo();
    wgetstr(window,username);
    noecho();

    mvwprintw(window,2,1,"enter password : ");
    wrefresh(window);
    echo();
    wgetstr(window,password);
    noecho();

    int natijeh = valid_login(username,password);
    if(natijeh == 1){
        mvwprintw(window,4,1,"login successful");
        wrefresh(window);
        napms(500);
        menu2(window,username);
    }
    else if(natijeh == 0){
        error(window,"incorrect password");
    }
    else if( natijeh == -2){
        error(window , "username doesn't exist");
    }
    else{
        error(window,"no user register");
    }
    wclear(window);
}

void menu_asli(WINDOW *window){
    int entekhab = 0;
    const char *menu[] = {
        "register new user",
        "login user",
        "exit",
    };
    int menu_size = sizeof(menu)/sizeof(menu[0]);
    while (1){
        wclear(window);
        box(window,0,0);
        mvwprintw(window,1,2,"main menu\n");
        for(int i=0 ; i<menu_size ; i++){
            if( i == entekhab){
                wattron(window,A_REVERSE);
                mvwprintw (window,3+i,2,"%s",menu[i]);
                wattroff(window,A_REVERSE);
            }
            else{
                mvwprintw (window,3+i,2,"%s",menu[i]);
            }
        }
        wrefresh(window);
        int c = wgetch(window);
        switch (c){
            case KEY_UP : entekhab = (entekhab - 1 + menu_size) % menu_size;
            break;
            case KEY_DOWN: entekhab = (entekhab + 1) % menu_size;
            break;
            case '\n' : {
                if(entekhab == 0){
                    sabt_user(window);
                }
                else if(entekhab == 1){
                    login_user(window);
                }
                else if( entekhab == 2 ){
                    endwin();
                    return;
                }
                wclear(window);
                break;
            }
        }
    }
}



int main() {
    setlocale(LC_ALL, "");
    initscr();
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    clear();
    cbreak();
    noecho();
    curs_set(0);
    WINDOW *window = newwin(height_map, width_map, 0, 0);
    keypad(window, TRUE);
    menu_asli(window);
    endwin();
    return 0;
}