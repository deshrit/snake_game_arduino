# snake_game_arduino
Writing snake game on arduino nano with ssd1306 128x164 display

![snake_game](https://github.com/deshrit/snake_game_arduino/assets/59757711/94b58e72-2af4-4af4-900f-d769b4c529ae)

## video
[video](https://www.youtube.com/shorts/5A0HsLjOQTo)

## major drawbacks
- The system does not have battery uses power from USB
- `#define MAX_SNAKE_SIZE 50 // Assuming only of size 50 - low memory`

## requirements
- arduino nano clone **(used old bootloader)**
- ssd1306 OLED 0.96' display
- libraries
    - Adafruit_GFX.h ==  1.11.3
    - Adafruit_SSD1306.h == 2.5.7
