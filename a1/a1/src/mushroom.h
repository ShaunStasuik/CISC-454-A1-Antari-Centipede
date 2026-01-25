// mushroom.h


#include "headers.h"
#include "drawbuffer.h"


#define MUSH_BODY_COLOUR      vec3( 1.000, 0.129, 0.741 )
#define MUSH_OUTLINE_COLOUR   vec3( 0.031, 0.851, 0.776 )

class Mushroom {

  // VAOs for drawing ("static", so these are shared by all Mushroom instances)

  static GLuint VAO;
  
 public:

  vec2 pos;
  int  damage;

  Mushroom( vec2 _pos ) {

    pos    = _pos;
    damage = 0;

    if (VAO == 0)
      generateVAOs();
  }

  void generateVAOs();

  void draw( mat4 &worldToViewTransform );
};
