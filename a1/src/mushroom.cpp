// mushroom.cpp

#include "mushroom.h"
#include "main.h"
#include "worldDefs.h"

extern GLFWwindow *window;
#define LINE_HALFWIDTH_IN_PIXELS 2.0

DrawBuffers *Mushroom::db = NULL;
DrawBuffers *Mushroom::maskDb = NULL;

float Mushroom::baseY = 0.0f;
float Mushroom::totalHeight = 0.0f;

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

  for (float theta = 0.0f; theta <= (float)M_PI + 1e-6f; theta += (float)M_PI / (float)PIECES_PER_CIRCLE)
  {
    positions.add(centre + vec2(rInner * cos(theta), rInner * sin(theta)));
    positions.add(centre + vec2(rOuter * cos(theta), rOuter * sin(theta)));
  }

  for (int i = offset; i < positions.size(); i++)
    colours.add(colour);
}

static void uploadInterleavedToBoundVBO(GLuint VBO, const seq<vec2> &positions, const seq<vec3> &colours)
{
  int numFloats = positions.size() * (2 + 3);
  float *buffer = new float[numFloats];

  float *p = buffer;
  for (int i = 0; i < positions.size(); i++)
  {
    *p++ = positions[i].x;
    *p++ = positions[i].y;
    *p++ = colours[i].x;
    *p++ = colours[i].y;
    *p++ = colours[i].z;
  }

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, numFloats * sizeof(float), buffer, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  delete[] buffer;
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
  float R = 0.25f * COL_SPACING; // reduced radius, otherwise mushrooms look too large.

  // Proportions (single model)
  float capR = 0.95f * R;
  float stemW = 0.60f * R;
  float stemH = 0.85f * R;

  // float R0 = 0.33f * cell;  // overall size reference

  float stemBaseY = -R; // bottom of STEM FILL (not the visual bottom)
  float stemTopY = stemBaseY + stemH;
  float capCY = stemTopY + 0.20f * R;

  // Make the mask start at the *visual* bottom (include outline thickness)
  baseY = stemBaseY - lw;

  // Make the mask tall enough to cover the *visual* top (include outline thickness)
  float maskTopY = (capCY + capR) + lw;

  // Total height that the mask must be able to cover
  totalHeight = maskTopY - baseY;

  vec3 capCol = MUSH_BODY_COLOUR;
  vec3 stemCol = vec3(0.92f, 0.88f, 0.75f);
  vec3 outCol = MUSH_OUTLINE_COLOUR;
  vec3 black = vec3(0.0f, 0.0f, 0.0f);

  {
    // ---- VAO/VBO setup (exactly like Dart) ----
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    db = new DrawBuffers(VAO);

    // CPU-side geometry buffers
    seq<vec2> positions;
    seq<vec3> colours;
    GLuint offset;

    // ---- 1) STEM FILL (rectangle fan) ----
    {
      float x0 = -0.5f * stemW;
      float x1 = 0.5f * stemW;
      float y0 = stemBaseY; // Cover stem outline.
      float y1 = stemTopY;

      offset = positions.size();

      positions.add(vec2(0.0f, 0.5f * (y0 + y1))); // center
      positions.add(vec2(x0, y0));
      positions.add(vec2(x1, y0));
      positions.add(vec2(x1, y1));
      positions.add(vec2(x0, y1));
      positions.add(vec2(x0, y0)); // close

      for (int i = offset; i < positions.size(); i++)
        colours.add(stemCol);

      db->mode.add(GL_TRIANGLE_FAN);
      db->first.add(offset);
      db->count.add(positions.size() - offset);
    }

    // ---- 2) CAP FILL (semi-circle fan) ----
    {
      offset = positions.size();
      addSemiCircleFan(positions, colours, vec2(0.0f, capCY), capR, capCol);

      db->mode.add(GL_TRIANGLE_FAN);
      db->first.add(offset);
      db->count.add(positions.size() - offset);
    }

    // ---- 3) STEM OUTLINE (3 edges only: bottom, left, right) ----
    // No top edge so you don't get an extra cyan band where cap meets stem.
    {
      float x0 = -0.5f * stemW;
      float x1 = 0.5f * stemW;
      float y0 = stemBaseY;
      float y1 = stemTopY;

      // bottom edge
      offset = positions.size();
      addThickSegment(positions, colours, vec2(x0, y0), vec2(x1, y0), lw, outCol);
      db->mode.add(GL_TRIANGLE_STRIP);
      db->first.add(offset);
      db->count.add(positions.size() - offset);

      // right edge
      offset = positions.size();
      addThickSegment(positions, colours, vec2(x1, y0), vec2(x1, y1), lw, outCol);
      db->mode.add(GL_TRIANGLE_STRIP);
      db->first.add(offset);
      db->count.add(positions.size() - offset);

      // left edge
      offset = positions.size();
      addThickSegment(positions, colours, vec2(x0, y1), vec2(x0, y0), lw, outCol);
      db->mode.add(GL_TRIANGLE_STRIP);
      db->first.add(offset);
      db->count.add(positions.size() - offset);
    }

    // ---- 4) CAP OUTLINE (thick arc strip) ----
    {
      offset = positions.size();

      float rInner = std::max(0.0f, capR - lw);
      float rOuter = capR + lw;

      addThickArcStrip(positions, colours, vec2(0.0f, capCY), rInner, rOuter, outCol);

      db->mode.add(GL_TRIANGLE_STRIP);
      db->first.add(offset);
      db->count.add(positions.size() - offset);
    }

    // ---- Upload interleaved data to the VBO ----
    uploadInterleavedToBoundVBO(VBO, positions, colours);
  }

  // ============================================================
  // (B) Build the MASK VAO + DrawBuffers (maskDb)
  // ============================================================
  {
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    maskDb = new DrawBuffers(VAO);

    seq<vec2> positions;
    seq<vec3> colours;

    // Build one black rectangle fan that can cover the whole mushroom.
    float maskW = 2.4f * capR;
    float x0 = -0.5f * maskW;
    float x1 = 0.5f * maskW;

    float y0 = baseY;
    float y1 = baseY + totalHeight;

    GLuint offset = positions.size();

    positions.add(vec2(0.0f, 0.5f * (y0 + y1))); // center
    positions.add(vec2(x0, y0));
    positions.add(vec2(x1, y0));
    positions.add(vec2(x1, y1));
    positions.add(vec2(x0, y1));
    positions.add(vec2(x0, y0));

    for (int i = offset; i < positions.size(); i++)
      colours.add(black);

    maskDb->mode.add(GL_TRIANGLE_FAN);
    maskDb->first.add(offset);
    maskDb->count.add(positions.size() - offset);

    uploadInterleavedToBoundVBO(VBO, positions, colours);
  }
}

void Mushroom::draw(mat4 &worldToViewTransform)
{
  // Provide base translation for this mushroom instance
  mat4 T = translate(pos.x, pos.y, 0.0f);

  // Draw the normal mushroom.
  {
    mat4 MVP = worldToViewTransform * T;
    gpuProg->setMat4("MVP", MVP);
    db->draw();
  }

  // 2) Draw damage mask (black rectangle coming from below).
  float maskFrac = 0.0f;

  if (MUSH_MAX_DAMAGE > 0)
    maskFrac = damage / (float)MUSH_MAX_DAMAGE;

  // Manually cap to [0,1]
  if (maskFrac < 0.0f)
    maskFrac = 0.0f;
  if (maskFrac > 1.0f)
    maskFrac = 1.0f;

  if (maskFrac > 0.0f)
  {
    mat4 M = T * translate(0.0f, baseY, 0.0f) * scale(1.0f, maskFrac, 1.0f) * translate(0.0f, -baseY, 0.0f);

    mat4 MVP = worldToViewTransform * M;
    gpuProg->setMat4("MVP", MVP);

    maskDb->draw();
  }
}