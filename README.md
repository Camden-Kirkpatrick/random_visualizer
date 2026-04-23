**Good RNG** — uniform noise with no visible structure

<img width="1912" height="1077" alt="Screenshot 2026-04-21 223843" src="https://github.com/user-attachments/assets/add780c1-f91e-4659-957b-5b8e225179f8" />

**Good RNG with custom colors**

<img width="1916" height="1075" alt="image" src="https://github.com/user-attachments/assets/59a0574d-ec7c-441b-90e5-f29d7bdc2e66" />

**Bad RNG** — diagonal patterns emerge from low-quality bit extraction

<img width="1912" height="1075" alt="image" src="https://github.com/user-attachments/assets/a46fdafa-634b-4031-8779-6a1dd59b4db3" />

**Bad RNG with a low extraction bit** — a lower bit value produces a much more obvious repeating pattern

<img width="1914" height="1074" alt="image" src="https://github.com/user-attachments/assets/0fbf4292-3242-47c9-a4a5-34c195bb65f5" />

**Color percentage mode** — each cell has an independent probability of being "on", here set very low

<img width="1912" height="1072" alt="image" src="https://github.com/user-attachments/assets/4d1cc53c-2565-4146-be2c-059cd52e5112" />

# Random Visualizer

A real-time interactive visualizer for comparing random number generator quality. Renders a 1920x1080 grid of colored cells and lets you swap between a high-quality RNG, a deliberately flawed LCG, and a probability-based mode — making the statistical differences immediately visible.

## Features

- **Good RNG** - `std::ranlux24_base`, produces a visually uniform, noise-like grid
- **Bad RNG** — a hand-rolled Linear Congruential Generator (LCG); low bits produce obvious diagonal/striped patterns, higher bits look more random
- **Color percentage mode** — each cell has a configurable probability of being "on", useful for visualizing density
- **Animation** — optionally re-randomizes the grid every N frames
- **Seed control** — fix a seed and use *Regenerate* to reproduce the exact same image
- **Custom colors** — pick any two colors for the 1/0 cell states
- **Cell size slider** — zoom from 1 px per cell up to 100 px chunky blocks
- **ImGui control panel** — all settings are live-editable with no restart needed

## Controls (ImGui panel)

| Control | Effect |
|---|---|
| Randomize | Re-fill the grid from the current RNG state |
| Seed + Regenerate | Rewind to a fixed seed and regenerate |
| Color #1 / #2 | Color pickers for the two cell states |
| Reset color | Restore default white/black palette |
| Cell size | Slider (1–100 px); resizes and regenerates immediately |
| Animate | Toggle automatic re-randomization |
| Interval in frames | Frames between animation updates (1–240) |
| Good / Bad / Percentage | Switch the active generator |
| Percentage | Probability of a cell being "on" in percentage mode |
| Extraction bit | Which LCG output bit to sample (lower = more patterned) |
| Reset All | Restore every setting to its default |

## LCG bit extraction

The bad RNG uses the recurrence `state = state * 1103515245 + 12345`. Because the period of bit N is 2^(N+1), low bits cycle very quickly and create visible patterns, while higher bits look increasingly random. Setting *Extraction bit* to 0–4 makes the patterns obvious; 12+ looks almost uniform.

> **Frozen animation tip:** if `rows x cols` is a multiple of 2^(extractBit+1), every regeneration lands at the same point in the bit cycle and the image never changes. Fix it by adjusting *Cell size* or *Extraction bit*.

## Running

A pre-built Windows executable is available in [Releases](../../releases). Download the zip, extract, and run `random_visualizer.exe` — no install or dependencies required.