## Maze-Solving Robot (Arduino) — IR Wall Following + Dead-End Handling

This Arduino sketch controls a two-wheel differential drive robot to autonomously navigate a maze using **three IR reflective sensors** (left / center / right) and **two continuous-rotation servos** (left / right wheels). The robot follows a **wall-following strategy (right-hand rule as the default)**, performs **90° corner turns**, and executes a **180° escape turn** when it detects a dead end.

### Hardware Overview
- **Controller:** Arduino (any compatible board)
- **Motors:** 2 × continuous-rotation servos (left + right wheel)
- **Sensors:** 3 × IR emitter/receiver pairs
  - Left sensor: detects left wall proximity
  - Center sensor: detects wall in front
  - Right sensor: detects right wall / openings
- **Indicators:** Red LEDs for left / center / right wall detection (debugging)

### How Wall Detection Works
Each IR sensor uses a **modulated IR signal**. The code flashes the IR LED with a tone and reads the receiver:
- `irDetect()` emits IR at a given frequency and reads the receiver pin.
- `distance()` sweeps from **38 kHz to 42 kHz** in steps of 500 Hz and sums readings.
  - **Low score (≈0–3):** strong reflection → wall is close  
  - **High score (≈9):** weak/no reflection → open space / no wall detected

This sweep improves robustness against noise and mismatched sensor frequencies.

### Navigation Logic (High Level)
The main loop continuously reads:
- `leftWallValue`
- `centreWallValue`
- `rightWallValue`

Then it applies decision rules:

1. **Dead End Detection**
   - If **front + left + right** are all walls, the robot is at a dead end.
   - It performs a **180° turn** (`spin()`), then moves forward to exit the dead end.
   - A timer (`lastCornerMs`) tracks time since the last corner turn.  
     If the robot stays in a dead-end corridor too long (e.g., > 2.5s, after 15s runtime), it stops permanently to prevent getting stuck in loops.

2. **Right-Hand Rule (Default)**
   - If the **right side is open**, the robot moves forward slightly to clear the corner, then **turns right**, then continues forward.

3. **Front Blocked → Turn Left (when possible)**
   - If there is a wall in front but the **left side is open**, the robot clears the corner and **turns left**.

4. **Wall-Centering Adjustments**
   - While driving forward, the robot compares left vs right wall distance values.
   - If it gets too close to one side, it performs a short correction:
     - `adjustRight()` if too close to the left wall
     - `adjustLeft()` if too close to the right wall

5. **“New Mode” (Change State)**
   - If both side walls are detected for a long time (corridor driving), a `change` flag is enabled.
   - This mode changes how the robot prioritizes turns to help avoid repeating the same pattern in long corridors.

### Motor Control
Motor speed/direction is controlled using servo pulse widths:
- ~1500 µs = stop
- >1500 µs / <1500 µs drive the continuous-rotation servo forward/backward depending on orientation

Key movement functions:
- `moveForward()` — forward drive
- `turnLeft()` / `turnRight()` — timed ~90° turns
- `spin()` — timed ~180° escape turn (dead end)
- `stop()` — stops both wheels

### Notes / Known Limitations
- The `spin()` implementation currently sets **both servos to the same direction**, which may cause veering instead of a perfect 180° rotation on some robots. A true spin usually requires one wheel forward and the other backward.
- Turn delays (e.g., 970 ms / 990 ms) and distance thresholds (e.g., `< 9`) are **calibration-dependent**. You may need to tune them based on servo speed, wheel friction, and sensor placement.

### Files
- `maze_solver.ino` — main Arduino sketch

### Usage
1. Wire servos and IR sensors according to the pin definitions at the top of the sketch.
2. Upload the sketch to your Arduino.
3. Place the robot in a maze with walls detectable by IR reflection.
4. Tune turning delays and thresholds if the robot under-turns/over-turns.
## Tech Stack

### Hardware
- **Microcontroller:** Arduino (UNO / compatible)
- **Actuation:** 2× continuous-rotation micro servos  
  - Example: SparkFun “Servo - Generic (Sub-Micro Size)” (see product page)
- **Sensing:** 3× IR wall sensors (Left / Center / Right) using **modulated IR at ~38 kHz**  
  - IR receiver module with **38 kHz filtering** (e.g., TSOP4038-class receiver) :contentReference[oaicite:1]{index=1}
- **Indicators:** 3× LEDs for wall-detection status (debug)

### Software / Firmware
- **Language:** Arduino C/C++
- **Core library:** `Servo.h` (servo pulse control via `writeMicroseconds`)
- **IR detection method:** Modulated tone + **frequency sweep (38–42 kHz)** for robust wall detection

### Tools
- **Arduino IDE** (upload + Serial Monitor debugging)
- **Tinkercad Circuits** (simulation / prototyping)

