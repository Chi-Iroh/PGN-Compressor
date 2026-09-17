from .lib import *

def make_alternative_moves_end():
    s = version(0)
    s += tags([])
    s += en_passant(0, "")
    s += pawn + square("e4")
    s += pawn + square("f6")
    s += pawn + square("a3")
    s += pawn + square("g5")
    s += queen + square("g4")

    s += alternative_moves_start
    s += queen + square("h5")
    s += end_of_game + white_wins
    s += alternative_moves_end

    s += end_of_game + draw

    write_file("End of the game in alternative moves", "cpgn/alternative_moves_end.cpgn", s)
