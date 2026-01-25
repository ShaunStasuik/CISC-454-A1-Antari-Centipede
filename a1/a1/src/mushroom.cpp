// mushroom.cpp


#include "mushroom.h"
#include "main.h"
#include "worldDefs.h"


extern GLFWwindow* window;
#define LINE_HALFWIDTH_IN_PIXELS 2.0


GLuint Mushroom::VAO = 0;

// Mushroom geometry

// [ YOUR CODE HERE ]


void Mushroom::generateVAOs()

{
  // [YOUR CODE HERE]
  //
  // REPLACE THE CODE in this function to generate VAOs that store
  // mushroom geometry.  You will need MULTIPLE VAOs.  You should look
  // at the Player and the Centipede to see how those multiple VAOs
  // are set up.  You can do something similar here, or take your own
  // approach.
  //
  // For full marks, you should use the parameters in worldDefs.h to
  // determine the SIZE of your geometry.
  //
  // Your geometry should be centred at (0,0) so that you can later
  // translate it to its correct position
  //
  // In Step 5 (after you get the mushroom rendered correctly), also
  // take into account the mushroom's 'damage' to draw a mushroom with
  // the appropriate amount of damage.

  // Start setting up a VAO
    
  glGenVertexArrays( 1, &VAO );
  glBindVertexArray( VAO );

  // Define the VBO
    
  GLuint VBO;
  glGenBuffers( 1, &VBO );
  glBindBuffer( GL_ARRAY_BUFFER, VBO );

  // Define the attributes

  glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0 ); // two floats for a position
  glEnableVertexAttribArray( 0 );

  glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*) (2*sizeof(float)) ); // three floats for a colour
  glEnableVertexAttribArray( 1 );

  // Stop setting up this VAO.

  glBindVertexArray( 0 );

  // Find line width 'lw' in world coordinate system
  
  int width, height;
  glfwGetFramebufferSize( window, &width, &height );
  float lw = LINE_HALFWIDTH_IN_PIXELS / (float) height * 2; // relies on top-bottom = 2 in WCS

  // ---------------- Set up the mushroom geometry ----------------
  //
  // This dummy code just builds a small square centred at (0,0).
  //
  // [ YOUR CODE HERE ]

  int numVerts  = 10;
  int numFloats = 5 * numVerts;  // five attributes for each vertex
  
  float *buffer = new float[ numFloats ];

  int i = 0;
  
  // Vertex 1
  
  buffer[i++] = -0.015 + lw;
  buffer[i++] = -0.015 + lw;

  buffer[i++] = MUSH_OUTLINE_COLOUR.x;
  buffer[i++] = MUSH_OUTLINE_COLOUR.y;
  buffer[i++] = MUSH_OUTLINE_COLOUR.z;

  buffer[i++] = -0.015 - lw;
  buffer[i++] = -0.015 - lw;

  buffer[i++] = MUSH_OUTLINE_COLOUR.x;
  buffer[i++] = MUSH_OUTLINE_COLOUR.y;
  buffer[i++] = MUSH_OUTLINE_COLOUR.z;

  // Vertex 2
  
  buffer[i++] = +0.015 - lw;
  buffer[i++] = -0.015 + lw;

  buffer[i++] = MUSH_OUTLINE_COLOUR.x;
  buffer[i++] = MUSH_OUTLINE_COLOUR.y;
  buffer[i++] = MUSH_OUTLINE_COLOUR.z;

  buffer[i++] = +0.015 + lw;
  buffer[i++] = -0.015 - lw;

  buffer[i++] = MUSH_OUTLINE_COLOUR.x;
  buffer[i++] = MUSH_OUTLINE_COLOUR.y;
  buffer[i++] = MUSH_OUTLINE_COLOUR.z;

  // Vertex 3
  
  buffer[i++] = +0.015 - lw;
  buffer[i++] = +0.015 - lw;

  buffer[i++] = MUSH_OUTLINE_COLOUR.x;
  buffer[i++] = MUSH_OUTLINE_COLOUR.y;
  buffer[i++] = MUSH_OUTLINE_COLOUR.z;

  buffer[i++] = +0.015 + lw;
  buffer[i++] = +0.015 + lw;

  buffer[i++] = MUSH_OUTLINE_COLOUR.x;
  buffer[i++] = MUSH_OUTLINE_COLOUR.y;
  buffer[i++] = MUSH_OUTLINE_COLOUR.z;

  // Vertex 4
  
  buffer[i++] = -0.015 + lw;
  buffer[i++] = +0.015 - lw;

  buffer[i++] = MUSH_OUTLINE_COLOUR.x;
  buffer[i++] = MUSH_OUTLINE_COLOUR.y;
  buffer[i++] = MUSH_OUTLINE_COLOUR.z;

  buffer[i++] = -0.015 - lw;
  buffer[i++] = +0.015 + lw;

  buffer[i++] = MUSH_OUTLINE_COLOUR.x;
  buffer[i++] = MUSH_OUTLINE_COLOUR.y;
  buffer[i++] = MUSH_OUTLINE_COLOUR.z;

  // and back to Vertex 1
  
  buffer[i++] = -0.015 + lw;
  buffer[i++] = -0.015 + lw;

  buffer[i++] = MUSH_OUTLINE_COLOUR.x;
  buffer[i++] = MUSH_OUTLINE_COLOUR.y;
  buffer[i++] = MUSH_OUTLINE_COLOUR.z;

  buffer[i++] = -0.015 - lw;
  buffer[i++] = -0.015 - lw;

  buffer[i++] = MUSH_OUTLINE_COLOUR.x;
  buffer[i++] = MUSH_OUTLINE_COLOUR.y;
  buffer[i++] = MUSH_OUTLINE_COLOUR.z;


  glBufferData( GL_ARRAY_BUFFER, numFloats*sizeof(float), buffer, GL_STATIC_DRAW );
      
  glBindBuffer( GL_ARRAY_BUFFER, 0 );
}



void Mushroom::draw( mat4 &worldToViewTransform )

{
  // [YOUR CODE HERE]
  //
  // REPLACE THE CODE in this function so that your mushroom is drawn
  // centred at its 'pos'.
  //
  // Later, in Step 5, also take into account the mushroom's 'damage'
  // to draw a mushroom with the appropriate amount of damage.
  
  // Provide MVP to GPU program

  mat4 T = translate( pos.x, pos.y, 0 );
  mat4 MVP = worldToViewTransform * T;

  gpuProg->setMat4( "MVP", MVP );

  // [ YOUR CODE HERE ]

  glBindVertexArray( VAO );

  glDrawArrays( GL_TRIANGLE_STRIP, 0, 10 );

  glBindVertexArray( 0 );
}



