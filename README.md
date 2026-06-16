# Advanced Naval Battle Simulator

## Author
**Name:** Yasith Prashan  
**Course:** Programming Fundamentals (C Language)  
**Assignment:** Advanced Naval Battle Simulator  

---

# Project Overview

The Advanced Naval Battle Simulator is a command-line C program that simulates naval combat between battleships and escort ships. The project is divided into multiple parts, gradually introducing more advanced battle mechanics such as projectile motion, health systems, and strategic target selection.

The simulator demonstrates:

- User input handling
- Mathematical calculations
- Projectile trajectory simulation
- Randomized combat behavior
- Health and damage management
- Strategy-based target selection
- Modular programming concepts

---

# Project Structure
Advanced_Naval_Battle_Simulator/
│
├── part1a.c
├── part1b_sim1.c
├── part1b_sim2.c
├── part1c.c
├── part2a.c
└── README.md

---

# Part 1-A: Basic Projectile Simulation

## Objective

Simulate a battleship firing a projectile toward a target.

### Features

- Input firing angle
- Input projectile speed
- Calculate projectile range
- Determine if target is hit
- Display battle results

### Formula Used

Range of projectile:

R = (v² × sin(2θ)) / g

Where:

- R = Range
- v = Initial Velocity
- θ = Launch Angle
- g = 9.81 m/s²

### Compile

```bash
gcc part1a.c -o part1a -lm
Run
./part1a
Part 1-B: Advanced Naval Simulations
Simulation 1
Features
Multiple battleships
Random target positions
Repeated firing rounds
Hit and miss tracking
Compile
gcc part1b_sim1.c -o sim1 -lm
Run
./sim1
Simulation 2
Features
Battleships with escort ships
Multiple attack rounds
Damage calculations
Fleet engagement simulation
Compile
gcc part1b_sim2.c -o sim2 -lm
Run
./sim2
Part 1-C: Health and Damage System
Objective

Introduce ship health and realistic damage handling.

Features
Ship health points
Damage reduction after attacks
Ship destruction detection
Battle continuation until fleet elimination
Battleship Statistics
Ship Type	Health
Battleship	100
Escort Ship	50
Compile
gcc part1c.c -o part1c -lm
Run
./part1c
Part 2-A (Optional): Intelligent Attack Strategy
Objective

Implement strategic target selection.

Features
Prioritize weakest enemy ship
Smart attack decisions
Improved battle efficiency
Dynamic target selection
Strategy
Scan enemy fleet.
Find ship with lowest health.
Attack highest-priority target.
Repeat until all enemies are destroyed.
Compile
gcc part2a.c -o part2a -lm
Run
./part2a
Sample Output
=================================
ADVANCED NAVAL BATTLE SIMULATOR
=================================

Battleship fires!

Angle: 45 degrees
Velocity: 120 m/s

Projectile Range: 1468.81 m

Target Distance: 1450.00 m

Result: TARGET HIT!
Learning Outcomes

Through this project the following concepts were practiced:

Variables and data types
Loops and conditional statements
Functions
Arrays and structures
Random number generation
Mathematical computations
Simulation modeling
Modular software design
Git Commit History

Recommended commits:

git commit -m "Add Part 1-A projectile simulation"

git commit -m "Add Part 1-B naval battle simulations"

git commit -m "Implement Part 1-C health system"

git commit -m "Add Part 2-A intelligent targeting"

git commit -m "Complete Advanced Naval Battle Simulator assignment"
Technologies Used
C Programming Language
GCC Compiler
Math Library (math.h)
Standard C Libraries
How to Build Everything
gcc part1a.c -o part1a -lm
gcc part1b_sim1.c -o sim1 -lm
gcc part1b_sim2.c -o sim2 -lm
gcc part1c.c -o part1c -lm
gcc part2a.c -o part2a -lm
Future Improvements
Graphical battle visualization
Multiplayer support
Weather effects
Advanced AI opponents
Fleet customization
Different weapon systems
License

This project was developed for educational and academic purposes as part of a university programming assignment.

Acknowledgements

Developed as part of the Programming Fundamentals coursework to demonstrate simulation development and problem-solving using the C programming language.

