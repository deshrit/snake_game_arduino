#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define BLOCK_SIZE 4
#define rows 32 // SCREEN_WIDTH / BLOCK_SIZE
#define cols 16 // SCREEN_HEIGHT / BLOCK_SIZE

#define MAX_SNAKE_SIZE 50 // Assuming only of size 50 - low memory
#define SNAKE_SPEED 10
#define BUZZER_FREQ 300
#define BUZZER_DUR 50


// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


// pins
const int VRX = A0;
const int VRY = A1;
const uint8_t SW = 11, SW_RED = 12, BUZZER = 3;
// pin values
uint8_t sw_val = 1, sw_red_val = 1;
int vrx_val, vry_val;


// snake head
uint8_t snake_x = BLOCK_SIZE * 12;
uint8_t snake_y = BLOCK_SIZE * 6;
// snake body
uint8_t snake_body[MAX_SNAKE_SIZE][2];
uint8_t snake_length = 1;
// snake moving directions
struct{
  const uint8_t LEFT = 0, UP= 1, RIGHT = 2, DOWN = 3;
} Direction;
// current snake direction
uint8_t dir = Direction.RIGHT;


// food
uint8_t food_x = BLOCK_SIZE * 10, food_y = BLOCK_SIZE * 10;
int8_t velocity_x = 0, velocity_y= 0;


// game over
bool game_over = false;


// time
unsigned long diff, current_time, prev_time = millis();


// temporary variable
int tmp;


void setup() {
  Serial.begin(115200);
  pinMode(SW, INPUT_PULLUP);
  pinMode(SW_RED, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  // wellcome screen
  while(true) {
    wellcome_screen();
    read_all_inputs();
    if(sw_val == 0 || sw_red_val == 0) break;
  }
}


void loop() {

  // consistent frames and snake speed
  current_time = millis();
  diff = current_time - prev_time;
  prev_time = current_time;
  tmp = int(1000/SNAKE_SPEED);
  if(diff < tmp) {
    delay(tmp - diff);
  }

  // game over logic and restarting
  if(game_over) {
    while(true) {
      gameover_screen();
      read_all_inputs();
      // restart with button press
      if(sw_val == 0 || sw_red_val == 0) {
        game_over = false;
        snake_length = 1;
        snake_x = BLOCK_SIZE * 12;
        snake_y = BLOCK_SIZE * 6;
        dir = Direction.RIGHT;
        velocity_x = 1;
        velocity_y = 0;
        new_food_position();
        break;
      }
    }
  }
  
  // inputs
  read_all_inputs();
  dir = get_direction();
  
  // update
  update();
  
  // render
  render();
}


// read inputs of all pins
void read_all_inputs() {
  sw_val = digitalRead(SW);
  sw_red_val = digitalRead(SW_RED);
  vrx_val = analogRead(VRX);
  vry_val = analogRead(VRY);
}


void update() {

  // snake head move
  snake_x += velocity_x * BLOCK_SIZE;
  snake_y += velocity_y * BLOCK_SIZE;

  // snake eats food
  if(snake_x == food_x && snake_y == food_y) {
    snake_body[snake_length][0] = food_x;
    snake_body[snake_length][1] = food_y;
    new_food_position();
    snake_length++;
    // play buzzer
    tone(BUZZER, BUZZER_FREQ, BUZZER_DUR);
  }

  // change direction
  if(dir == Direction.UP&& velocity_y != 1) {
    velocity_x = 0;
    velocity_y = -1;
  } else if(dir == Direction.DOWN && velocity_y != -1) {
    velocity_x = 0;
    velocity_y = 1;
  } else if(dir == Direction.LEFT && velocity_x != 1) {
    velocity_x = -1;
    velocity_y = 0;
  } else if(dir == Direction.RIGHT && velocity_x != -1) {
    velocity_x = 1;
    velocity_y = 0;
  }

  // snake body move
  for(int8_t i = snake_length - 1; i > 0; i--) {
    snake_body[i][0] = snake_body[i-1][0];
    snake_body[i][1] = snake_body[i-1][1];
  }
  snake_body[0][0] = snake_x;
  snake_body[0][1] = snake_y;


  // game over condition
  if(snake_x >= rows*BLOCK_SIZE || snake_x <= 0 || snake_y >= cols*BLOCK_SIZE || snake_y <= 0) {
    game_over = true;
  }
  for(uint8_t i=0; i < snake_length; i++) {
    if(i == 0) continue;
    if(snake_body[i][0] == snake_x && snake_body[i][1] == snake_y) game_over = true;
  }
}


void render() {
  display.clearDisplay();
  // border
  display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
  // food
  display.fillRect(food_x, food_y, BLOCK_SIZE, BLOCK_SIZE, WHITE);
  // snake
  draw_snake();
  display.display();
}


void draw_snake() {
  for(uint8_t i = 0; i < snake_length; i++) {
    display.fillRect(snake_body[i][0], snake_body[i][1], BLOCK_SIZE, BLOCK_SIZE, WHITE);
  }
}


void new_food_position() {
  food_x = random(0, rows) * BLOCK_SIZE;
  food_y = random(0, cols) * BLOCK_SIZE;
  for(uint8_t i=0; i < snake_length; i++) {
    if(snake_body[i][0] == food_x && snake_body[i][1] == food_y) new_food_position();
  }
}


// current direction input from joystick
uint8_t get_direction() {
  if(vry_val < 400 && vrx_val > 400) return Direction.LEFT;
  if(vrx_val > 700 && vry_val < 700) return Direction.UP;
  if(vry_val > 700 && vrx_val < 700) return Direction.RIGHT;
  if(vrx_val < 400 && vry_val > 400) return Direction.DOWN;
  return dir;
}


void wellcome_screen() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(5, 20);
  display.print("SNAKE GAME");
  
  display.setCursor(10, 40);
  display.setTextSize(1);
  display.print("(press any button)");
  display.display();
}


void gameover_screen() {
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(18, 20);
  display.print("!! GAME OVER !!");

  display.setTextSize(1);
  display.setCursor(40, 38);
  Serial.print(snake_length);
  display.print("score: " + String(snake_length));
  display.display();
}
