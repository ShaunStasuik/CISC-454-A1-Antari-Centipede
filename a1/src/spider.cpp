#include "spider.h"
#include "main.h"
#include "worldDefs.h"

#include <cmath>
#include <cstdlib>

extern GLFWwindow *window;
#define LINE_HALFWIDTH_IN_PIXELS 2.0f

static const vec3 SPIDER_BODY_COL(0.95f, 0.30f, 0.30f);
static const vec3 SPIDER_LEG_COL(0.70f, 0.55f, 0.20f);

DrawBuffers *Spider::db = NULL;

static float rand01() { return (float)rand() / (float)RAND_MAX; }

// thick segment like mushroom/centipede
static void addThickSegment(seq<vec2> &pos, seq<vec3> &col, vec2 a, vec2 b, float hw, vec3 c)
{
    vec2 d = (b - a).normalize();
    vec2 p(-hw * d.y, hw * d.x);

    GLuint off = pos.size();
    pos.add(a + p);
    pos.add(a - p);
    pos.add(b + p);
    pos.add(b - p);

    for (int i = off; i < pos.size(); i++)
        col.add(c);
}

static void addCircleFan(seq<vec2> &pos, seq<vec3> &col, vec2 centre, float r, vec3 c)
{
    GLuint off = pos.size();
    pos.add(centre);
    for (float t = 0; t < 2.0f * (float)M_PI; t += 2.0f * (float)M_PI / (float)PIECES_PER_CIRCLE)
        pos.add(centre + vec2(r * cos(t), r * sin(t)));
    pos.add(pos[off + 1]); // close

    for (int i = off; i < pos.size(); i++)
        col.add(c);
}

static void uploadInterleaved(GLuint VBO, const seq<vec2> &pos, const seq<vec3> &col)
{
    int n = pos.size();
    float *buf = new float[n * 5];
    float *p = buf;
    for (int i = 0; i < n; i++)
    {
        *p++ = pos[i].x;
        *p++ = pos[i].y;
        *p++ = col[i].x;
        *p++ = col[i].y;
        *p++ = col[i].z;
    }
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, n * 5 * sizeof(float), buf, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    delete[] buf;
}

Spider::Spider(vec2 startPos, vec2 startVel)
{
    pos = startPos;
    vel = startVel;
    alive = true;
    changeTimer = 0.2f + 0.6f * rand01();

    if (db == NULL)
        generateVAOs();
}

float Spider::radius() const { return SPIDER_RADIUS; }

void Spider::generateVAOs()
{
    // line width in world coords
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    float lw = LINE_HALFWIDTH_IN_PIXELS / (float)h * 2.0f;

    // VAO/VBO like Dart
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

    seq<vec2> positions;
    seq<vec3> colours;
    GLuint offset;

    float R = SPIDER_RADIUS;
    float Rh = 0.55f * R; // head radius

    // body
    offset = positions.size();
    addCircleFan(positions, colours, vec2(0, 0), R, SPIDER_BODY_COL);
    db->mode.add(GL_TRIANGLE_FAN);
    db->first.add(offset);
    db->count.add(positions.size() - offset);

    // head (front)
    offset = positions.size();
    addCircleFan(positions, colours, vec2(0.6f * R, 0.0f), Rh, SPIDER_BODY_COL);
    db->mode.add(GL_TRIANGLE_FAN);
    db->first.add(offset);
    db->count.add(positions.size() - offset);

    // legs (4 on each side), anchored on the body boundary
    float legLen = 1.25f * R;
    float legW = 0.6f * lw;

    for (int side = -1; side <= 1; side += 2)
    { // -1 left, +1 right

        for (int i = 0; i < 4; i++)
        {

            // vertical placement along the body
            float y = (-0.55f + 0.37f * i) * R;

            // angle fan upward a bit as i increases
            float theta = 0.55f + 0.18f * i;

            // anchor on the *edge* of the body (not center)
            float bodyEdgeX = 1.02f * R; // pushes legs outside red blob
            vec2 a(side * bodyEdgeX, y);

            // endpoint goes outward (x) and a bit up (y)
            vec2 b = a + vec2(side * cos(theta) * legLen,
                              sin(theta) * legLen);

            offset = positions.size();
            addThickSegment(positions, colours, a, b, legW, SPIDER_LEG_COL);

            db->mode.add(GL_TRIANGLE_STRIP);
            db->first.add(offset);
            db->count.add(positions.size() - offset);
        }
    }

    uploadInterleaved(VBO, positions, colours);
}

void Spider::update(float elapsedTime)
{
    // Move
    pos = pos + elapsedTime * vel;

    // Occasionally change vertical behavior
    changeTimer -= elapsedTime;
    if (changeTimer <= 0.0f)
    {
        // pick a new vertical mode: up, down, or almost flat
        float r = rand01();
        float vy = 0.0f;
        if (r < 0.33f)
            vy = +SPIDER_SPEED_Y;
        else if (r < 0.66f)
            vy = -SPIDER_SPEED_Y;
        else
            vy = 0.0f;

        vel.y = vy;

        // next change in ~[0.4, 1.2]
        changeTimer = 0.4f + 0.8f * rand01();
    }

    // Keep it in the lower band (bounce vertically)
    if (pos.y < SPIDER_Y_MIN)
    {
        pos.y = SPIDER_Y_MIN;
        vel.y = fabs(vel.y);
    }
    if (pos.y > SPIDER_Y_MAX)
    {
        pos.y = SPIDER_Y_MAX;
        vel.y = -fabs(vel.y);
    }
}

void Spider::draw(mat4 &worldToViewTransform)
{
    mat4 T = translate(pos.x, pos.y, 0.0f);
    float angle = atan2(vel.y, vel.x); // face movement direction
    mat4 R = rotate(angle, vec3(0, 0, 1));
    mat4 MVP = worldToViewTransform * T * R;
    gpuProg->setMat4("MVP", MVP);
    db->draw();
}
