// spider.h
#pragma once

#include "headers.h"
#include "drawbuffer.h"

class Spider {
  static DrawBuffers *db; // shared geometry

public:
  vec2  pos;
  vec2  vel;
  bool  alive;

  // timer for changing movement pattern
  float changeTimer;

  Spider(vec2 startPos, vec2 startVel);

  static void generateVAOs();

  // Update position + behavior
  void update(float elapsedTime);

  // Draw at pos
  void draw(mat4 &worldToViewTransform);

  // Collision radius
  float radius() const;
};
