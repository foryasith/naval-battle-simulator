# ⚓ Advanced Naval Battle Simulator

> SE1012 – Programming Methodology Assignment
> BSc (Hons) in Computer Science / Computer Systems Engineering
> Year 1 — SLIIT
> Due: 22 March 2024 | Weight: 30%

---

## 📖 Overview

This is a command-line naval battle simulator written in **C**, built as a
first-year programming assignment. It simulates World War 2-era naval combat
between a single **Battleship (B)** (Allies) and multiple **Escort Ships (E)**
(Axis Powers) on a 2D ocean battlefield.

The simulator is built in progressive stages — each part adds more realistic
mechanics on top of the previous one.

---

## ⚔️ How It Works

- The battlefield is a **square canvas** of size D × D units
- **One Battleship (B)** is placed on the canvas
- **N Escort Ships (E)** are randomly placed around it
- Each ship has a **gun** that fires shells following **parabolic projectile motion** (no air resistance)
- The shell range is calculated using: `R = u² × sin(2θ) / g`
- **B's attack range** is a full circle (can fire at any angle 0–90°)
- **Each E's attack range** is a ring/annulus (limited angle range per type)
- The simulator determines who gets hit, tracks damage, and saves all results to text files

---

## 🚢 Ship Types

### Battleship Types (choose one at the start)

| Notation | Name | Gun |
|----------|------|-----|
| U | USS Iowa (BB-61) | 50-caliber Mark 7 gun |
| M | MS King George V | (356 mm) Mark VII gun |
| R | Richelieu | (15 inch) Mle 1935 gun |
| S | Sovetsky Soyuz-class | (16 inch) B-37 gun |

### Escort Ship Types (randomly assigned)

| Type | Name | Gun | Impact Power | Angle Range |
|------|------|-----|-------------|-------------|
| EA | 1936A-class Destroyer | SK C/34 naval gun | 8% | 20° |
| EB | Gabbiano-class Corvette | L/47 dual-purpose gun | 6% | 30° |
| EC | Matsu-class Destroyer | Type 89 dual-purpose gun | 7% | 25° |
| ED | F-class Escort Ships | SK C/32 naval gun | 5% | 50° |
| EE | Japanese Kaibokan | 4.7 inch naval guns | 4% | 70° |

> EA's max velocity = 1.2 × Vmax_B (strongest escort)
> All other escort types have max velocity < Vmax_B

---

## 📁 Repository Structure

```
naval-battle-simulator/
├── Part-1-A/
│   ├── Part-1-A.c
│   ├── Part-1-A.exe
│   ├── Initial_Conditions.txt
│   ├── Simulation_Statistics.txt
│   └── Final_Conditions.txt
│
├── Part-1-B/
│   ├── Simulation-1/
│   │   ├── Part-1-B-Sim1.c
│   │   ├── Part-1-B-Sim1.exe
│   │   ├── Initial_Conditions.txt
│   │   ├── Simulation_Statistics.txt
│   │   └── Final_Conditions.txt
│   └── Simulation-2/
│       ├── Part-1-B-Sim2.c
│       ├── Part-1-B-Sim2.exe
│       ├── Initial_Conditions.txt
│       ├── Simulation_Statistics.txt
│       └── Final_Conditions.txt
│
├── Part-1-C/
│   ├── Part-1-C.c
│   ├── Part-1-C.exe
│   ├── Initial_Conditions.txt
│   ├── Simulation_Statistics.txt
│   └── Final_Conditions.txt
│
├── Part-2-A/
│   ├── Part-2-A.c
│   ├── Part-2-A.exe
│   ├── Initial_Conditions.txt
│   ├── Simulation_Statistics.txt
│   └── Final_Conditions.txt
│
└── README.md
```

---

## 🗂️ What Each Part Does

### Part 1-A — Basic Simulation

The foundation of the simulator.

- Sets up the battlefield with B and N randomly placed escort ships
- Each escort ship gets a random type, position, velocity range, and angle range
- Calculates attack ranges using projectile motion physics
- **Single hit kills** — one shell from any E destroys B; one shell from B destroys any E
- Determines if B gets sunk or survives
- If B survives: reports how many E ships were hit and total battle time
- Saves results to `Initial_Conditions.txt`, `Final_Conditions.txt`, `Simulation_Statistics.txt`

### Part 1-B Simulation 1 — Moving Battleship

Adds movement to the simulator.

- B travels through **k randomly generated waypoints** on the canvas
- At each waypoint, a full Part 1-A simulation is run
- Escort ships destroyed in earlier steps are **removed** from later steps
- Simulation stops early if B is sunk
- Results saved for each step

### Part 1-B Simulation 2 — Gun Jam

Same as Simulation 1, but with a mechanical failure.

- After iteration **t**, B's gun jams
- Jammed gun can only fire between **θ_min and 90°** (not 0–90° anymore)
- This reduces B's effective attack range
- Shows how the gun jam changes the outcome compared to Simulation 1

### Part 1-C — Percentage Impact Power

Makes the simulator more realistic — B can no longer be destroyed in one hit.

- Each escort type has an **impact power** (e.g. EA = 8%, EE = 4%)
- Multiple E ships must hit B to destroy it (e.g. 13 EA hits = 100% damage)
- B still destroys any E in one hit
- Tracks B's **cumulative damage** throughout the simulation
- If B survives, reports its **remaining health percentage**

### Part 2-A — Reload Time + Attack Strategy *(Optional)*

Adds realistic gun reload mechanics and intelligent targeting.

- B has a **reload time T_B** between consecutive shots
- Each E type has its own **reload time T_E**
- B uses a **closest-first strategy**: attacks the nearest E ships first to maximize kills before taking damage
- E ships fire continuously based on their reload time
- Simulates a more realistic time-based combat sequence

---

## 🔨 Build & Run

### Requirements

- GCC compiler (MinGW on Windows)
- Windows OS (`.exe` files included for direct use)

### Compile any part yourself

```bash
# Part 1-A
gcc Part-1-A/Part-1-A.c -o Part-1-A/Part-1-A.exe -lm -std=c99

# Part 1-B Simulation 1
gcc "Part-1-B/Simulation-1/Part-1-B-Sim1.c" -o "Part-1-B/Simulation-1/Part-1-B-Sim1.exe" -lm -std=c99

# Part 1-B Simulation 2
gcc "Part-1-B/Simulation-2/Part-1-B-Sim2.c" -o "Part-1-B/Simulation-2/Part-1-B-Sim2.exe" -lm -std=c99

# Part 1-C
gcc Part-1-C/Part-1-C.c -o Part-1-C/Part-1-C.exe -lm -std=c99

# Part 2-A
gcc Part-2-A/Part-2-A.c -o Part-2-A/Part-2-A.exe -lm -std=c99
```

### Run on Windows

Double-click any `.exe` file, or run from PowerShell:

```powershell
.\Part-1-A\Part-1-A.exe
```

---

## 🎮 How to Use

When you run any `.exe`, the program will ask you a series of setup questions:

| Prompt | What to enter |
|--------|--------------|
| Random seed | Any integer (e.g. `42`) — same seed = same battlefield |
| Canvas size D | e.g. `1000` — battlefield will be 1000 × 1000 units |
| Number of escort ships N | e.g. `10` |
| Battleship type | `U`, `M`, `R`, or `S` |
| Vmax for B's shell | e.g. `150` m/s, or `0` to randomly generate |
| Starting position | e.g. `500 500`, or `-1 -1` to randomly generate |

> **Tip:** Use the same seed across all parts to compare results under identical battlefield conditions.

---

## 📊 Output Files

Every simulation run generates these files in the same folder as the `.exe`:

| File | Contents |
|------|----------|
| `Initial_Conditions.txt` | Full battlefield setup before any firing |
| `Simulation_Statistics.txt` | Step-by-step log of every hit and damage event |
| `Final_Conditions.txt` | Final state — who survived, B's health, battle outcome |

---

## 🧪 Example Run (Part 1-A)

```
  ****************************************************
  *      ADVANCED NAVAL BATTLE SIMULATOR             *
  *      SE1012 - Part 1-A                           *
  *      World War 2 Naval Combat Simulation         *
  ****************************************************

  [SETUP] Enter random seed: 42
  [SETUP] Enter canvas size D: 1000
  [SETUP] Enter number of escort ships N (1-100): 10
  Choose battleship type (U/M/R/S): M
  Enter Vmax for battleship shell: 0
  --> Randomly generated Vmax_B = 143.27 m/s
  Enter starting position: -1 -1
  --> Randomly generated position = (512.34, 387.91)

  [GENERATING] Placing 10 escort ships on battlefield...

  ============ SIMULATION RUNNING ============
  B hit E[2] (EA) dist=847.3 t=13.127s
  B hit E[5] (EC) dist=431.2 t=9.368s
  B hit E[8] (EB) dist=1203.1 t=15.648s

  RESULT: Battleship SURVIVED!
  Escort ships hit : 3 / 10
  Battle duration  : 15.648 seconds

  [SAVED] Initial_Conditions.txt
  [SAVED] Simulation_Statistics.txt
  [SAVED] Final_Conditions.txt
```

---

## ⚙️ Physics Reference

The simulator uses standard projectile motion equations:

| Formula | Description |
|---------|-------------|
| `R = u² sin(2θ) / g` | Horizontal range of a shell |
| `t_p = u sinθ / g` | Time to peak height |
| `t_f = 2 × t_p` | Total flight time |
| `g = 9.81 m/s²` | Gravity (constant) |

- Maximum range achieved at **θ = 45°**
- Air resistance and wind are **ignored**
- All ships are **stationary** (no velocity)

---

## 👤 Author

**Prashan Karunarathna**
Module: SE1012 – Programming Methodology
Year 1 | SLIIT

---

## 📄 License

This project was developed for academic purposes as part of the SE1012 module at SLIIT. Not for commercial use.
