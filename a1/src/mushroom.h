// mushroom.h

#include "headers.h"
#include "drawbuffer.h"

#define MUSH_BODY_COLOUR vec3(1.000, 0.129, 0.741)
#define MUSH_OUTLINE_COLOUR vec3(0.031, 0.851, 0.776)

class Mushroom
{

  // VAOs for drawing ("static", so these are shared by all Mushroom instances)

  // One shared DrawBuffers for the mushroom (cap + stem + outlines)
  static DrawBuffers *db;

  // One shared DrawBuffers for the black damage mask (a rectangle)
  static DrawBuffers *maskDb;

  // Local-space values used to anchor/scale the mask from below
  static float baseY;        // bottom of mushroom in local space
  static float totalHeight;  // height to top of mushroom in local space

public:
  vec2 pos;
  int damage;

  Mushroom(vec2 _pos)
  {

    pos = _pos;
    damage = 0;

    if (db == NULL)
      generateVAOs();
  }

  void generateVAOs();
  void draw(mat4 &worldToViewTransform);
};
