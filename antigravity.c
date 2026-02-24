/***********************************************************************
 *  ANTIGRAVITY  –  A 2D Arcade Game built with Raylib
 *
 *  Compile (Windows / MSVC):
 *      cl antigravity.c /I path\to\raylib\include /link path\to\raylib\lib\raylib.lib
 *
 *  Compile (GCC / MinGW / Linux):
 *      gcc antigravity.c -o antigravity -lraylib -lm -lpthread -ldl -lrt -lX11
 *
 *  On Windows with MinGW you may also need:
 *      gcc antigravity.c -o antigravity.exe -lraylib -lopengl32 -lgdi32 -lwinmm
 ***********************************************************************/

#include "raylib.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>

/* ── Tuning constants ─────────────────────────────────────────────── */

#define SCREEN_W        800
#define SCREEN_H        450
#define TARGET_FPS      60

#define GRAVITY         600.0f   /* pixels / sec^2 (normal downward)  */
#define ANTIGRAVITY    -600.0f   /* pixels / sec^2 (reversed upward)  */
#define MAX_VELOCITY    400.0f   /* clamp so player can't fly off     */

#define PLAYER_RADIUS   14.0f
#define PLAYER_X        120      /* fixed horizontal position         */

#define SCROLL_SPEED    200.0f   /* how fast obstacles move leftward  */
#define OBSTACLE_GAP    160.0f   /* vertical opening the player flies through */
#define SPAWN_INTERVAL  2.0f     /* seconds between new obstacle pairs */
#define MAX_OBSTACLES   20

/* ── Data structures ──────────────────────────────────────────────── */

typedef struct {
    float x;
    float gapY;       /* centre-Y of the opening */
    float width;
    bool  active;
    bool  scored;      /* have we already given a point for passing? */
} Obstacle;

typedef enum { STATE_MENU, STATE_PLAYING, STATE_PAUSED, STATE_GAMEOVER } GameState;

/* ── Globals ──────────────────────────────────────────────────────── */

static float     playerY;
static float     playerVelY;
static int       score;
static int       highScore = 0;
static float     spawnTimer;
static GameState state;
static Obstacle  obstacles[MAX_OBSTACLES];

/* Particle spark (tiny visual flair when gravity flips) */
#define MAX_PARTICLES 30
typedef struct { float x, y, vx, vy, life; Color col; } Particle;
static Particle particles[MAX_PARTICLES];

/* ── Helper: spawn a burst of particles at position ───────────────── */
static void SpawnBurst(float px, float py, Color baseCol)
{
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].life <= 0.0f) {
            particles[i].x    = px;
            particles[i].y    = py;
            particles[i].vx   = (float)(rand() % 200 - 100);
            particles[i].vy   = (float)(rand() % 200 - 100);
            particles[i].life = 0.35f + (float)(rand() % 20) / 100.0f;
            particles[i].col  = baseCol;
            break;
        }
    }
}

/* ── Helper: update & draw particles ──────────────────────────────── */
static void UpdateDrawParticles(float dt)
{
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].life > 0.0f) {
            particles[i].life -= dt;
            particles[i].x    += particles[i].vx * dt;
            particles[i].y    += particles[i].vy * dt;
            unsigned char alpha = (unsigned char)(255.0f * (particles[i].life / 0.55f));
            if (alpha > 255) alpha = 255;
            Color c = particles[i].col;
            c.a = alpha;
            DrawCircle((int)particles[i].x, (int)particles[i].y, 2.0f, c);
        }
    }
}

/* ── Reset everything for a new round ─────────────────────────────── */
static void ResetGame(void)
{
    playerY    = SCREEN_H / 2.0f;
    playerVelY = 0.0f;
    score      = 0;
    spawnTimer = 0.0f;
    state      = STATE_MENU;

    for (int i = 0; i < MAX_OBSTACLES; i++)
        obstacles[i].active = false;

    for (int i = 0; i < MAX_PARTICLES; i++)
        particles[i].life = 0.0f;
}

/* ── Spawn a new obstacle pair at the right edge ──────────────────── */
static void SpawnObstacle(void)
{
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (!obstacles[i].active) {
            obstacles[i].active = true;
            obstacles[i].scored = false;
            obstacles[i].x      = (float)SCREEN_W + 20.0f;
            obstacles[i].width  = 40.0f;

            float margin = OBSTACLE_GAP / 2.0f + 30.0f;
            obstacles[i].gapY = margin + (float)(rand() % (int)(SCREEN_H - 2 * margin));
            return;
        }
    }
}

/* ══════════════════════════════════════════════════════════════════ */
/*                          MAIN                                     */
/* ══════════════════════════════════════════════════════════════════ */

int main(void)
{
    srand((unsigned int)time(NULL));

    InitWindow(SCREEN_W, SCREEN_H, "ANTIGRAVITY");
    SetTargetFPS(TARGET_FPS);

    ResetGame();

    bool prevSpace = false;

    /* ── Main game loop ───────────────────────────────────────────── */
    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        /* ── UPDATE ───────────────────────────────────────────────── */

        if (state == STATE_MENU)
        {
            if (IsKeyPressed(KEY_ENTER)) {
                state = STATE_PLAYING;
            }
        }
        else if (state == STATE_PLAYING)
        {
            /* --- Pause toggle ------------------------------------- */
            if (IsKeyPressed(KEY_P)) {
                state = STATE_PAUSED;
            }

            /* --- Gravity / antigravity ----------------------------- */
            bool spaceHeld = IsKeyDown(KEY_SPACE);

            float accel = spaceHeld ? ANTIGRAVITY : GRAVITY;

            playerVelY += accel * dt;

            if (playerVelY >  MAX_VELOCITY) playerVelY =  MAX_VELOCITY;
            if (playerVelY < -MAX_VELOCITY) playerVelY = -MAX_VELOCITY;

            playerY += playerVelY * dt;

            if (spaceHeld && !prevSpace) {
                for (int p = 0; p < 8; p++)
                    SpawnBurst(PLAYER_X, playerY, (Color){180, 120, 255, 255});
            }
            prevSpace = spaceHeld;

            /* --- Ceiling / floor collision → game over ------------- */
            if (playerY - PLAYER_RADIUS < 0) {
                playerY = PLAYER_RADIUS;
                state = STATE_GAMEOVER;
            }
            if (playerY + PLAYER_RADIUS > SCREEN_H) {
                playerY = SCREEN_H - PLAYER_RADIUS;
                state = STATE_GAMEOVER;
            }

            /* --- Spawn obstacles on a timer ----------------------- */
            spawnTimer += dt;
            if (spawnTimer >= SPAWN_INTERVAL) {
                spawnTimer -= SPAWN_INTERVAL;
                SpawnObstacle();
            }

            /* --- Move & collide obstacles ------------------------- */
            for (int i = 0; i < MAX_OBSTACLES; i++) {
                if (!obstacles[i].active) continue;

                obstacles[i].x -= SCROLL_SPEED * dt;

                if (obstacles[i].x + obstacles[i].width < -10) {
                    obstacles[i].active = false;
                    continue;
                }

                if (!obstacles[i].scored &&
                    PLAYER_X > obstacles[i].x + obstacles[i].width)
                {
                    obstacles[i].scored = true;
                    score++;
                }

                float halfGap = OBSTACLE_GAP / 2.0f;
                Rectangle topRect = {
                    obstacles[i].x, 0,
                    obstacles[i].width,
                    obstacles[i].gapY - halfGap
                };
                Rectangle botRect = {
                    obstacles[i].x,
                    obstacles[i].gapY + halfGap,
                    obstacles[i].width,
                    SCREEN_H - (obstacles[i].gapY + halfGap)
                };

                if (CheckCollisionCircleRec(
                        (Vector2){PLAYER_X, playerY}, PLAYER_RADIUS, topRect) ||
                    CheckCollisionCircleRec(
                        (Vector2){PLAYER_X, playerY}, PLAYER_RADIUS, botRect))
                {
                    state = STATE_GAMEOVER;
                }
            }

            if (score > highScore) highScore = score;
        }
        else if (state == STATE_PAUSED)
        {
            if (IsKeyPressed(KEY_P)) {
                state = STATE_PLAYING;
            }
        }
        else /* STATE_GAMEOVER */
        {
            prevSpace = false;
            if (IsKeyPressed(KEY_ENTER)) ResetGame();
        }

        /* ── DRAW ─────────────────────────────────────────────────── */

        BeginDrawing();
        ClearBackground((Color){18, 18, 30, 255});   /* deep blue-black */

        /* subtle starfield background */
        for (int s = 0; s < 60; s++) {
            int sx = (s * 137 + (int)(GetTime() * (20 + s % 5))) % SCREEN_W;
            int sy = (s * 97) % SCREEN_H;
            unsigned char brightness = (unsigned char)(60 + (s * 3) % 80);
            DrawPixel(sx, sy, (Color){brightness, brightness, brightness, 200});
        }

        /* draw obstacles */
        for (int i = 0; i < MAX_OBSTACLES; i++) {
            if (!obstacles[i].active) continue;

            float halfGap = OBSTACLE_GAP / 2.0f;

            /* top pillar */
            DrawRectangle(
                (int)obstacles[i].x, 0,
                (int)obstacles[i].width,
                (int)(obstacles[i].gapY - halfGap),
                (Color){0, 200, 120, 220});

            /* bottom pillar */
            DrawRectangle(
                (int)obstacles[i].x,
                (int)(obstacles[i].gapY + halfGap),
                (int)obstacles[i].width,
                (int)(SCREEN_H - (obstacles[i].gapY + halfGap)),
                (Color){0, 200, 120, 220});

            /* bright edge lines */
            DrawRectangleLines(
                (int)obstacles[i].x, 0,
                (int)obstacles[i].width,
                (int)(obstacles[i].gapY - halfGap),
                (Color){100, 255, 180, 255});
            DrawRectangleLines(
                (int)obstacles[i].x,
                (int)(obstacles[i].gapY + halfGap),
                (int)obstacles[i].width,
                (int)(SCREEN_H - (obstacles[i].gapY + halfGap)),
                (Color){100, 255, 180, 255});
        }

        /* draw player  –  a glowing circle with a directional indicator */
        {
            bool flipped = IsKeyDown(KEY_SPACE) && (state == STATE_PLAYING);
            Color core  = flipped ? (Color){180, 100, 255, 255}
                                  : (Color){255, 200, 60, 255};
            Color glow  = flipped ? (Color){120, 60, 200, 60}
                                  : (Color){255, 180, 30, 60};

            /* outer glow rings */
            DrawCircle(PLAYER_X, (int)playerY, PLAYER_RADIUS + 8, glow);
            DrawCircle(PLAYER_X, (int)playerY, PLAYER_RADIUS + 4,
                       (Color){glow.r, glow.g, glow.b, 100});

            /* solid core */
            DrawCircle(PLAYER_X, (int)playerY, PLAYER_RADIUS, core);

            /* small triangle showing gravity direction */
            float dir = flipped ? -1.0f : 1.0f;
            Vector2 triA = { PLAYER_X,          playerY + dir * (PLAYER_RADIUS + 6) };
            Vector2 triB = { PLAYER_X - 5.0f,   playerY + dir * (PLAYER_RADIUS - 2) };
            Vector2 triC = { PLAYER_X + 5.0f,   playerY + dir * (PLAYER_RADIUS - 2) };
            DrawTriangle(triA, triB, triC, core);
        }

        /* particles */
        UpdateDrawParticles(dt);

        /* ceiling / floor danger lines */
        DrawRectangle(0, 0, SCREEN_W, 3, (Color){255, 60, 60, 180});
        DrawRectangle(0, SCREEN_H - 3, SCREEN_W, 3, (Color){255, 60, 60, 180});

        /* ── MENU / RULES SCREEN ──────────────────────────────────── */
        if (state == STATE_MENU)
        {
            DrawRectangle(0, 0, SCREEN_W, SCREEN_H, (Color){0, 0, 0, 180});

            /* Title */
            const char *title = "ANTIGRAVITY";
            int tw = MeasureText(title, 52);
            DrawText(title, (SCREEN_W - tw) / 2, 40, 52, (Color){180, 100, 255, 255});

            /* Subtitle */
            const char *sub = "A Gravity-Flip Arcade Game";
            int stw = MeasureText(sub, 20);
            DrawText(sub, (SCREEN_W - stw) / 2, 98, 20, (Color){200, 200, 200, 255});

            /* Rules box */
            int boxX = SCREEN_W / 2 - 230;
            int boxY = 140;
            int boxW = 460;
            int boxH = 210;
            DrawRectangle(boxX, boxY, boxW, boxH, (Color){30, 30, 50, 200});
            DrawRectangleLines(boxX, boxY, boxW, boxH, (Color){100, 255, 180, 180});

            DrawText("HOW TO PLAY", boxX + 20, boxY + 14, 24, (Color){100, 255, 180, 255});
            DrawLine(boxX + 20, boxY + 44, boxX + boxW - 20, boxY + 44, (Color){100, 255, 180, 80});

            Color ruleCol = (Color){220, 220, 240, 255};
            int fs = 18;
            DrawText("SPACE (hold)   Flip gravity upward",   boxX + 24, boxY +  56, fs, ruleCol);
            DrawText("SPACE (release) Normal gravity down",  boxX + 24, boxY +  80, fs, ruleCol);
            DrawText("P              Pause / Resume",        boxX + 24, boxY + 104, fs, ruleCol);
            DrawText("ESC            Quit game",             boxX + 24, boxY + 128, fs, ruleCol);

            DrawLine(boxX + 20, boxY + 154, boxX + boxW - 20, boxY + 154, (Color){100, 255, 180, 80});

            Color tipCol = (Color){255, 220, 100, 255};
            DrawText("Dodge the green pillars to score!",    boxX + 24, boxY + 164, fs, tipCol);
            DrawText("Hit a wall, ceiling, or floor = Game Over", boxX + 24, boxY + 186, fs, tipCol);

            /* Start prompt */
            const char *startText = "Press ENTER to start";
            int sw2 = MeasureText(startText, 22);
            if (((int)(GetTime() * 2.0)) % 2 == 0)
                DrawText(startText, (SCREEN_W - sw2) / 2, SCREEN_H - 60, 22, WHITE);
        }

        /* ── HUD (visible during play, pause, and game over) ─────── */
        if (state != STATE_MENU)
        {
            DrawText(TextFormat("SCORE  %d", score), 10, 10, 24, WHITE);
            DrawText(TextFormat("BEST   %d", highScore), 10, 38, 18,
                     (Color){180, 180, 180, 255});

            if (state == STATE_PLAYING)
                DrawText("[P] Pause", SCREEN_W - 110, 10, 16, (Color){160, 160, 180, 200});
        }

        /* ── PAUSE OVERLAY ────────────────────────────────────────── */
        if (state == STATE_PAUSED)
        {
            DrawRectangle(0, 0, SCREEN_W, SCREEN_H, (Color){0, 0, 0, 160});

            const char *pauseTitle = "PAUSED";
            int pw = MeasureText(pauseTitle, 48);
            DrawText(pauseTitle, (SCREEN_W - pw) / 2, SCREEN_H / 2 - 50, 48,
                     (Color){255, 220, 100, 255});

            const char *resumeText = "Press P to resume";
            int rw2 = MeasureText(resumeText, 22);
            if (((int)(GetTime() * 2.0)) % 2 == 0)
                DrawText(resumeText, (SCREEN_W - rw2) / 2, SCREEN_H / 2 + 10, 22,
                         (Color){200, 200, 200, 255});
        }

        /* ── GAME OVER OVERLAY ────────────────────────────────────── */
        if (state == STATE_GAMEOVER)
        {
            DrawRectangle(0, 0, SCREEN_W, SCREEN_H, (Color){0, 0, 0, 160});

            const char *titleText = "GAME OVER";
            int tw2 = MeasureText(titleText, 48);
            DrawText(titleText, (SCREEN_W - tw2) / 2, SCREEN_H / 2 - 60, 48,
                     (Color){255, 80, 80, 255});

            const char *scoreText = TextFormat("Score: %d", score);
            int sw3 = MeasureText(scoreText, 28);
            DrawText(scoreText, (SCREEN_W - sw3) / 2, SCREEN_H / 2, 28, WHITE);

            if (score >= highScore && score > 0) {
                const char *newBest = "NEW BEST!";
                int nw = MeasureText(newBest, 22);
                DrawText(newBest, (SCREEN_W - nw) / 2, SCREEN_H / 2 + 34, 22,
                         (Color){255, 220, 60, 255});
            }

            const char *restartText = "Press ENTER to restart";
            int rw3 = MeasureText(restartText, 20);

            if (((int)(GetTime() * 2.0)) % 2 == 0)
                DrawText(restartText, (SCREEN_W - rw3) / 2, SCREEN_H / 2 + 70, 20,
                         (Color){200, 200, 200, 255});
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
