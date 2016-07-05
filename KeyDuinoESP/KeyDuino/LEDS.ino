/********** LED functions **********/

void Color_ALL(int RED, int GREEN, int BLUE)
{
  for (int j  = 0; j < NUM_LEDS; j++) leds[j] = CRGB(RED, GREEN, BLUE);
  FastLED.show();
}


void  Color_wait(int Red, int Green, int Blue)
{
  if (i >= NUM_LEDS) i = 0;
  Color_ALL(0, 0, 0);
  leds[i] += CRGB(Red, Green, Blue);
  FastLED.show();
  i++;
}

void sinelon(int Red, int Green, int Blue)
{
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(75, 0, NUM_LEDS);
  leds[pos] += CRGB(Red, Green, Blue);
  FastLED.show();
}
