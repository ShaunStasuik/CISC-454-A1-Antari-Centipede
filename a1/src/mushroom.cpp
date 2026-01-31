// mushroom.cpp

#include "mushroom.h"
#include "main.h"
#include "worldDefs.h"

extern GLFWwindow *window;
#define LINE_HALFWIDTH_IN_PIXELS 2.0

seq<DrawBuffers> Mushroom::mushroomStates;
// GLuint Mushroom::VAO = 0;

// Mushroom geometry

// [ YOUR CODE HERE ]

// Thick segment as a triangle strip.
static void addThickSegment(seq<vec2> &positions, seq<vec3> &colours,
                            vec2 a, vec2 b, float halfWidth, vec3 colour)
{
  vec2 d = (b - a).normalize();                 // direction along segment
  vec2 perp(-halfWidth * d.y, halfWidth * d.x); // perpendicular thickness vector

  GLuint offset = positions.size();

  positions.add(a + perp);
  positions.add(a - perp);
  positions.add(b + perp);
  positions.add(b - perp);

  for (int i = offset; i < positions.size(); i++)
    colours.add(colour);
}

// Filling the cap of the mushroom.
static void addSemiCircleFan(seq<vec2> &positions, seq<vec3> &colours,
                             vec2 centre, float radius, vec3 colour)
{
  GLuint offset = positions.size();

  // Triangle fan: center vertex first
  positions.add(centre);

  // Arc points from 0..pi (top half)
  for (float theta = 0.0f; theta <= (float)M_PI + 1e-6f; theta += (float)M_PI / (float)PIECES_PER_CIRCLE)
    positions.add(centre + vec2(radius * cos(theta), radius * sin(theta)));

  // Close fan
  positions.add(positions[offset + 1]);

  for (int i = offset; i < positions.size(); i++)
    colours.add(colour);
}

// Cap outline thick arc strip.
static void addThickArcStrip(seq<vec2> &positions, seq<vec3> &colours,
                             vec2 centre, float rInner, float rOuter, vec3 colour)
{
  GLuint offset = positions.size();

  for (float theta = 0.0f; theta <= (float)M_PI + 1e-6f; theta += (float)M_PI / (float)PIECES_PER_CIRCLE) {
    positions.add( centre + vec2(rInner * cos(theta), rInner * sin(theta)) );
    positions.add( centre + vec2(rOuter * cos(theta), rOuter * sin(theta)) );
  }

  for (int i = offset; i < positions.size(); i++)
    colours.add(colour);
}

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

  // Convert pixel line half-width to world coords (same as centipede)
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  float lw = LINE_HALFWIDTH_IN_PIXELS / (float)height * 2.0f;

  // Properly size the mushrrom by tying it to the grid spacing.
  float cell = COL_SPACING;      // 0.07
  float R0   = 0.33f * cell;     // overall size reference

  // Build VAO/VBO per damage state (0..MUSH_MAX_DAMAGE-1)
  for (int d = 0; d < MUSH_MAX_DAMAGE; d++)
  {
    // Damage fraction t in [0,1]
    float t = (MUSH_MAX_DAMAGE <= 1) ? 0.0f : (float)d / (float)(MUSH_MAX_DAMAGE - 1);

    // Shape change with damage (cap shrinks more)
    float capScale  = std::max(0.35f, 1.0f - 0.55f * t);
    float stemScale = std::max(0.60f, 1.0f - 0.25f * t);

    // Dimensions
    float R     = R0;
    float capR  = 0.95f * R * capScale;
    float stemW = 0.60f * R;
    float stemH = 0.85f * R * stemScale;

    // Local-space layout (stem under cap, centered at x=0)
    float baseY    = -R;
    float stemTopY = baseY + stemH;
    float capCY    = stemTopY + 0.20f * R;

    vec3 capCol  = MUSH_BODY_COLOUR;
    vec3 stemCol = vec3(0.92f, 0.88f, 0.75f);
    vec3 outCol  = MUSH_OUTLINE_COLOUR;


  // VAO and VBO setup.
  GLuint VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // Define the VBO

  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  // Define the attributes

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0); // two floats for a position
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(2 * sizeof(float))); // three floats for a colour
  glEnableVertexAttribArray(1);

  // Stop setting up this VAO.

  glBindVertexArray(0);

  // Find line width 'lw' in world coordinate system

  // int width, height;
  // glfwGetFramebufferSize(window, &width, &height);
  // float lw = LINE_HALFWIDTH_IN_PIXELS / (float)height * 2; // relies on top-bottom = 2 in WCS

  // ---------------- Set up the mushroom geometry ----------------
  //
  // This dummy code just builds a small square centred at (0,0).
  //
  // [ YOUR CODE HERE ]

    DrawBuffers db(VAO);

    // CPU-side vertex lists
    seq<vec2> positions;
    seq<vec3> colours;

    GLuint offset;

    // ---------- 1) Stem fill (rectangle triangle fan) ----------
    {
      float x0 = -0.5f * stemW;
      float x1 =  0.5f * stemW;
      float y0 =  baseY;
      float y1 =  stemTopY;

      offset = positions.size();

      positions.add( vec2(0.0f, 0.5f*(y0+y1)) ); // center
      positions.add( vec2(x0, y0) );
      positions.add( vec2(x1, y0) );
      positions.add( vec2(x1, y1) );
      positions.add( vec2(x0, y1) );
      positions.add( vec2(x0, y0) );             // close

      for (int i=offset; i<positions.size(); i++)
        colours.add(stemCol);

      db.mode.add(GL_TRIANGLE_FAN);
      db.first.add(offset);
      db.count.add(positions.size() - offset);
    }

    // ---------- 2) Cap fill (semi-circle fan) ----------
    {
      offset = positions.size();

      addSemiCircleFan(positions, colours, vec2(0.0f, capCY), capR, capCol);

      db.mode.add(GL_TRIANGLE_FAN);
      db.first.add(offset);
      db.count.add(positions.size() - offset);
    }

    // ---------- 3) Stem outline (4 thick edges) ----------
    {
      float x0 = -0.5f * stemW;
      float x1 =  0.5f * stemW;
      float y0 =  baseY;
      float y1 =  stemTopY;

      // bottom
      offset = positions.size();
      addThickSegment(positions, colours, vec2(x0,y0), vec2(x1,y0), lw, outCol);
      db.mode.add(GL_TRIANGLE_STRIP); db.first.add(offset); db.count.add(positions.size()-offset);

      // right
      offset = positions.size();
      addThickSegment(positions, colours, vec2(x1,y0), vec2(x1,y1), lw, outCol);
      db.mode.add(GL_TRIANGLE_STRIP); db.first.add(offset); db.count.add(positions.size()-offset);

      // top
      offset = positions.size();
      addThickSegment(positions, colours, vec2(x1,y1), vec2(x0,y1), lw, outCol);
      db.mode.add(GL_TRIANGLE_STRIP); db.first.add(offset); db.count.add(positions.size()-offset);

      // left
      offset = positions.size();
      addThickSegment(positions, colours, vec2(x0,y1), vec2(x0,y0), lw, outCol);
      db.mode.add(GL_TRIANGLE_STRIP); db.first.add(offset); db.count.add(positions.size()-offset);
    }

    // ---------- 4) Cap outline (thick arc strip) ----------
    {
      offset = positions.size();

      float rInner = std::max(0.0f, capR - lw);
      float rOuter = capR + lw;

      addThickArcStrip(positions, colours, vec2(0.0f, capCY), rInner, rOuter, outCol);

      db.mode.add(GL_TRIANGLE_STRIP);
      db.first.add(offset);
      db.count.add(positions.size() - offset);
    }

    // ---------- Upload interleaved (x y r g b) to VBO ----------
    int numFloats = positions.size() * (2 + 3);
    float *buffer = new float[numFloats];

    float *p = buffer;
    for (int i=0; i<positions.size(); i++) {
      *p++ = positions[i].x;
      *p++ = positions[i].y;
      *p++ = colours[i].x;
      *p++ = colours[i].y;
      *p++ = colours[i].z;
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, numFloats*sizeof(float), buffer, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    delete [] buffer;

    // Save this damage-state geometry
    mushroomStates.add(db);
  }

  // int numVerts = 10;
  // int numFloats = 5 * numVerts; // five attributes for each vertex

  // float *buffer = new float[numFloats];

  // int i = 0;

  // // Vertex 1

  // buffer[i++] = -0.015 + lw;
  // buffer[i++] = -0.015 + lw;

  // buffer[i++] = MUSH_OUTLINE_COLOUR.x;
  // buffer[i++] = MUSH_OUTLINE_COLOUR.y;
  // buffer[i++] = MUSH_OUTLINE_COLOUR.z;

  // buffer[i++] = -0.015 - lw;
  // buffer[i++] = -0.015 - lw;

  // buffer[i++] = MUSH_OUTLINE_COLOUR.x;
  // buffer[i++] = MUSH_OUTLINE_COLOUR.y;
  // buffer[i++] = MUSH_OUTLINE_COLOUR.z;

  // // Vertex 2

  // buffer[i++] = +0.015 - lw;
  // buffer[i++] = -0.015 + lw;

  // buffer[i++] = MUSH_OUTLINE_COLOUR.x;
  // buffer[i++] = MUSH_OUTLINE_COLOUR.y;
  // buffer[i++] = MUSH_OUTLINE_COLOUR.z;

  // buffer[i++] = +0.015 + lw;
  // buffer[i++] = -0.015 - lw;

  // buffer[i++] = MUSH_OUTLINE_COLOUR.x;
  // buffer[i++] = MUSH_OUTLINE_COLOUR.y;
  // buffer[i++] = MUSH_OUTLINE_COLOUR.z;

  // // Vertex 3

  // buffer[i++] = +0.015 - lw;
  // buffer[i++] = +0.015 - lw;

  // buffer[i++] = MUSH_OUTLINE_COLOUR.x;
  // buffer[i++] = MUSH_OUTLINE_COLOUR.y;
  // buffer[i++] = MUSH_OUTLINE_COLOUR.z;

  // buffer[i++] = +0.015 + lw;
  // buffer[i++] = +0.015 + lw;

  // buffer[i++] = MUSH_OUTLINE_COLOUR.x;
  // buffer[i++] = MUSH_OUTLINE_COLOUR.y;
  // buffer[i++] = MUSH_OUTLINE_COLOUR.z;

  // // Vertex 4

  // buffer[i++] = -0.015 + lw;
  // buffer[i++] = +0.015 - lw;

  // buffer[i++] = MUSH_OUTLINE_COLOUR.x;
  // buffer[i++] = MUSH_OUTLINE_COLOUR.y;
  // buffer[i++] = MUSH_OUTLINE_COLOUR.z;

  // buffer[i++] = -0.015 - lw;
  // buffer[i++] = +0.015 + lw;

  // buffer[i++] = MUSH_OUTLINE_COLOUR.x;
  // buffer[i++] = MUSH_OUTLINE_COLOUR.y;
  // buffer[i++] = MUSH_OUTLINE_COLOUR.z;

  // // and back to Vertex 1

  // buffer[i++] = -0.015 + lw;
  // buffer[i++] = -0.015 + lw;

  // buffer[i++] = MUSH_OUTLINE_COLOUR.x;
  // buffer[i++] = MUSH_OUTLINE_COLOUR.y;
  // buffer[i++] = MUSH_OUTLINE_COLOUR.z;

  // buffer[i++] = -0.015 - lw;
  // buffer[i++] = -0.015 - lw;

  // buffer[i++] = MUSH_OUTLINE_COLOUR.x;
  // buffer[i++] = MUSH_OUTLINE_COLOUR.y;
  // buffer[i++] = MUSH_OUTLINE_COLOUR.z;

  // glBufferData(GL_ARRAY_BUFFER, numFloats * sizeof(float), buffer, GL_STATIC_DRAW);

  // glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mushroom::draw(mat4 &worldToViewTransform)
{
  // Destroyed mushrooms should not be drawn
  if (damage >= MUSH_MAX_DAMAGE)
    return;

  // Clamp damage for safety
  int d = std::max(0, std::min(damage, MUSH_MAX_DAMAGE - 1));

  // MVP exactly like Dart
  mat4 T = translate(pos.x, pos.y, 0.0f);
  mat4 MVP = worldToViewTransform * T;

  gpuProg->setMat4("MVP", MVP);

  // Draw the correct geometry for this damage state
  mushroomStates[d].draw();
}
