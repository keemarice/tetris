#include <gamma.h> //works in arduino
#include <RGBmatrixPanel.h> //works in arduino
#include <Adafruit_GFX.h> //works in arduino
#include <math.h>
#define VRX_PIN  A4 
#define VRY_PIN  A5
int SW = 10;
// define the wiring of the LED screen
const uint8_t CLK  = 8;
const uint8_t LAT = A3;
const uint8_t OE = 9;
const uint8_t A = A0;
const uint8_t B = A1;
const uint8_t C = A2;
const char FILL = '*';
const char EMPTY = '-';
const char DEAD = 'X';
void printTetris();
int ij = 0;

// define the wiring of the inputs
const int POTENTIOMETER_PIN_NUMBER = 5;
const int BUTTON_PIN_NUMBER = 10;

// a global variable that represents the LED screen
RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);
char tetris[32][16];

class Color {
 public:
   int red;
   int green;
   int blue;
   Color() {
     red = 0;
     green = 0;
     blue = 0;
   }
   Color(int r, int g, int b) {
     red = r;
     green = g;
     blue = b;
   }
   uint16_t to_333() const {
     return matrix.Color333(red, green, blue);
   }
  char colorToChar(){
     //RED
  
     if(red == 4 && green == 0 && blue == 0){
      return 'R';
     }
     else if (red == 4 && green == 4 && blue == 0) {
       return 'Y';
     }
     else if (red == 0 && green == 0 && blue == 4){
       return 'B';
     }
     else {
       return DEAD;
     }
   }
   
};

const Color BLACK(0, 0, 0);
const Color RED(4, 0, 0);
const Color ORANGE(6, 1, 0);
const Color YELLOW(4, 4, 0);
const Color GREEN(0, 4, 0);
const Color BLUE(0, 0, 4);
const Color PURPLE(1, 0, 2);
const Color WHITE(4, 4, 4);
const Color LIME(2, 4, 0);
const Color AQUA(0, 4, 4);

class Blocks{
public:
  Blocks(){
    x = 0;
    y = 8;
    isBottom = false;
    type = "cube";
    r = 0;
  }

  void drawCube(){
    drawSeg(YELLOW, x, y, false);
    drawSeg(YELLOW, x + 2, y, false);
    drawSeg(YELLOW, x + 2, y + 2, false);
    drawSeg(YELLOW, x, y + 2, false);
  }

  void eraseCube(){
    drawSeg(BLACK, x, y, true);
    drawSeg(BLACK, x + 2, y, true);
    drawSeg(BLACK, x + 2, y + 2, true);
    drawSeg(BLACK, x, y + 2, true);
  }
  void drawT(){
    drawSeg(PURPLE, x, y, false);
    drawSeg(PURPLE, x + 2, y, false);
    drawSeg(PURPLE, x + 2, y - 2, false);
    drawSeg(PURPLE, x + 4, y, false);
  }
  void eraseT(){
    drawSeg(BLACK, x, y, true);
    drawSeg(BLACK, x + 2, y, true);
    drawSeg(BLACK, x + 2, y - 2, true);
    drawSeg(BLACK, x + 4, y, true);
  }
  void draw(){
    if(type == "cube"){
      drawCube();
    }
    else if (type == "L"){
      drawL();
    }
    else if (type == "I"){
      drawI();
    }
  
    else if (type == "L90"){
      drawL90();
    }
    else if (type == "L180"){
      drawL180();
    }
    else if (type == "L270"){
      drawL270();
    }
  }
  void erase(){
    if(type == "cube"){
      eraseCube();
    }
    else if (type == "L"){
      eraseL();
    }
    else if (type == "I"){
      eraseI();
    }
 
     else if (type == "L90"){
      eraseL90();
    }
    else if (type == "L180"){
      eraseL180();
    }
    else if (type == "L270"){
      eraseL270();
    }
    
  }

  void control(int joy){
      //we need to move this when y = 16
        int side = isOutside();
        if(joy == 2 && (side != 2) && hitSide() != 2 && !hit()){
        delay(10);
        erase();
        y = y - 2;
        draw();
        }
        //when y = 0
        else if (joy == 0 && (side != 1) && hitSide() != 1 && !hit()){
        delay(10);
        erase();
        y = y + 2;
        draw();
        }
  }

  void move(int pin){
      if(!hitBottom() && !hit()){
      erase();
      x++;
      draw();
      delay(100);
      }
      else
      {
      isBottom = true;
      dead = true;
      draw();
      }
  }

  void initalize()
  {
    //center of top
    randomize();
    x = 2;
    y = 8;
    isBottom = false;
    dead = false;
    //testing only
    // replaced type = "" with randomize func

    //type = "cube";
    r = 0;
  }
  int tempR = 0;
  void randomize(){
    // something wrong with the way we are receiving random nums 
    delay (1000);
    tempR = ij;
    if (tempR == 1){
      type = "cube";
    }
    else if (tempR == 2){
      type = "L";
    }
    else if (tempR == 3){
      type = "I";
    }
    
    else if (tempR == 4){
      type = "L90";
    }
    else if (tempR == 5){
      type = "L180";
    }
    else if (tempR == 6){
      type = "L270";
    }
    else if (tempR == 7){
      type = "I";
    }
    else{
      type = "cube";
    }
    ij++;
    if(ij > 8){
      ij = 0;
    }
  }

  bool hitBottom(){
    // for up and down
    for(int i = 0; i < 16; i++){
      if(tetris[31][i] == FILL){
        return true;
      }
    }
    return false;
  }


  int isOutside(){
    for(int i = 0; i < 32; i++){
      if(tetris[i][15] == FILL){
        return 1;
      }
      if(tetris[i][0] == FILL){
        return 2;
      }
    }
    return 0;
  }
  //used for any type of piece
  bool hit(){
    //checking whole array
    for(int i = 0; i < 31; i++){
      for(int j = 0; j < 15; j++){
        if(tetris[i][j] == FILL && (tetris[i + 1][j] == DEAD || tetris[i + 1][j] == 'R' || tetris[i + 1][j] == 'B' || tetris[i + 1][j] == 'Y')){
          return true;
        }
      }
    }
    return false;
  }

  int hitSide(){
    //checking whole array
    for(int i = 0; i < 31; i++){
      for(int j = 0; j < 15; j++){
        if (tetris[i][j] == FILL && (tetris[i][j + 1] == DEAD || tetris[i][j+1] == 'Y' || tetris[i][j+1] == 'B' || tetris[i][j+1] == 'R')){
          //right side
          return 1;
        }
        if(tetris[i][j] == FILL && (tetris[i][j - 1] == DEAD || tetris[i][j-1] == 'Y' || tetris[i][j-1] == 'B' || tetris[i][j-1] == 'R')){
          //left side
          return 2;
        }
      }
    }
    return 0;
  }

  void drawL(){
    //Orange
    drawSeg(BLUE, x, y, false);
    drawSeg(BLUE, x + 2, y, false);
    drawSeg(BLUE, x + 4, y, false);
    drawSeg(BLUE, x + 4, y - 2, false);
  }

  void drawL90(){
    //Orange
    drawSeg(BLUE, x, y, false);
    drawSeg(BLUE, x , y + 2, false);
    drawSeg(BLUE, x, y + 4, false);
    drawSeg(BLUE, x + 2, y + 4, false);
  }
  void eraseL90(){
    //Orange
    drawSeg(BLACK, x, y, true);
    drawSeg(BLACK, x , y + 2, true);
    drawSeg(BLACK, x, y + 4, true);
    drawSeg(BLACK, x + 2, y + 4, true);
  }
  void drawL180(){
    drawSeg(BLUE, x, y, false);
    drawSeg(BLUE, x , y - 2, false);
    drawSeg(BLUE, x + 2, y - 2, false);
    drawSeg(BLUE, x + 4, y - 2, false);
  }
  void eraseL180(){
    drawSeg(BLACK, x, y, true);
    drawSeg(BLACK, x , y - 2, true);
    drawSeg(BLACK, x + 2, y - 2, true);
    drawSeg(BLACK, x + 4, y - 2, true);
  }
  void drawL270(){
    drawSeg(BLUE, x, y, false);
    drawSeg(BLUE, x , y - 2, false);
    drawSeg(BLUE, x , y - 4, false);
    drawSeg(BLUE, x -2, y - 4, false);
  }
  void eraseL270(){
    drawSeg(BLACK, x, y, true);
    drawSeg(BLACK, x , y - 2, true);
    drawSeg(BLACK, x , y - 4, true);
    drawSeg(BLACK, x -2, y - 4, true);
  }

  void eraseL(){
    //Orange
    drawSeg(BLACK, x, y, true);
    drawSeg(BLACK, x + 2, y, true);
    drawSeg(BLACK, x + 4, y, true);
    drawSeg(BLACK, x + 4, y - 2, true);
  }

  void drawI(){
    //Orange
    drawSeg(RED, x, y, false);
    drawSeg(RED, x + 2, y, false);
    drawSeg(RED, x + 4, y, false);
    drawSeg(RED, x + 6, y, false);
  }

  void eraseI(){
    //Orange
    drawSeg(BLACK, x, y, true);
    drawSeg(BLACK, x + 2, y, true);
    drawSeg(BLACK, x + 4, y, true);
    drawSeg(BLACK, x + 6, y, true);
  }

  bool get_isBottom(){
    return isBottom;
  }

  int get_x(){
    return x;
  }
  int get_y(){
    return y;
  }
  void rotate(int pin){
    Serial.println(r);
    if(pin == 0){
    if(r == 0 && type == "L"){
      erase();
      r = r + 90;
      type = "L90";
    }
    else if(r == 90 && type == "L90"){
      erase();
      r = r + 90;
      type = "L180";
    }
    else if(r == 180 && type == "L180"){
      erase();
      r = r + 90;
      type = "L270";
    }
    else if(r == 270 && type == "L270"){
      erase();
      r = 0;
      type = "L";
    }
    }
  }
private:
  bool isBottom;
  int x;
  int y;
  bool dead;
  String type;
  int r = 0;
  void drawSeg(Color color, int x1, int y1, bool isBlack){
    
    if(dead){
    tetris[x1][y1] = color.colorToChar();
    tetris[x1 + 1][y1] = color.colorToChar();
    tetris[x1][y1 + 1] = color.colorToChar();
    tetris[x1 + 1][y1 + 1] = color.colorToChar();
    }
    else if(isBlack){
    matrix.drawPixel(x1, y1, color.to_333());
    tetris[x1][y1] = EMPTY;
    matrix.drawPixel(x1 + 1, y1, color.to_333());
    tetris[x1 + 1][y1] = EMPTY;
    matrix.drawPixel(x1, y1 + 1, color.to_333());
    tetris[x1][y1 + 1] = EMPTY;
    matrix.drawPixel(x1 + 1, y1 + 1, color.to_333());
    tetris[x1 + 1][y1 + 1] = EMPTY;  
    }
    else{
    //testing
    matrix.drawPixel(x1, y1, color.to_333());
    tetris[x1][y1] = FILL;
    matrix.drawPixel(x1 + 1, y1, color.to_333());
    tetris[x1 + 1][y1] = FILL;
    matrix.drawPixel(x1, y1 + 1, color.to_333());
    tetris[x1][y1 + 1] = FILL;
    matrix.drawPixel(x1 + 1, y1 + 1, color.to_333());
    tetris[x1 + 1][y1 + 1] = FILL;
    }
  } 
};
// a global variable that represents the game tetris
Blocks block;
class Game{
public:
  Game(){
    score = 0;
  }

  int checkRowCompletion(){
    bool rowComp = false;
    for(int i = 0; i < 32; i++){
      //this checks each pixel in the row
      for(int j = 0; j < 16; j++){
        if(tetris[i][j] == DEAD || tetris[i][j] == 'Y' || tetris[i][j] == 'B' || tetris[i][j] == 'R'){
          rowComp = true;
        }
        else{
          rowComp = false;
          break;
        }
      }
      if(rowComp){
        return i;
      }
      rowComp = false;
    }
  return -1;
  }

  void clearRow(int row){
    for(int i = row; i < row + 2; i++){
      for(int j = 0; j < 16; j++){
        matrix.drawPixel(i, j, WHITE.to_333());
        score = score + 1;
      }
    }
    delay(1000);
    for(int i = row; i < row + 2; i++){
      for(int j = 0; j < 16; j++){
        matrix.drawPixel(i, j, BLACK.to_333());
        tetris[i][j] = EMPTY;
      }
    }
    //move all evements in array down
    //redraw everything
    
    char temp;
    // from deleted row to top
    for(int i = row - 1; i >= 0; i--){
      for(int j = 0; j < 16; j++){
        tetris[i + 2][j] = tetris[i][j];
        
        /*temp = tetris[i][j];
        tetris[i][j] = EMPTY;
        tetris[i + 2][j] = temp;
        if(tetris[i][j] == DEAD){
          matrix.drawPixel(i, j, AQUA.to_333());
        }
        */
      }
    }
    for(int i = 0; i < 32; i++){
      for (int j = 0; j < 16; j++){
        if(tetris[i][j] == 'Y'){
          matrix.drawPixel(i, j, YELLOW.to_333());
        }
        else if(tetris[i][j] == 'B'){
          matrix.drawPixel(i, j, BLUE.to_333());
        }
        else if(tetris[i][j] == 'R'){
          matrix.drawPixel(i, j, RED.to_333());
        }
        if(tetris[i][j] == EMPTY){
          matrix.drawPixel(i, j, BLACK.to_333());
        }
      }
      
    }
    //FIXES TOP ROWS
    for(int i = 0; i < 2; i++){
      for(int j = 0; j < 16; j++){
        tetris[i][j] = EMPTY;
      }
    }
  }

  void game_over() {
  matrix.setCursor(0,0);
  matrix.fillScreen(BLACK.to_333());
  matrix.setTextColor(GREEN.to_333());
  matrix.print("Score:");
  matrix.print(score);
  delay(5000);
  matrix.fillScreen(BLACK.to_333());
  matrix.setTextColor(RED.to_333());
  matrix.fillScreen(BLACK.to_333());
  delay(1000000000000);
  }

  void printStart(){
  matrix.setCursor(0,0);
  matrix.setTextSize(1);
  matrix.print("T");
  matrix.setTextColor(BLUE.to_333());
  delay(1000);
  matrix.print("E");
  matrix.setTextColor(YELLOW.to_333());
  delay(1000);
  matrix.print("T");
  matrix.setTextColor(PURPLE.to_333());
  delay(1000);
  matrix.print("R");
  matrix.setTextColor(BLUE.to_333());
  delay(1000);
  matrix.print("I");
  matrix.setTextColor(RED.to_333());
  delay(1000);
  matrix.print("S");
  delay(5000);
  matrix.fillScreen(BLACK.to_333());
  }

  double timer = 0;
  //should add single piece into the grid
  //this should be called every update
  void update(int xVal, bool yVal, int swSTATE) {
    //block movement
    
    int clearRow1 = checkRowCompletion();
    if(clearRow1 != -1){
      clearRow(clearRow1);
    }
    if(block.get_isBottom()){
      block.initalize();
      block.move(swSTATE);
    }
    else{
      block.move(swSTATE);
      block.control(xVal);
    }
    //FIXME
    Serial.println(tetris[4][8]);
    if(tetris[4][8] != EMPTY){
      Serial.println(timer);
      timer = timer + 0.1;
      if(timer >= 0.5){
        game_over();
      }
      
    }
    else{
      timer = 0;
    }
  
  }
private:
  //32 row 16 col
  int score;
};

Game game;

// see https://www.arduino.cc/reference/en/language/structure/sketch/setup/
void setup() {
 Serial.begin(9600);
 matrix.begin();
 pinMode(BUTTON_PIN_NUMBER, INPUT);
 pinMode(10, INPUT_PULLUP);
 for(int i = 0; i < 16; i++){
   for(int j = 0; j < 32; j++){
     tetris[i][j] = EMPTY;
   }
 }
 game.printStart();
}

// see https://www.arduino.cc/reference/en/language/structure/sketch/loop/
void loop() {
  int xValue = analogRead(VRX_PIN);
  int yValue = analogRead(VRY_PIN);
  int swSTATE = digitalRead(10);
  xValue = xValue / 500;

  game.update(xValue, yValue, swSTATE);
}
