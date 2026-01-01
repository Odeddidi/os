#!/usr/bin/env python3

import io
import os
import sys


def main() -> int:
    if len(sys.argv) != 2:
        print("Usage: parse_moves.py <input.pgn>", file=sys.stderr)
        return 1

    try:
        import chess.pgn  # type: ignore
    except Exception:
        print(
            "Missing dependency: python-chess\n"
            "Install with: python3 -m pip install python-chess",
            file=sys.stderr,
        )
        return 2

    input_arg = sys.argv[1]

    if os.path.exists(input_arg):
        with open(input_arg, "r", encoding="utf-8", errors="replace") as f:
            pgn_text = f.read()
    else:
        pgn_text = input_arg

    pgn = io.StringIO(pgn_text)
    game = chess.pgn.read_game(pgn)
    if game is None:
        return 0

    moves = [move.uci() for move in game.mainline_moves()]
    print(" ".join(moves))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
