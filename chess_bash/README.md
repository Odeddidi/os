# OS Exercise 01 (`os_ex01`)

This folder contains three small parts:
- `chess_sim.sh`: interactive chessboard “replay” for a PGN game (focus of this exercise).
- `split_pgn.sh`: split a multi-game PGN into one file per game.
- `file_sync.c` (+ `test_file_sync.py`): directory file synchronizer (copies new files; updates if source is newer).

## Chess (PGN replay) — `chess_sim.sh`

### What it does
- Reads a PGN file, prints the header metadata, parses the mainline moves, and lets you step through the position move-by-move in the terminal.
- Keys:
  - `d`: next move
  - `a`: previous move
  - `w`: go to start
  - `s`: go to end
  - `q`: quit

### Requirements
- Bash (script uses arrays).
- Python 3.
- Python package `python-chess` (used to convert PGN → UCI moves via `parse_moves.py`).

### Run
From the repo root:

```bash
cd chess_sim
chmod +x chess_sim.sh
python3 -m pip install python-chess
./chess_sim.sh /path/to/game.pgn
```

Notes:
- If your PGN contains multiple games, split it first (see next section) and run `chess_sim.sh` on one of the generated `*.pgn` files.

## Split a multi-game PGN — `split_pgn.sh`

### Run
```bash
cd os_ex01
chmod +x split_pgn.sh
./split_pgn.sh /path/to/input.pgn ./out_games
```

This writes files like `out_games/input_1.pgn`, `out_games/input_2.pgn`, ...

## File synchronizer — `file_sync.c`

### Build
```bash
cd os_ex01
gcc -Wall -Wextra -O2 -o file_sync file_sync.c
```

### Run
```bash
./file_sync /path/to/source_dir /path/to/dest_dir
```

### Tests
```bash
python3 test_file_sync.py ./file_sync
```
