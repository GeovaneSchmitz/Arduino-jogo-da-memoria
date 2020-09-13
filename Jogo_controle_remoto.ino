#include <IRremote.h>

#define LED_0 44
#define LED_1 42
#define LED_2 40
#define LED_3 46
#define BUTTON_1 0xFD00FF
#define BUTTON_2 0xFD807F
#define BUTTON_3 0xFD40BF
#define BUTTON_4 0xFD20DF

#define BUTTON_OK 0xFD827D
#define BUTTON_LEFT 0xFD02FD
#define BUTTON_RIGHT 0xFD42BD
#define BUTTON_RIGHT 0xFD42BD
#define BUTTON_RESET 0xFDC03F
#define OPTIONS_COUNT 4
#define LIMIT 500

#define RECV_PIN 38

IRrecv irrecv(RECV_PIN);
decode_results results;

const unsigned long buttons[OPTIONS_COUNT] = {BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4};
const int leds[OPTIONS_COUNT] = {LED_0, LED_1, LED_2, LED_3};

short order[LIMIT] = {0};
int position = 0;

int max_points = 0;

int is_record = 0;

unsigned long key_value = 0;

short difficulty = 0;

void select_difficulty();

void reset()
{
  is_record = 0;
  position = 0;
  for (int i = 0; i < OPTIONS_COUNT; i++)
    digitalWrite(leds[i], LOW);
  delay(10);
  for (int i = 0; i < OPTIONS_COUNT; i++)
  {
    digitalWrite(leds[i], HIGH);
    delay(100);
  }

  for (int i = 0; i < OPTIONS_COUNT; i++)
  {
    digitalWrite(leds[i], LOW);
    delay(100);
  }
  select_difficulty();
}

void show_sequece()
{
  int interval = 1000 / (difficulty + 1);
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

    while (position > 0 && order[position] == order[position - 1])
    {
      order[position] = (short)random(0, OPTIONS_COUNT);
    }

    position++;
  }
}

void check_if_win()
{
  if (position >= LIMIT)
  {
    for (int i = 0; i < OPTIONS_COUNT; i++)
    {
      digitalWrite(leds[i], HIGH);
      delay(100);
    }
    for (int i = 0; i < OPTIONS_COUNT; i++)
    {
      digitalWrite(leds[i], LOW);
      delay(100);
    }
    delay(2000);
    reset();
  }
}

void read_sequence()
{
  int i = 0;
  int button_index_pressed = 0;
  key_value = 0;
  while (i < position)
  {
    if (irrecv.decode(&results))
    {
      if (results.value != 0XFFFFFFFF)
        key_value = results.value;
      irrecv.resume();

      if (key_value == BUTTON_RESET)
        reset();

      if (i > 0 && key_value == buttons[order[i - 1]])
        continue;

      int is_option_button = 0;
      for (button_index_pressed = 0; button_index_pressed < OPTIONS_COUNT; button_index_pressed++)
      {
        if (key_value == buttons[button_index_pressed])
        {
          is_option_button = 1;
          break;
        }
      }
      if (!is_option_button)
        continue;

      if (i > 0)
        digitalWrite(leds[order[i - 1]], LOW);

      int correct_button_index = order[i];
      if (correct_button_index == button_index_pressed)
      {
        digitalWrite(leds[button_index_pressed], HIGH);
        i++;
      }
      else
      {
        reset();
        break;
      }
    }
  }
  delay(500);
  if (i > 0)
    digitalWrite(leds[order[i - 1]], LOW);
}

void select_difficulty()
{
  for (int i = 0; i <= difficulty; i++)
    digitalWrite(leds[i], HIGH);
  while (true)
  {
    if (irrecv.decode(&results))
    {
      if (results.value != 0XFFFFFFFF)
        key_value = results.value;

      switch (key_value)
      {
      case BUTTON_LEFT:
        if (difficulty > 0)
        {
          digitalWrite(leds[difficulty], LOW);
          difficulty--;
        }
        break;
      case BUTTON_RIGHT:
        if (difficulty < OPTIONS_COUNT)
        {
          difficulty++;
          digitalWrite(leds[difficulty], HIGH);
        }
        break;
      }
      if (key_value == BUTTON_OK)
        break;
      irrecv.resume();
    }
  }
  irrecv.resume();
  for (int i = 0; i <= difficulty; i++)
    digitalWrite(leds[i], LOW);
  delay(2000);
}

void setup()
{
  irrecv.enableIRIn();
  irrecv.blink13(true);
  pinMode(LED_0, OUTPUT);
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  reset();
}

void loop()
{

  check_if_win();
  add_sequece();
  show_sequece();
  read_sequence();
}
