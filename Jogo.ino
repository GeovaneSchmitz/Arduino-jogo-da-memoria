
#define LED_0 13
#define LED_1 10
#define LED_2 9
#define LED_3 8
#define BUTTON_0 A0
#define BUTTON_1 A1
#define BUTTON_2 A2
#define BUTTON_3 A3
#define BUTTON_RESET A4
#define OPTIONS_COUNT 4
#define LIMIT 500

#include <LiquidCrystal.h>

const int buttons[OPTIONS_COUNT] = {BUTTON_0, BUTTON_1, BUTTON_2, BUTTON_3};
const int leds[OPTIONS_COUNT] = {LED_0, LED_1, LED_2, LED_3};

short order[LIMIT] = {0};
int position = 0;

int max_points = 0;

int is_record = 0;

short difficulty = 0;

/*
    LiquidCrystal Library - Hello World

  Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
  library works with all LCD displays that are compatible with the
  Hitachi HD44780 driver. There are many of them out there, and you
  can usually tell them by the 16-pin interface.

  This sketch prints "Hello World!" to the LCD
  and shows the time.

    The circuit:
  * LCD RS pin to digital pin 12
  * LCD Enable pin to digital pin 11
  * LCD D4 pin to digital pin 5
  * LCD D5 pin to digital pin 4
  * LCD D6 pin to digital pin 3
  * LCD D7 pin to digital pin 2
  * LCD R/W pin to ground
  * LCD VSS pin to ground
  * LCD VCC pin to 5V
  * 10K resistor:
  * ends to +5V and ground
  * wiper to LCD VO pin (pin 3)

  Library originally added 18 Apr 2008
  by David A. Mellis
  library modified 5 Jul 2009
  by Limor Fried (http://www.ladyada.net)
  example added 9 Jul 2009
  by Tom Igoe
  modified 22 Nov 2010
  by Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/LiquidCrystal
  */

// include the library code:

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void select_difficulty();

void reset()
{
  is_record = 0;
  position = 0;
  for (int i = 0; i < OPTIONS_COUNT; i++)
    digitalWrite(leds[i], HIGH);

  delay(500);

  for (int i = 0; i < OPTIONS_COUNT; i++)
    digitalWrite(leds[i], LOW);
  select_difficulty();
}

void show_sequece()
{
  int interval = 2000 / (difficulty + 1);
  for (int i = 0; i < position; i++)
  {
    int pin = leds[order[i]];
    digitalWrite(pin, HIGH);
    delay(interval);
    digitalWrite(pin, LOW);
  }
}

void add_sequece()
{
  if (position < LIMIT)
  {
    order[position] = (short)random(0, OPTIONS_COUNT);

    Serial.println(order[position]);
    while (position > 0 && order[position] == order[position - 1])
    {
      order[position] = (short)random(0, OPTIONS_COUNT);
      Serial.println(order[position]);
    }

    position++;
  }
  else
  {
  }
}

void check_if_win()
{
  if (position >= LIMIT)
  {
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("Parabens");
    lcd.setCursor(2, 0);
    lcd.print("Voce ganhou!");
    delay(2000);
    reset();
  }
}

void read_buttons()
{

  int i = 0;
  while (i < position)
  {
    if (digitalRead(BUTTON_RESET))
    {
      reset();
    }
    int correct_button = order[i];
    int is_lost = 0;
    for (int j = 0; j < OPTIONS_COUNT; j++)
    {
      if (digitalRead(buttons[j]))
      {
        if (correct_button == j)
        {
          i += 1;

          digitalWrite(leds[j], HIGH);
          while (digitalRead(buttons[j]))
            ;
          digitalWrite(leds[j], LOW);
        }
        else
        {
          is_lost = 1;
          break;
        }
      }
    }
    if (is_lost)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Tente novamente!");
      lcd.setCursor(7, 1);
      lcd.print(":(");
      delay(2000);
      reset();
      break;
    }
  }
}

void show_init_message()
{
  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.print("Jogo da memoria");
  delay(2000);
}

void select_difficulty()
{
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Dificuldade:");
  lcd.setCursor(3, 1);
  lcd.print("1  2  3  4");
  lcd.setCursor(3 + difficulty * 3, 1);
  lcd.blink();
  while (!digitalRead(BUTTON_RESET))
  {
    for (int i = 0; i < OPTIONS_COUNT; i++)
    {
      if (digitalRead(buttons[i]))
      {
        lcd.setCursor(3 + i * 3, 1);
        difficulty = i;
      }
    }
  }
  lcd.noBlink();
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Bom jogo!");
}

void setup()
{
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.setCursor(0, 1);
  pinMode(LED_0, OUTPUT);
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);

  pinMode(BUTTON_0, INPUT);
  pinMode(BUTTON_1, INPUT);
  pinMode(BUTTON_2, INPUT);
  pinMode(BUTTON_3, INPUT);
  pinMode(BUTTON_RESET, INPUT);

  pinMode(3, INPUT);
  show_init_message();
  reset();
}

void update_display()
{

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Pontos: ");
  lcd.print(position);
  if (position > max_points)
  {
    max_points = position;
    if (!is_record)
    {
      is_record = 1;
      lcd.noDisplay();
      delay(100);
      lcd.display();
      delay(100);
      lcd.noDisplay();
      delay(100);
      lcd.display();
    }
  }
  lcd.setCursor(0, 1);
  lcd.print("Recorde: ");
  lcd.print(max_points);
}

void loop()
{
  update_display();
  check_if_win();
  add_sequece();
  show_sequece();
  delay(500);
  read_buttons();
}
