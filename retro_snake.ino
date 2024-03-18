/*

Author: Nishant Soni
Date: 2023

*/

// Including the LedControl library
#include <LedControl.h>

// Pins for the LED matrix
int DIN = 12;
int CS = 11;
int CLK = 10;

// Initializing the LedControl object
LedControl lc = LedControl(DIN, CLK, CS, 0);

// Constants for button pins
const byte up = 7;
const byte dn = 6;
const byte rt = 5;
const byte lt = 4;

// Array to represent the snake
int snake[64] = { 0 };
int snake_size;

// Direction of the snake (1-up, 2-down, 3-right, 4-left)
volatile int curr_dir;

//other variables/Flags for the game
int i, j, fi, fj, is_eaten;

//These hexadecimal values represent the pixel patterns for each frame of the snake animation
byte snake_ani[17][8] = {
  { 0x24, 0x18, 0x38, 0x44, 0x44, 0x28, 0x38, 0x38 },
  { 0x18, 0x38, 0x44, 0x44, 0x28, 0x38, 0x38, 0x38 },
  { 0x38, 0x44, 0x44, 0x28, 0x38, 0x38, 0x38, 0x38 },
  { 0x44, 0x44, 0x28, 0x38, 0x38, 0x38, 0x38, 0x38 },
  { 0x44, 0x28, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38 },
  { 0x28, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38 },
  { 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38 },
  { 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x10 },
  { 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x10, 0x10 },
  { 0x38, 0x38, 0x38, 0x38, 0x38, 0x10, 0x10, 0x00 },
  { 0x38, 0x38, 0x38, 0x38, 0x10, 0x10, 0x00, 0x00 },
  { 0x38, 0x38, 0x38, 0x10, 0x10, 0x00, 0x00, 0x00 },
  { 0x38, 0x38, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00 },
  { 0x38, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00 },
  { 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
  { 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

};


// Function to initialize the game
void initt() {
  for (int i = 0; i < 17; i++) {
    printByte(snake_ani[i]);
    delay(100);
    lc.clearDisplay(0);
  }

  delay(500);

  //Setting initial values for the snake
  curr_dir = 3;    // Initial direction is right
  is_eaten = 1;    // Flag to check if food is eaten
  snake_size = 2;  // Initial size of the snake

  //These lines initialize the starting position of the snake and its head

  /*
  snake[0] = 2; and snake[1] = 3;
  These lines set the initial position of the snake's head on the LED matrix.
  The coordinates are represented by array indices, where snake[0] represents the x-coordinate and snake[1] 
  represents the y-coordinate. So, the snake's head is initially positioned at (2, 3) on the LED matrix.
  */
  snake[0] = 2;
  snake[1] = 3;


  /*
  snake[2] = 2; and snake[3] = 2;
  These lines set the initial position of the second segment of the snake's body. The snake's body is 
  represented by segments, and its initial length is 1 segment. These lines set the coordinates of the 
  first segment of the snake's body, which in this case, is positioned right behind the head.
  */
  snake[2] = 2;
  snake[3] = 2;

  //current position of the snake's head
  i = snake[0];
  j = snake[1];
}


// Function to update snake's head position
void changeHead() {
  // Wrapping snake around the screen if it goes out of bounds
  if ((j >= 8) && (curr_dir == 3)) {
    j = 0;
  }
  if ((j <= 1) && (curr_dir == 4)) {
    j = 9;
  }

  if ((i >= 8) && curr_dir == 2) {
    i = 0;
  }
  if ((i <= 1) && (curr_dir == 1)) {
    i = 9;
  }

  // Moving snake's head based on the current direction
  if (curr_dir == 1) {
    i--;
  } else if (curr_dir == 2) {
    i++;
  } else if (curr_dir == 3) {
    j++;
  } else if (curr_dir == 4) {
    j--;
  }
  // Updating snake's head position
  snake[0] = i;
  snake[1] = j;
  
  // Checking if the snake has collided with itself
  for (int a = 2; a < (snake_size * 2); a += 2) {

    // Resetting the game if the snake collides with itself
    if ((i == snake[a]) && (j == snake[a + 1])) {
      lc.setLed(0, (fi - 1), (fj - 1), false);
      initt();
    }

  }


  // Checking if the snake has eaten the food
  if ((i == fi) && (j == fj)) {

    // If food is eaten, increasing snake's size and updating its position
    lc.setLed(0, (fi - 1), (fj - 1), false);
    ++snake_size;
    snake[((snake_size * 2) - 2)] = snake[((snake_size * 2) - 4)];
    snake[((snake_size * 2) - 1)] = snake[((snake_size * 2) - 3)];
    is_eaten = 1;

  }
}


// Function to draw the game board
void drawBoard() {

  // Drawing the snake
  for (int a = 0; a < (snake_size * 2); a += 2) {
    lc.setLed(0, (snake[a] - 1), (snake[(a + 1)] - 1), true);
  }

  // Generating food
  if (is_eaten) {
    // Randomly generating food position
    fi = random(1, 9);
    fj = random(1, 9);

    // Checking if food is generated on the snake's body, and re-generating if so
    for (int a = 0; a < (snake_size * 2); a += 2) {
      while ((snake[a]) == fi) {
        fi = random(1, 9);
      }

      while ((snake[(a + 1)]) == fj) {
        fj = random(1, 9);
      }
    }

    // Displaying the food
    lc.setLed(0, (fi - 1), (fj - 1), true);
    is_eaten = 0;
  }

  // Delay for animation
  delay(200);

  // Erasing the snake's trail
  for (int a = 0; a < (snake_size * 2); a += 2) {
    lc.setLed(0, (snake[a] - 1), (snake[(a + 1)] - 1), false);
  }

  // Updating snake's body segments
  for (int a = (snake_size * 2) - 1; a >= 0; a -= 2) {
    snake[(a + 2)] = snake[a];
    snake[(a + 1)] = snake[(a - 1)];
  }

  // Moving the snake's head
  changeHead();
}

// Setup function
void setup() {
  // Initializing the LED matrix
  lc.shutdown(0, false);
  lc.setIntensity(0, .2);
  lc.clearDisplay(0);

  // Configuring button pins
  pinMode(up, INPUT_PULLUP);
  pinMode(dn, INPUT_PULLUP);
  pinMode(rt, INPUT_PULLUP);
  pinMode(lt, INPUT_PULLUP);

  // Initializing serial communication
  Serial.begin(9600);

  // enable PCIE3 Bit 3 (port D)
  // enable pin change interrupts for specific groups of pins.
  PCICR |= B00000100;

  /*
  B11110000 enables pin change interrupts for the four most significant bits (D7 to D4) 
  of port D, which corresponds to Arduino pins 7 to 4.
  */
  PCMSK2 |= B11110000;

  // Initializing the game
  initt();

  // Initializing random seed
  randomSeed(analogRead(0));
}

// Main loop
void loop() {
  // Drawing the game board
  drawBoard();
}

// Interrupt service routine for button presses
ISR(PCINT2_vect) {
  if ((digitalRead(up) == LOW) && (curr_dir != 2)) {
    curr_dir = 1; // Change direction to up
  }
  if ((digitalRead(dn) == LOW) && (curr_dir != 1)) {
    curr_dir = 2; // Change direction to down
  }
  if ((digitalRead(rt) == LOW) && (curr_dir != 4)) {
    curr_dir = 3; // Change direction to right
  }
  if ((digitalRead(lt) == LOW) && (curr_dir != 3)) {
    curr_dir = 4; // Change direction to left
  }
}

// Function to print a byte on the LED matrix
void printByte(byte character[]) {
  int i = 0;
  for (i = 0; i < 8; i++) {
    lc.setRow(0, i, character[i]);
  }
}
