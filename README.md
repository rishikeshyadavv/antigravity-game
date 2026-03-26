# 🚀 ANTIGRAVITY

**A 2D Gravity-Flip Arcade Game built with C and Raylib**

*Developed by **Rishikesh Yadav***

---

![Language](https://img.shields.io/badge/Language-C-blue?style=flat-square)
![Library](https://img.shields.io/badge/Library-Raylib-green?style=flat-square)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-lightgrey?style=flat-square)
![License](https://img.shields.io/badge/License-MIT-yellow?style=flat-square)

## 🎮 About

**Antigravity** is a fast-paced arcade game where the player controls a glowing orb navigating through an endless field of obstacles. The core mechanic is simple yet addictive — **hold SPACE to flip gravity upward**, release to fall back down. Dodge green pillars, rack up your score, and try to beat your high score!

## 🎮 Play Online

🚀 **[Play the game directly in your browser here!](https://tempantigravitydeploy.vercel.app)** 🚀

*(Web version supports both Keyboard and Touch/Click inputs!)*

## ✨ Features

- **Gravity-flip mechanic** — Hold SPACE to defy gravity; release to fall
- **Procedurally generated obstacles** — Randomized pillar gaps for endless replayability
- **Particle effects** — Burst of sparks when gravity flips
- **Animated starfield background** — Subtle parallax stars for visual depth
- **Glowing player** — Colour-changing orb with directional indicator
- **Pause / Resume** — Press `P` to pause mid-game
- **High-score tracking** — Per-session best score displayed on the HUD
- **In-game rules screen** — Full instructions shown on the menu

## 🕹️ Controls

| Key | Action |
|-----|--------|
| **SPACE** (hold) | Flip gravity upward |
| **SPACE** (release) | Normal gravity (downward) |
| **P** | Pause / Resume |
| **ENTER** | Start game / Restart after Game Over |
| **ESC** | Quit |

## 🛠️ Build Instructions

### Prerequisites

- **C Compiler** — GCC (MinGW on Windows) or MSVC
- **Raylib** — [Download Raylib](https://www.raylib.com/)

### Windows (MinGW / GCC)

```bash
gcc antigravity.c -o antigravity.exe -I raylib/include -L raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm
```

### Windows (MSVC)

```bash
cl antigravity.c /I path\to\raylib\include /link path\to\raylib\lib\raylib.lib
```

### Linux (GCC)

```bash
gcc antigravity.c -o antigravity -lraylib -lm -lpthread -ldl -lrt -lX11
```

## 📁 Project Structure

```
antigravity-game/
├── antigravity.c      # Complete game source code
├── .gitignore         # Ignores compiled binaries and raylib dependency
├── raylib/            # Raylib library (not tracked in git)
└── README.md          # This file
```

## 🎯 How It Works

1. The player orb is fixed horizontally and moves vertically based on gravity
2. Holding **SPACE** applies upward acceleration (antigravity); releasing applies downward gravity
3. Green pillar obstacles scroll from right to left with randomized gap positions
4. Score increases each time the player passes through an obstacle gap
5. Hitting an obstacle, the ceiling, or the floor ends the game

## 👤 Author

**Rishikesh Yadav**
- GitHub: [@rishikeshyadavv](https://github.com/rishikeshyadavv)

## 📄 License

This project is open source and available under the [MIT License](LICENSE).