#include <led_matrix.h>

#include <Wire.h>
#include <SoftwareSerial.h>

#define DATA_PIN 7
#define CLK_PIN 4
#define WRCLK_PIN 6
#define CS_PIN 5

int lost = 0;

long randNumber;

SoftwareSerial mySerial =  SoftwareSerial(10, 11);

/*-------------------------------------------------------------*/
/* -------------- Model : Generating the game board------------*/
/*-------------------------------------------------------------*/

const int width = 32;
const int height = 16;

int gameBoard[WIDTH*HEIGHT];

void initializeBoard(){
  for(int i=0; i<WIDTH*HEIGHT; i++){
    gameBoard[i] = -1;
  }
}

int coordinatesToNumber(int x, int y){
  return(WIDTH*y+x);
}

int numberToCoordinatesX(int n){
  return(n%width);
}

int numberToCoordinatesY(int n){
  return(int(n/width));
}


/*-------------------------------------------------------------*/
/* ------- Model : Generating a random apple somewhere---------*/
/*-------------------------------------------------------------*/

int currentAppleX  = 0;
int currentAppleY = 0;

static LEDMatrix* matrix;

const int applePosition = -2;

void randomApple(){
  randNumber = random(width);
  currentAppleX = int(randNumber);
  randNumber = random(height);
  currentAppleY = int(randNumber);
  matrix -> plot(currentAppleX, currentAppleY, RED);
  gameBoard[coordinatesToNumber(currentAppleX, currentAppleY)]=applePosition;
}

/*----------------------------------------------------*/
/* -------  Model: Giving Birth to the snake ---------*/
/*----------------------------------------------------*/

int snake0X = 2;
int snake0Y = 7;

int snake1X = 1;
int snake1Y = 7;

int snake2X = 0;
int snake2Y = 7;

int head = coordinatesToNumber(snake0X,snake0Y);

int snakeLength = 3;

void snakeBirthOnePlayer(){
  matrix -> plot(snake0X,snake0Y, GREEN);
  gameBoard[coordinatesToNumber(snake0X, snake0Y)] = 0;
  matrix -> plot(snake1X,snake1Y, GREEN);
  gameBoard[coordinatesToNumber(snake1X, snake1Y)] = 1;
  matrix -> plot(snake2X,snake2Y, GREEN);
  gameBoard[coordinatesToNumber(snake2X, snake2Y)] = 2;
}

/*-------------------------------------------------------*/
/* ------- Model : How the snake move by its own---------*/
/*-------------------------------------------------------*/

const int UP = 0;
const int RIGHT = 1;
const int DOWN = 2;
const int LEFT = 3;

int snakeDirection = 1; 

void whichDirection(){
  if((snake0X==snake1X)&&(snake0Y)<(snake1Y)){
      snakeDirection = 0;
  }
  else if((snake0X==snake1X)&&(snake0Y)>(snake1Y)){
      snakeDirection = 2;
  }
  else if((snake0Y==snake1Y)&&(snake0X)<(snake1X)){
      snakeDirection = 3;
  }
  else if((snake0Y==snake1Y)&&(snake0X)>(snake1X)){
      snakeDirection = 1;
  }
}


void move(){
  moveHead();
  if(head==(coordinatesToNumber(currentAppleX, currentAppleY))){
    moveBodyEaten();
    snakeLength ++;
    randomApple();  
  }
  else{
    moveBodyNoEaten();
  }
}

void moveHead(){
    if((snakeDirection == 0)&&(head>=WIDTH)){
      gameBoard[head] = 1;
      head -= WIDTH;
      gameBoard[head] = 0;
      matrix -> plot((numberToCoordinatesX(head)), (numberToCoordinatesY(head)), GREEN);
    }
    else if((snakeDirection == 1)&&(head%WIDTH<31)){
      gameBoard[head] = 1;
      head += 1;
      gameBoard[head] = 0;
      matrix -> plot((numberToCoordinatesX(head)), (numberToCoordinatesY(head)), GREEN);
    }
    else if((snakeDirection == 2)&&(head<=(WIDTH*HEIGHT-1-WIDTH))){
      gameBoard[head] = 1;
      head += WIDTH;
      gameBoard[head] = 0;
      matrix -> plot(numberToCoordinatesX(head), numberToCoordinatesY(head),GREEN);
    }
    else if((snakeDirection == 3)&&(head%WIDTH!=0)){
      gameBoard[head] = 1;
      head -= 1;
      gameBoard[head] = 0;
      matrix -> plot(numberToCoordinatesX(head), numberToCoordinatesY(head), GREEN);
    }
    else{
    Serial.print("You lost!");
    endingScreen();
    }
 }

void moveBodyNoEaten(){
 
  for(int i=width*height-1; i>=0; i--){
    if(gameBoard[i]==(snakeLength-1)){
      matrix -> plot((numberToCoordinatesX(i)), numberToCoordinatesY(i), BLACK);
      gameBoard[i]=-1;
      matrix -> render();
    }
    else if((gameBoard[i]<(snakeLength-1))&&(gameBoard[i]>0)){
      gameBoard[i]++;
      matrix-> render();
    }
    }
  }

void moveBodyEaten(){
    for(int i=0 ; i<WIDTH*HEIGHT; i++){
    if(gameBoard[i]>1){
      gameBoard[i]++;
    }
    }
}


/* ----------------------------------------------------------------------------------------*/
/* ------------------------Model : create the board and begin to play----------------------*/
/* ----------------------------------------------------------------------------------------*/
void setup(){
  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // set the data rate for the SoftwareSerial port
  mySerial.begin(4800);
  
  matrix = new LEDMatrix(DATA_PIN, CLK_PIN, WRCLK_PIN, CS_PIN);
  matrix -> render();
  
  
  openingScreen();
  initializeBoard();
  matrix -> render();
  randomApple();
  matrix -> render();
  snakeBirthOnePlayer();
  matrix -> render();
  delay(1000);
}

void loop(){
  while(!lost){
  move();
  matrix -> render();
   
    if(mySerial.available()>0){
    char instruction = mySerial.read();
    Serial.println(mySerial.read());
    switch(snakeDirection){
         case 0:
           if(instruction==3){snakeDirection = 3;}
           if(instruction==4){snakeDirection = 1;}
          break;
          case 1:
            if(instruction==1){snakeDirection = 0;}
            if(instruction==2){snakeDirection = 2;}
            break;
           case 2:
            if(instruction==3){snakeDirection = 3;}
            if(instruction==4){snakeDirection = 1;}
            break;
           case 3:
            if(instruction==1){snakeDirection = 0;}
            if(instruction==2){snakeDirection = 2;}
            break;
  }
    }
    else{
      Serial.println(40);
    }
  delay(50);
  }
}

/*-------------------------------------------------------------*/
/* ----------------- Model : Screen presentation --------------*/
/*-------------------------------------------------------------*/

void openingScreen(){
  for(int i=0; i<WIDTH; i++){
    for(int j=0; j<HEIGHT; j++){
      matrix -> plot(i, j, GREEN);
    }
    matrix -> render();
    delay(50);
  }
  for(int i=0; i<WIDTH; i++){
    for(int j=0; j<HEIGHT; j++){
      matrix -> plot(i, j, BLACK);
    }
    matrix -> render();
    delay(50);
  }
}

void endingScreen(){
  for(int i=0; i<WIDTH/2+1; i++){
    for(int j=0; j<HEIGHT; j++){
      matrix -> plot(i, j, BLACK);
      matrix -> plot(WIDTH - i,j, BLACK);
      matrix -> render();
      matrix -> plot(WIDTH - i,j, RED);
      matrix -> plot(i, j, RED);
    }
    matrix -> render();
    delay(50);
    lost = 1;
  }
}
