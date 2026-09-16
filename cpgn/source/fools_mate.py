from .lib import *

def make_fools_mate():
    s = version(0)
    s += tags([])
    s += en_passant(0, "")
    s += pawn + square("e4")
    s += pawn + square("f6")
    s += pawn + square("a3")
    s += pawn + square("g5")
    s += queen + square("h5")
    s += end_of_game + white_wins
    write_file("Fool's mate", "cpgn/fools_mate.cpgn", s)
