#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dos.h>
#include <time.h>

#define TRUE  1
#define FALSE 0
#define UP 72
#define LEFT 75
#define RIGHT 77
#define D     32
#define DOWN 80
#define HZ   50
#define MEMORY_ADRESS 0xB8000000
#define PIT_CHANNEL0      0x40
#define PIT_CMDREG        0x43

#define DETERMINE_ADRESS(row,column) ( ( row*80 + column ) * 2 )
#define SWAP(variable1,variable2)    ( variable1 ^= variable2 ^= variable1 ^= variable2 )
#define CLEARVARIABLE(destination)   ( writeScreen(destination,'0',0) )

struct coordinate
{
    int column;
    int row;
};

struct monster
{
    struct coordinate monster_location;
    int a;
    struct monster *next;
};

typedef struct monster monsterStruct;
typedef monsterStruct *monsterPointer;
typedef struct coordinate location;

int openFile(FILE **file,char *arguman,char *type);
int setCoordinateAndReturnColor(char eleman_number , int row_number , int column_number);
int checkDestination(int destination, int isPackman);
int determineVariable(int adress);
int randomMove();
void closeFile(FILE *file);
void readFile(FILE *input_file);
void setCoordinate(struct coordinate *variable, int column_number , int row_number );
void writeScreen(int adress, char variable, int color);
void checkPressedKey(unsigned char scanCode);
void interrupt (far *OldKeyboardHandler) () = 0;
void interrupt far KeyboardInterruptHandler();
void interrupt far timer();
void interrupt (far *old_timer_handler)() = 0;
void newKeyboardHandler();
void newTimerHandler();
int moveP(location *current_location ,int current_adress,int destination_row, int destination_column,int isPackman);
void restoreKeyboardHandler();
void restoreTimerHandler();
void monsterEkle(int row, int column);
void moveMonsters();
void timer_set();
void writeDamp();

location packman_location;
location matrix_size;
monsterPointer first_monster = NULL, last_monster= NULL;

int timer_ticks=0;
char *output_prefix;

int main(int argc , char *argv[]){
    FILE *input_file;
    srand(time(NULL));
    if(argc > 0)
    {
        if( openFile(&input_file,argv[1],"r") )
        {
            output_prefix = argv[2];
            newKeyboardHandler();

            readFile(input_file);                      //if file is open, read table
            closeFile(input_file);                     //close file after process done

            timer_set();
            newTimerHandler();
        }
        else
            printf("No file");                          //there is no file
    }
    getch();
    return 0;
}

void closeFile(FILE *file){
    fclose(file);
}

int openFile(FILE **file, char *arguman , char *type ){
    if( (*file =fopen( arguman, type)) != NULL)
      return TRUE;
    else
      return FALSE;
}

void readFile(FILE *input_file){
     char *readed;
     int column_number, row_number = 0 , column_size , row_size;

     fscanf(input_file,"%s",readed);                        //read column size and row size

     column_size = atoi( strtok(readed,"xX") );      //determine column size
     row_size    = atoi( strtok(NULL,"xX") );        //determine row size
     setCoordinate( &matrix_size, row_size, column_size );

     for(; row_number < matrix_size.row; row_number++)
     {
        fscanf(input_file,"%s",readed);

        for(column_number = 0; column_number < matrix_size.column; column_number++)
        {
            int adress         = DETERMINE_ADRESS(row_number,column_number);
            char eleman_number = readed[column_number];
            int color          = setCoordinateAndReturnColor( eleman_number , row_number , column_number );
            writeScreen(adress, eleman_number , color);
        }
     }
}

int setCoordinateAndReturnColor(char eleman_number, int row_number , int column_number ){
    switch(eleman_number){
        case '1':                           return 5;
        case '2': monsterEkle(row_number, column_number);                      return 7;
        case '3':                           return 2;
        case '4': setCoordinate(&packman_location, row_number,column_number);  return 4;
        default : return 0;
    }
}

void setCoordinate(struct coordinate *variable, int row_number, int column_number  ){
    variable->column = column_number;
    variable->row    = row_number;
}

void interrupt far KeyboardInterruptHandler(){
    unsigned char scanCode,command_register;

    disable();
    scanCode = inportb(0x60);               //determine scan code

    command_register = inportb(0x61);          //reset the command register
    command_register = command_register | 0x80;

    checkPressedKey(scanCode);

    outportb(0x61, command_register);

    outportb(0x20, 0x20);
    enable();
}

void interrupt far timer(){
    timer_ticks++;

    if (timer_ticks % HZ == 0)
    {
       moveMonsters();
    }

	outportb(0x20, 0x20);
}

void moveMonsters(){
    monsterPointer current_monster = first_monster;

    while(current_monster != NULL){
        int moved=0;
        int monster_adress = DETERMINE_ADRESS( current_monster->monster_location.row, current_monster->monster_location.column);
        int direction = randomMove();

        do{
            direction++;                    /*direction variable firstly take a random direction then check other directions */
            direction = direction % 4;
            switch(direction){
                case 0 : moved = moveP(&(current_monster->monster_location), monster_adress, current_monster->monster_location.row-1, current_monster->monster_location.column  ,FALSE);  break;
                case 1 : moved = moveP(&(current_monster->monster_location), monster_adress, current_monster->monster_location.row+1, current_monster->monster_location.column  ,FALSE);  break;
                case 2 : moved = moveP(&(current_monster->monster_location), monster_adress, current_monster->monster_location.row  , current_monster->monster_location.column+1,FALSE);  break;
                case 3 : moved = moveP(&(current_monster->monster_location), monster_adress, current_monster->monster_location.row  , current_monster->monster_location.column-1,FALSE);  break;
            }
        }
        while(!moved);

        current_monster = current_monster->next;
    }
}

void checkPressedKey(unsigned char scanCode){
    if( scanCode & 0x80){
        int packman_adress = DETERMINE_ADRESS(packman_location.row, packman_location.column);

        switch(scanCode & 0x7f){
            case UP     :  moveP(&packman_location, packman_adress, packman_location.row-1, packman_location.column  ,TRUE);  break;
            case DOWN   :  moveP(&packman_location, packman_adress, packman_location.row+1, packman_location.column  ,TRUE);  break;
            case RIGHT  :  moveP(&packman_location, packman_adress, packman_location.row  , packman_location.column+1,TRUE);  break;
            case LEFT   :  moveP(&packman_location, packman_adress, packman_location.row  , packman_location.column-1,TRUE);  break;
            case D      :  writeDamp();
        }
    }
}

void writeDamp(){
    FILE *output_file;
    static int damp_count = 0;
    char *output_text;

    damp_count++;
    sprintf(output_text,"%s%d.txt" , output_prefix,damp_count);

    if(openFile(&output_file,output_text,"w"))
    {
        int column = 0, row = 0;

        for(; row < matrix_size.row; row++)
        {
            for(column = 0; column < matrix_size.column; column++)
            {
                int address  = DETERMINE_ADRESS( row, column);
                fprintf(output_file,"%c",determineVariable(address));
            }
            fprintf(output_file,"\n");
        }
    }
}

int moveP(location *current_location ,int current_adress,int destination_row, int destination_column, int isPackman){
        int destination  = DETERMINE_ADRESS( destination_row, destination_column);
        int canMove      = checkDestination(destination, isPackman);
        char far *screen = (char far *) MEMORY_ADRESS;

        if(canMove == TRUE){
            SWAP(screen[destination]  , screen[current_adress] );
            SWAP(screen[destination+1], screen[current_adress+1] );
            setCoordinate(current_location, destination_row, destination_column );
            return TRUE;
        }
        else
            return FALSE;
}

void restore(){
    restoreKeyboardHandler();
    restoreTimerHandler();
}

int checkDestination(int destination, int isPackman){
    int variable = determineVariable(destination);

    if( variable == ' ' && isPackman){              /*check whether the packman reach end of road*/
        restore();
        return FALSE;
    }

    else{
        switch(variable){                           /*check whether destination is clear or not*/
            case '0': return  TRUE;
            case '1': return FALSE;
            case '2': return FALSE;
            case '3': if(isPackman) CLEARVARIABLE(destination);             return TRUE;  /*if packman eats diamand, the diamand will disappear*/
            case '4': restore(); CLEARVARIABLE(destination);  return TRUE;   /*if one monster eats packman, the game over*/
        }
    }
}

void writeScreen(int adress, char variable, int color){
    char far *screen = (char far *) MEMORY_ADRESS;
    screen[adress]   = variable;
    screen[adress+1] = color;
}

int determineVariable(int adress){
    char far *screen = (char far *) MEMORY_ADRESS;
    return screen[adress];
}

void newKeyboardHandler(){
    OldKeyboardHandler = getvect(9);
    setvect(9, KeyboardInterruptHandler);
}

void newTimerHandler(){
    if (old_timer_handler != 0) return;

    old_timer_handler = getvect(8);
    setvect(8, timer);
}

void restoreKeyboardHandler(){
    setvect(9, OldKeyboardHandler);
    OldKeyboardHandler = 0;
}

void restoreTimerHandler(){
    if(old_timer_handler == 0) return;

    setvect(8, old_timer_handler);
    old_timer_handler = 0;
}

void timer_set(){
    int divisor = 1193180 / HZ;
    outportb(PIT_CMDREG , 0x34);
    outportb(PIT_CHANNEL0, divisor & 0xFF);
    outportb(PIT_CHANNEL0, divisor >> 8);
}

void monsterEkle(int row, int column){
    monsterPointer new_monster = malloc( sizeof(monsterStruct));

    new_monster->monster_location.row = row;
    new_monster->monster_location.column = column;
    new_monster->next = NULL;

    if(first_monster == NULL)
        first_monster = last_monster = new_monster;
    else{
        last_monster->next = new_monster;
        last_monster = new_monster;
    }
}

int randomMove(){
    return rand()%4;
}
