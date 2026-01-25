// world.h


#ifndef WORLD_H
#define WORLD_H


#include <sys/time.h>

#include "headers.h"
#include "main.h"
#include "seq.h"
#include "centipede.h"
#include "mushroom.h"
#include "player.h"
#include "dart.h"
#include "worldDefs.h"


class World {

  GLFWwindow *window;

  int  score;
  int  numCols;

  bool playerDied;
  bool goToNextLevel;
  
  bool    pauseForMessage;
  timeval pauseUntil;

  seq<Centipede*> centipedes;
  seq<Mushroom*>  mushrooms;
  Player          *player;
  seq<Dart*>      darts;

  Mushroom *highlightMushroom;  // mushroom to highlight (for debugging)

 public:

  int  level;
  int  livesRemaining;
  bool gameOver;
  
  float l, r, b, t;  // coordinates of window edges

  World( GLFWwindow *w ) {
    initWorld( w );
  }

  void initWorld( GLFWwindow *w );
  
  void initLevel() {

    // Start with one centipede.  With each new level, make the
    // centipede one segment shorter AND create a length-one centipede

    centipedes.clear();
    
    centipedes.add( new Centipede( MAX_CENTIPEDE_SEGMENTS - level, INIT_CENTIPEDE_POS, INIT_CENTIPEDE_DIR ) );
    for (int i=0; i<level; i++) // might at centipedes on top of each other ... would be easy to fix.
      centipedes.add( new Centipede( 1, 
				     vec2( WORLD_LEFT_EDGE + (randIn01()*(numCols-1)+0.5)*COL_SPACING, INIT_CENTIPEDE_POS.y ),
				     vec2( randIn01() > 0.5 ? 1 : -1, INIT_CENTIPEDE_DIR.y ) ) );
    // One player

    if (!player)
      player = new Player( INIT_PLAYER_POS );
    else
      player->pos = INIT_PLAYER_POS;

    playerDied      = false;
    goToNextLevel   = false;
    pauseForMessage = false;

    // Start level with no darts, fleas, spiders

    darts.clear();

    // Set up the window
    
    setWindowEdgeCoordinates();
  }

  
  void playerMove( vec2 pos ) {

    int winX, winY;
    glfwGetWindowSize( window, &winX, &winY );

    // [YOUR CODE HERE]
    //
    // Convert the mouse position 'pos' into a world position.  Use
    // the window size 'winX' and 'winY' and use the world coordinates
    // of the window edges (defined above).

    float worldX = 0.02;
    float worldY = -0.9;

    player->moveTo( vec2( worldX, worldY ) );
  }

  void playerFire() {

    if (darts.size() < MAX_DARTS_AT_ONCE)
      darts.add( new Dart( player->pos ) );
  }

  void draw();
  void updateState( float elapsedTime );
  void setWindowEdgeCoordinates();
  Mushroom *findClosestMushroomAhead( vec2 pos, vec2 dir, float maxPerpDist );
  int lowerMushroomCount();
};


#endif
