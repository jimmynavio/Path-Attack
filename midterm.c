#include <stdio.h>
#include <stdlib.h>

typedef struct{
  char status;
  int checked;
} square;

int size = 0;
square** gameboard;

int randomNum(int);
void initBoard();
void printBoard();
void humanTurn(char);
void compTurn(char);
void resetChecked();
char checkBoard();
void checkSquare(int, int);

int main(int argc, char *argv[]){
  srand(time(NULL));
  if(argc < 3){
    printf( "usage: %s size humans (0 - 2)\n", argv[0] );
    exit(1);
  }
  
  size = atoi(argv[1]);
  int humans = atoi(argv[2]);
  initBoard();
  
  int i;
  for(i=0; i<size*size; i++){
    if(i%2 == 0){
      if(humans > 0)
        humanTurn('V');
      else
        compTurn('V');
    }else{
      if(humans > 1)
        humanTurn('H');
      else
        compTurn('H');
    }
        
    char winner = checkBoard();
    if(winner > -1){
      printBoard();
      printf("%c WINS!\n", winner);
      break;
    }
  }
}

int randomNum(int max){
  int r = (rand() % (max));

  return r;
}

void initBoard(){
  square* temp;
  int i,j;
  
  gameboard = (square**)malloc(size * sizeof(square*));
  temp = (square*)malloc(size * size * sizeof(square));
  for (i = 0; i < size; i++) {
    gameboard[i] = temp + (i * size);
  }
  
  for(i=0; i<size; i++){
    for(j=0; j<size; j++){
      gameboard[i][j].status = '0';
    }
  }
}

void printBoard(){
  printf("------------------------\n");
  printf("GAMEBOARD OF SIZE %dx%d \n", size, size);
  printf("------------------------\n");
  
  int i,j;
  for(i=0; i<size; i++){
    for(j=0; j<size; j++){
      printf("%c ",gameboard[i][j].status);
    }
    printf("\n");
  }
  
  printf("\n");
}

void humanTurn(char player){
  int x, y;
  
  printBoard();
  printf("\n%c's turn! Enter a location (x y): ", player);
  scanf("%d %d",&y,&x);
  
  while(outOfBounds(x) || outOfBounds(y)){
    printBoard();
    printf("\nOut of bounds! Try another location (x y): ", player);
    scanf("%d %d",&y,&x);
  }
  
  while(gameboard[x][y].status != '0'){
    printBoard();
    printf("\nSpot taken! Try another location (x y): ", player);
    scanf("%d %d",&y,&x);
  }
  
  gameboard[x][y].status = player;
  printBoard();
}

void compTurn(char player){
  int x = randomNum(size);
  int y = randomNum(size);
  
  while(gameboard[x][y].status != '0'){
    x = randomNum(size);
    y = randomNum(size);
  }
  
  gameboard[x][y].status = player;
}

void resetChecked(){
  int i, j;
  for(i=0; i<size; i++){
    for(j=0; j<size; j++){
      gameboard[i][j].checked = -1;
    }
  }  
}

char checkBoard(){
  int i;
  for(i=0; i<size; i++){
    resetChecked();
    if(checkV(0,i) > -1){
      return 'V';
    }
  }
  
  for(i=0; i<size; i++){
    resetChecked();
    if(checkH(i,0) > -1){
      return 'H';
    }
  }
    
  return -1;
}

int checkV(int x, int y){
  if (outOfBounds(x) || outOfBounds(y)) return -1;
  if (gameboard[x][y].status != 'V') return -1;
  if (gameboard[x][y].checked == 1) return -1;
  
  gameboard[x][y].status = '.';
  if (x == size-1) return 1;
  
  gameboard[x][y].checked = 1;
  if (checkV(x-1, y-1) == 1) return 1;
  if (checkV(x  , y-1) == 1) return 1;
  if (checkV(x+1, y-1) == 1) return 1;
  if (checkV(x+1, y  ) == 1) return 1;
  if (checkV(x+1, y+1) == 1) return 1;
  if (checkV(x  , y+1) == 1) return 1;
  if (checkV(x-1, y+1) == 1) return 1;
  if (checkV(x-1, y  ) == 1) return 1;
  
  gameboard[x][y].status = 'V';
  return -1;
}

int checkH(int x, int y){
  if (outOfBounds(x) || outOfBounds(y)) return -1;
  if (gameboard[x][y].status != 'H') return -1;
  if (gameboard[x][y].checked == 1) return -1;
  
  gameboard[x][y].status = ',';
  if (y == size-1) return 1;
  
  gameboard[x][y].checked = 1;
  if (checkH(x-1, y-1) == 1) return 1;
  if (checkH(x  , y-1) == 1) return 1;
  if (checkH(x+1, y-1) == 1) return 1;
  if (checkH(x+1, y  ) == 1) return 1;
  if (checkH(x+1, y+1) == 1) return 1;
  if (checkH(x  , y+1) == 1) return 1;
  if (checkH(x-1, y+1) == 1) return 1;
  if (checkH(x-1, y  ) == 1) return 1;
  
  gameboard[x][y].status = 'H';
  return -1;
}

int outOfBounds(int position){
  return position >= size || position < 0;
}
