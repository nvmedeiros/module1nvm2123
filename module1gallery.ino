/**************************************************************************
 nvm2123
 Module 1 'Transitions': a yellow smiley face (with rainbow confetti) turns
 into a vibrant pink straight face (plain), which turns into a blue sad
 face (with light blue mini teardrop dots). Loops continuously!
 **************************************************************************/
#include <TFT_eSPI.h>
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();

uint16_t hotPink = tft.color565(255, 20, 147);    // custom vibrant pink
uint16_t lightBlue = tft.color565(173, 216, 230); // custom light blue

// Rainbow palette for confetti
uint16_t rainbowColors[] = {
  TFT_RED, TFT_ORANGE, TFT_YELLOW, TFT_GREEN, TFT_CYAN, TFT_BLUE, TFT_MAGENTA
};
const int numRainbowColors = 7;

enum Expression { SMILE, STRAIGHT, SAD };

void setup() {
  tft.init();
  tft.setRotation(1); // 1 = landscape, 2 = portrait
  randomSeed(analogRead(0)); // configuration of confetti changes randomly for every loop
}

void loop() {
  drawFace(SMILE, TFT_YELLOW);
  delay(1500);

  drawFace(STRAIGHT, hotPink);
  delay(1500);

  drawFace(SAD, TFT_BLUE);
  delay(1500);
}

void drawFace(Expression exp, uint16_t color) {
  tft.fillScreen(TFT_BLACK);

  int16_t cx = tft.width() / 2;
  int16_t cy = tft.height() / 2;
  int16_t faceRadius = min(tft.width(), tft.height()) / 2 - 10;

  // Face outline
  tft.drawCircle(cx, cy, faceRadius, color);
  tft.drawCircle(cx, cy, faceRadius - 1, color); // thicken the outline slightly

  // Eyes
  int16_t eyeOffsetX = faceRadius / 2;
  int16_t eyeOffsetY = faceRadius / 3;
  int16_t eyeRadius = faceRadius / 10;

  tft.fillCircle(cx - eyeOffsetX, cy - eyeOffsetY, eyeRadius, color);
  tft.fillCircle(cx + eyeOffsetX, cy - eyeOffsetY, eyeRadius, color);

  // Mouth
  int16_t mouthWidth = faceRadius; // half-width on each side
  int16_t mouthY = cy + faceRadius / 3;

  switch (exp) {
    case SMILE:
      drawArc(cx, mouthY - faceRadius / 4, faceRadius / 2, 20, 160, color);
      drawConfetti(cx, cy, faceRadius);
      break;
    case STRAIGHT:
      tft.drawLine(cx - mouthWidth / 2, mouthY, cx + mouthWidth / 2, mouthY, color);
      tft.drawLine(cx - mouthWidth / 2, mouthY + 1, cx + mouthWidth / 2, mouthY + 1, color);
      // no extra decoration in background or on face for the plain expression
      break;
    case SAD:
      drawArc(cx, mouthY + faceRadius / 3, faceRadius / 2, 200, 340, color);
      drawTears(cx, cy, eyeOffsetX, eyeOffsetY, eyeRadius);
      break;
  }
}

// Scatters small rainbow-colored circles around the screen (outside the face)
void drawConfetti(int16_t cx, int16_t cy, int16_t faceRadius) {
  const int numConfetti = 25;
  const int dotRadius = 3;

  for (int i = 0; i < numConfetti; i++) {
    int16_t x = random(0, tft.width());
    int16_t y = random(0, tft.height());

    // skip dots that would land inside/overlap the face circle
    float dist = sqrt(sq((float)(x - cx)) + sq((float)(y - cy)));
    if (dist < faceRadius + dotRadius) continue;

    uint16_t color = rainbowColors[random(0, numRainbowColors)];
    tft.fillCircle(x, y, dotRadius, color);
  }
}

// Draws a couple of mini light blue dots under each eye, like falling tears
void drawTears(int16_t cx, int16_t cy, int16_t eyeOffsetX, int16_t eyeOffsetY, int16_t eyeRadius) {
  const int dotRadius = 2;
  const int dropsPerEye = 2;
  const int dropSpacing = eyeRadius * 2;

  int16_t leftEyeX = cx - eyeOffsetX;
  int16_t rightEyeX = cx + eyeOffsetX;
  int16_t eyeY = cy - eyeOffsetY;

  for (int i = 1; i <= dropsPerEye; i++) {
    int16_t dropY = eyeY + eyeRadius + (i * dropSpacing);
    tft.fillCircle(leftEyeX, dropY, dotRadius, lightBlue);
    tft.fillCircle(rightEyeX, dropY, dotRadius, lightBlue);
  }
}

// Draws an arc by connecting points along a circle from startDeg to endDeg (0 = right, 90 = down)
// this is for the smile and the frown
void drawArc(int16_t cx, int16_t cy, int16_t radius, int16_t startDeg, int16_t endDeg, uint16_t color) {
  int16_t prevX = cx + radius * cos(radians(startDeg));
  int16_t prevY = cy + radius * sin(radians(startDeg));

  for (int16_t deg = startDeg; deg <= endDeg; deg += 5) {
    int16_t x = cx + radius * cos(radians(deg));
    int16_t y = cy + radius * sin(radians(deg));
    tft.drawLine(prevX, prevY, x, y, color);
    prevX = x;
    prevY = y;
  }
}
