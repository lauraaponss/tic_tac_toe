//R250
//DELIVERABLE 4
//Laura Pons Garcia - Modificado para Linux

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#define N 3
#define MAX 3

// Primero, define la estructura game ANTES de las declaraciones de funciones
struct game {
    char board[N][N];
    int moves[3][N*N];
    char firstplayer;
    char winner;
    int turn_counter;
};

// Función para reemplazar getch() de conio.h
char getch(void) {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    return (buf);
}

// Función para reemplazar getche() de conio.h
char getche(void) {
    char c = getch();
    putchar(c);  // Echo manually
    return c;
}

//functions to draw - Ahora con la estructura ya definida
void header();
char first_player();
void display_guide(struct game[], int);
void string_board(struct game[], int);

//functions to read and check
void display_board(struct game[], int);

//functions to play
int enter_symbol(struct game[],int,int, char, int);
int tic_tac_toe (struct game[], int);
int moves_points (struct game[], char, int, int, int);

int main() {
    //header
    header();
    
    //MENU
    int option = 0; // Inicializar para evitar warnings
    //intitialize the struct components of the game
    struct game mygames[MAX];
    int game=0;
    for (int i=0;i<MAX;i++){
        mygames[i].winner='0'; //all winners start at 0 to recognize empty games
        mygames[i].turn_counter=0; //initialize turn counter
    }
    char symbol, esc;
    int row, column, value_enter_symbol = 0, value_tictactoe = 0;
    
    while (option!=5){
        printf ("\nMENU. Select one of the following options:\n1. Play game.\n2. Restart game.\n3. Delete game.\n4. Statics.\n5. End of program\n");
        printf ("\nOption: ");
        scanf ("%i", &option);
        getchar(); // Limpiar el buffer de entrada
        
        while (option<1||option>5){
            printf ("\nPlease, select a correct option: ");
            scanf ("%i", &option);
            getchar(); // Limpiar el buffer de entrada
        }
        while (option==1&&game==MAX){
            printf ("\nYou can't start another game, select another option: ");
            scanf ("%i", &option);
            getchar(); // Limpiar el buffer de entrada
        }
        
        if (option==1||option==2){
            
            //We need to use the first emtpy game in mygames struct
            for (int i=0;i<MAX;i++){
                if (mygames[i].winner=='0'&&mygames[i].turn_counter==0){
                    game=i;
                    i=MAX;
                }
            }
            int do_not_play=0; //in case there are no games to restart in option 2
            
            //OPTION 1
            if (option==1){
                //who starts
                char start, player2;
                start=first_player();
                if (start=='O') {
                    player2='X';
                } else {
                    player2='O';
                }
                symbol=start;
                mygames[game].firstplayer=symbol;
                
                //initialize guide board
                char board_guide[N][N];
                for (int i=0; i<N; i++) {
                    for (int j=0; j<N; j++) {
                        board_guide[i][j]='-';
                    }
                }
                
                //initialize game board
                for (int i=0; i<N; i++) {
                    for (int j=0; j<N; j++) {
                        mygames[game].board[i][j]='-';
                    }
                }
                
                //initialize move points
                for (int i=0; i<3; i++) {
                    for (int j=0; j<N*N; j++) {
                        mygames[game].moves[i][j]=0;
                    }
                }
                
                //initialize turn counter
                mygames[game].turn_counter=0;
                
                //initialize value of functions enter_symbol, value_tictactoe and esc
                value_enter_symbol=0;
                value_tictactoe=0;
                esc='0';
            }
            
            //OPTION 2
            else if (option==2){
                int answer, games_available=0;    
                for (int i=0;i<MAX;i++){        
                    if (mygames[i].winner=='s'){
                        printf ("Game %i can be restarted\n", i+1);
                        games_available++;
                    }
                }
                if (games_available==0){
                    printf ("There are no games aviable to restart\n");
                    do_not_play=1;
                    value_enter_symbol=0;
                    value_tictactoe=0;
                }
                else{
                    printf ("Which game do you want to restart? ");
                    scanf ("%i", &answer);
                    getchar(); // Limpiar el buffer de entrada
                    while (mygames[answer-1].winner!='s'){
                        puts("This game can't be restarted, please select an aviable game: ");
                        scanf ("%i", &answer);
                        getchar(); // Limpiar el buffer de entrada
                        answer-1;
                    }
                    game=answer-1;
                    esc='0';
                }
                
                //Setting the correct symbol value
                if (mygames[game].turn_counter%2==0){
                    symbol=mygames[game].firstplayer;
                }
                else{
                    if (mygames[game].firstplayer=='X') {
                        symbol='O';
                    } 
                    else {
                        symbol='X';
                    }
                }
            }
            
            //GAME
            while(mygames[game].turn_counter<(N*N)&&value_tictactoe!=3&&esc!=27&&do_not_play==0){
                display_guide(mygames,game);
                display_board(mygames,game);
                printf ("\nTurn for %c\n", symbol);
                //Offering to stop the game
                printf ("\nPress any key to start your turn, if you press ESC the game will be stopped: ");
                esc=getche();
                if (esc==27){
                    printf("\nThe game has been stopped, you can restart it later\n");
                    mygames[game].winner='s';
                    game++;
                }
                else{
                    //asking the row
                    printf ("\nIntroduce a row: ");
                    scanf ("%i", &row);
                    getchar(); // Limpiar el buffer de entrada
                    row=row-1;
                    
                    //checking if the value of the row is correct
                    while (row<0 || row>N-1) {
                        printf ("\nIntroduce a valid row: ");
                        scanf ("%i", &row);
                        getchar(); // Limpiar el buffer de entrada
                        row=row-1;
                    }
                    
                    //asking the column
                    printf ("\nIntroduce a column: ");
                    scanf ("%i", &column);
                    getchar(); // Limpiar el buffer de entrada
                    column=column-1;
                    
                    //checking if the value of the column is correct
                    while (column<0||column>N-1) {
                        printf ("\nIntroduce a valid column: ");
                        scanf ("%i", &column);
                        getchar(); // Limpiar el buffer de entrada
                        column=column-1;
                    }
                    value_enter_symbol=enter_symbol(mygames,row,column, symbol, game);
                    
                    //OCCUPIED POSITION
                    while (value_enter_symbol==1){
                        puts ("Please, introduce non occupied position");
                        printf ("\nRow: ");
                        scanf ("%i", &row);
                        getchar(); // Limpiar el buffer de entrada
                        row=row-1;
                        while (row+1<1||row+1>N) {
                            printf ("\nIntroduce a valid row: ");
                            scanf ("%i", &row);
                            getchar(); // Limpiar el buffer de entrada
                            row=row-1;
                        }
                        printf ("\nColumn: ");
                        scanf ("%i", &column);
                        getchar(); // Limpiar el buffer de entrada
                        column=column-1;
                        while (column+1<1||column+1>N) {
                            printf ("\nIntroduce a valid column: ");
                            scanf ("%i", &column);
                            getchar(); // Limpiar el buffer de entrada
                            column=column-1;
                        }
                        value_enter_symbol=enter_symbol(mygames,row,column,symbol, game);
                    }
                    
                    //CORRECT POSITION            
                    if (value_enter_symbol==2) {
                        mygames[game].board[row][column]=symbol;
                        display_guide(mygames,game);
                        display_board(mygames,game);
                        //saving positions and points in the moves array
                        int counter;
                        counter=mygames[game].turn_counter;
                        mygames[game].moves[0][counter]=row;
                        mygames[game].moves[1][counter]=column;
                        mygames[game].moves[2][counter]=moves_points(mygames, symbol, row, column, game);
        
                        //change of turn
                        if (symbol=='X') {
                            symbol='O';
                        } else if (symbol=='O') {
                            symbol='X';
                        }
                        mygames[game].turn_counter++;
                        value_tictactoe=tic_tac_toe(mygames, game);
                    }
                }
            }
            
            //At this point, or all positions have been entered or there is tic tac toe
            //TIE
            if (mygames[game].turn_counter==N*N&&value_enter_symbol==2&&value_tictactoe!=3){ //If there is no tic tac toe, there is a tie
                printf("\n\nTIE!!\n\n");
                //FINAL BOARD
                printf("\nFINAL BOARD:\n");
                string_board(mygames,game);    
                int points_first=0, points_second=0;
                //Sum of the points of each player in each turn
                for (int i=0;i<N*N;i++){
                    if (i%2==0){
                        points_first=points_first+mygames[game].moves[2][i];
                    }
                    else{
                        points_second=points_second+mygames[game].moves[2][i];
                    }
                }
                //If both players have same points, we set the winner to the second player
                if (points_first==points_second){
                    if (mygames[game].firstplayer=='X'){
                        mygames[game].winner='O';    
                    }
                    else{
                        mygames[game].winner='X';
                    }
                    printf ("\nFirst player points: %i\nSecond player points: %i\n", points_first, points_second);
                    printf ("\nThe winner is the second player: %c\n", mygames[game].winner);
                }
                //Else the winner with more points wins
                else{
                    if (points_first<points_second){
                        if (mygames[game].firstplayer=='X'){
                            mygames[game].winner='O';    
                        }
                        else{
                            mygames[game].winner='X';
                        }
                    }
                    else{
                        mygames[game].winner=mygames[game].firstplayer;
                    }
                    printf ("\nPOINTS:\nFirst player points: %i\nSecond player points: %i\n", points_first, points_second);
                    printf ("The winner by points is: %c!!!\n", mygames[game].winner);
                }
                game++;
            }
            
            //TIC TAC TOE
            else if (value_tictactoe==3){ //changing the symbol to print correctly the winner, since it changes when a correct position is entered
                if (symbol=='X'){
                    symbol='O';
                }
                else{
                    symbol='X';
                }
                mygames[game].board[row][column]=symbol;
                printf("\n    TIC TAC TOE!!!");
                printf("\n    %c WINS!\n", symbol);
                mygames[game].winner=symbol;
                //FINAL BOARD
                printf("\nFINAL BOARD:\n");
                string_board(mygames,game);    
                game++;
            }
        }
        
        //OPTION 3
        else if (option==3){
            int delete_game=0, to_delete=0;
            for (int i=0;i<MAX;i++){
                if (mygames[i].winner!='0'){
                    printf ("Game %i can be deleted\n", i+1);
                    delete_game=1;
                }
            }
            if (delete_game==1){
                printf ("\nWhich game do you want to delete?\n");
                scanf ("%i", &to_delete);
                getchar(); // Limpiar el buffer de entrada
                to_delete--;
                while (mygames[to_delete].winner=='0'){
                    printf ("Please, select an started or finished game\n");
                    scanf ("%i", &to_delete);
                    getchar(); // Limpiar el buffer de entrada
                    to_delete--;
                }
                
                //DELETING SELECTED GAME
                //initialize game board
                for (int i=0; i<N; i++) {
                    for (int j=0; j<N; j++) {
                        mygames[to_delete].board[i][j]='-';
                    }
                }
                //initialize move points
                for (int i=0; i<3; i++) {
                    for (int j=0; j<N*N; j++) {
                        mygames[to_delete].moves[i][j]=0;
                    }
                }
                //initialize turn counter
                mygames[to_delete].turn_counter=0;
                //initialize winner to 0
                mygames[to_delete].winner='0';
            }
            else{
                printf ("There are no games available to delete\n");
            }
            
        }
        
        else if (option==4){
            printf ("\nSTATICS\n");
            int played_games=0, ended_games=0, stopped_games=0, tictactoe_games=0, stalemate_games=0;
            
            //PLAYED GAMES
            played_games=0;
            for (int i=0;i<MAX;i++){
                if (mygames[i].winner=='s'||mygames[i].winner=='O'||mygames[i].winner=='X'){
                    played_games++;
                }
            }
            printf ("\nNumber of played games: %i", played_games);
            
            //ENDED GAMES
            ended_games=0;
            for (int i=0;i<MAX;i++){
                if (mygames[i].winner=='O'||mygames[i].winner=='X'){
                    ended_games++;
                }
            }
            printf ("\nNumber of ended games: %i", ended_games);
            
            //STOPPED GAMES
            stopped_games=0;
            for (int i=0;i<MAX;i++){
                if (mygames[i].winner=='s'){
                    stopped_games++;
                }
            }
            printf ("\nNumber of stopped games: %i", stopped_games);
            
            //GAMES WON BY TIC TAC TOE
            tictactoe_games=0;
            stalemate_games=0;
            for (int i=0;i<MAX;i++){
                if (mygames[i].winner=='O'||mygames[i].winner=='X'){
                    if (tic_tac_toe(mygames,i)==3){
                        tictactoe_games++;
                    }
                    else{
                        stalemate_games++;
                    }
                }
            }
            printf("\nNumber of games won getting tic tac toe: %i", tictactoe_games);
            printf("\nNumber of games won by stalemate: %i\n", stalemate_games);
        }
    }
    
    return 0; // Añadido return para evitar warnings
}

//title
void header() {
    int i;
    putchar('+');  // Cambiado 201 por +
    for(i=1; i<=29; i++) putchar('-');  // Cambiado 205 por -
    putchar('+');  // Cambiado 187 por +
    puts("");
    putchar('|');  // Cambiado 186 por |
    printf(" DELIVERABLE 4 - TIC TAC TOE ");
    putchar('|');  // Cambiado 186 por |
    puts("");
    putchar('+');  // Cambiado 200 por +
    for(i=1; i<=29; i++) putchar('-');  // Cambiado 205 por -
    putchar('+');  // Cambiado 188 por +
}

//function that determintes if x or o starts
char first_player() {
    char start;
    puts("\nWhich player starts? X or O?\n");
    start=getche();
    while (start!='x'&&start!='X'&&start!='o'&&start!='O') {
        puts("\nWhich player starts? X or O?\n");
        start=getche();
    }
    if(start=='x'||start=='X') {
        puts("\nFirst player plays with X, and second player plays with O");
        start='X';
    }

    else {
        puts ("\nFirst player plays with O, and second player plays with X");
        start='O';
    }
    return start;
}

//function that prints the guide board
void display_guide(struct game mygames[], int game) {
    printf("\nBoard guide:\n\n");

    for (int i=0; i<N; i++) {
        puts(" ");
        printf (" ");
        int j;
        for (j=0; j<N; j++) {
            if (j<N-1) {
                if (mygames[game].board[i][j]=='-') {
                    printf(" %i.%i |", i+1, j+1);  // Cambiado 179 por |
                } else {
                    printf ("     |");  // Cambiado 179 por |
                }
            } else {
                if (mygames[game].board[i][j]=='-') {
                    printf(" %i.%i", i+1, j+1);
                } else {
                    printf ("     ");
                }
            }
        }
        puts("");
        printf (" ");
        if (i<N-1) {
            for (int k=0; k<N; k++) {
                for (int z=0; z<5; z++) {
                    putchar ('-');  // Cambiado 196 por -
                }
                if (k<N-1) {
                    putchar ('+');  // Cambiado 197 por +
                }
            }
        }
    }
}

//function that analices each position to print the game board
void display_board(struct game mygames[], int game) {
    puts ("\nCurrent game board:");
    puts("");

    for (int i=0; i<N; i++) {
        puts(" ");
        printf (" ");
        int j;
        for (j=0; j<N; j++) {
            if (j<N-1) {
                printf("  %c  |", mygames[game].board[i][j]);  // Cambiado 179 por |
            } else {
                printf("  %c", mygames[game].board[i][j]);
            }
        }
        puts("");
        printf (" ");
        if (i<N-1) {
            for (int k=0; k<N; k++) {
                for (int z=0; z<5; z++) {
                    putchar ('-');  // Cambiado 196 por -
                }
                if (k<N-1) {
                    putchar ('+');  // Cambiado 197 por +
                }
            }
        }
    }
}

//function that analices if the optition introduced is correct
int enter_symbol(struct game mygames[],int row, int column, char symbol, int game) {
    //OCCUPIED POSITION
    for (int i=0; i<N; i++) {
        if (i==row) {
            for (int j=0; j<N; j++) {
                if (j==column) {
                    if (mygames[game].board[i][j]!='-') {
                        return 1;
                    }
                }
            }
        }
    }
//CORRECT POSITION
    for (int i=0; i<N; i++) {
        if (i==row) {
            for (int j=0; j<N; j++) {
                if (j==column) {
                    if (mygames[game].board[i][j]=='-') {
                        int valuetictactoe;
                        valuetictactoe=tic_tac_toe(mygames, game);
                            if (valuetictactoe==0){ //there is not tic tac toe
                                return 2;
                            }
                            else if (valuetictactoe==3){ //there is tic tac toe
                                return 3;
                            }
                    }
                }
            }
        }
    }
    return 0; // Añadido return para evitar warnings
}

//function that analices if there is tic tac toe
int tic_tac_toe(struct game mygames[], int game){
    int counter;
    counter=1;
    //tic tac toe horizontal
    for(int i=0; i<N; i++){ // Corregido: i<N-1 -> i<N
        for(int j=0; j<N-1; j++){
            if(mygames[game].board[i][j]==mygames[game].board[i][j+1]&&mygames[game].board[i][j]!='-'){ // Añadida comprobación en primera posición
                counter++;
                if(counter==N){
                    return 3;
                }
            }
            else{
                counter=1;
            }
        }
        counter=1; // Reiniciar counter para cada fila
    }
    counter=1;
    //tic tac toe vertical    
    for(int j=0; j<N; j++){ // Corregido: j<N-1 -> j<N
        for(int i=0; i<N-1; i++){
            if(mygames[game].board[i][j]==mygames[game].board[i+1][j]&&mygames[game].board[i][j]!='-'){ // Añadida comprobación en primera posición
                counter++;
                if(counter==N){
                return 3;
                }
            }
            else{
                counter=1;
            }
        }
        counter=1; // Reiniciar counter para cada columna
    }
    counter=1;
    
    //tic tac toe main diagonal principal left-right
    // Comprobar diagonal principal
    for(int i=0; i<N-1; i++){
        if(mygames[game].board[i][i]==mygames[game].board[i+1][i+1] && mygames[game].board[i][i]!='-'){
            counter++;
            if(counter==N){
                return 3;
            }
        }
        else{
            counter=1;
        }
    }
    
    counter=1;    
    //tic tac toe diagonal secundaria right-left
    // Comprobar diagonal secundaria
    for(int i=0; i<N-1; i++){
        if(mygames[game].board[i][N-1-i]==mygames[game].board[i+1][N-2-i] && mygames[game].board[i][N-1-i]!='-'){
            counter++;
            if(counter==N){
                return 3;
            }
        }
        else{
            counter=1;
        }
    }

    return 0;
}

//function that displays the final board wuth double lines
void string_board(struct game mygames[], int game){
    char string[650]; //char with 650 characters to know that there are enough spaces
    int counter=0;

    for(int i=0; i<N; i++){
        for(int j=0; j<N; j++){
            if (j!=N-1){ //columns with lines between them
                string[counter]=' ';
                counter++;
                string[counter]=mygames[game].board[i][j];
                counter++;
                string[counter]=' ';
                counter++;
                string[counter]='|';  // Cambiado 186 por |
                counter++;
            }
            else { //last column, without line
                string[counter]=' ';
                counter++;
                string[counter]=mygames[game].board[i][j];
                counter++;
                string[counter]=' ';
                counter++;    
            }
        }
        string[counter]='\n';
        counter++;
        if (i!=N-1){ //rows with lines between them
            for(int k=1; k<4*N; k++){
                int resto;
                resto=k%4;
                if(resto==0){
                    string[counter]='+';  // Cambiado 206 por +
                    counter++;
                }
                else if(resto!=0){
                    string[counter]='-';  // Cambiado 205 por -
                    counter++;    
                }
            }
            string[counter]='\n';
            counter++;
        }
    }    
    for (int i=0; i<counter; i++){
        printf("%c", string[i]);
    }
}

//function that analices the points giveen in each turn
int moves_points (struct game mygames[], char symbol, int row, int column, int game){
    
    int counter_symbol=0, counter_oponent=0;
    char oponent;
    int points=0;
    if (symbol=='O'){
        oponent='X';
    } 
    else {
        oponent='O';
    }
    
    //tic tac toe horizontal
    for (int j=0;j<N;j++){
        if(mygames[game].board[row][j]==symbol){
            counter_symbol++;        
        }
        else if(mygames[game].board[row][j]==oponent){
            counter_oponent++;        
        }
    }
    //points horizontal
    if (counter_symbol==N-1&&counter_oponent==0){
        points=points+1;
    }
    else if (counter_symbol==1&&counter_oponent==N-1){
        points=points+2;
    }
    counter_symbol=0;
    counter_oponent=0;

    //tic tac toe vertical
    for (int i=0;i<N;i++){
        if(mygames[game].board[i][column]==symbol){
            counter_symbol++;        
        }
        else if(mygames[game].board[i][column]==oponent){
            counter_oponent++;        
        }
    }
    //points vertical
    if (counter_symbol==N-1&&counter_oponent==0){
        points=points+1;
    }
    else if (counter_symbol==1&&counter_oponent==N-1){
        points=points+2;
    }
    counter_symbol=0;
    counter_oponent=0;
    
    //tic tac toe diagonal left-right
    if (row==column){
        for (int i=0; i<N; i++) {
            if (mygames[game].board[i][i]==symbol) {
                counter_symbol++;
            }
            else if (mygames[game].board[i][i]==oponent) {
                counter_oponent++;
            }
        }
        //points diagonal left-right
        if (counter_symbol==N-1&&counter_oponent==0){
            points=points+1;
        }
        else if (counter_symbol==1&&counter_oponent==N-1){
            points=points+2;
        }
    }
    counter_symbol=0;
    counter_oponent=0;
    
    //tic tac toe diagonal right-left
    if (row+column==N-1){
        for (int i=0; i<N; i++) {
            if (mygames[game].board[i][N-1-i]==symbol) {
                counter_symbol++;
            }
            else if (mygames[game].board[i][N-1-i]==oponent) {
                counter_oponent++;
            }
        }
        //points diagonal right-left
        if (counter_symbol==N-1&&counter_oponent==0){
            points=points+1;
        }
        else if (counter_symbol==1&&counter_oponent==N-1){
            points=points+2;
        }
    }
    return points;
}