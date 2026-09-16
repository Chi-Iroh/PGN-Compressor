from .lib import *

def make_readme_example():
    s = version(0)
    s += tags([("Date", "Epoch: 01/01/1970")])
    s += en_passant(2, "10")
    s += pawn + square("e4")
    s += pawn + square("e6")
    s += pawn + square("e5")
    s += pawn + square("d5")

    s += alternative_moves_start
    s += pawn + square("d6") + comment + text("Avoids en passant")
    s += alternative_moves_end

    s += pawn + square("d6")
    s += queen + square("d6")
    s += queen + square("f3")
    s += bishop + square("e7")
    s += pawn + square("d3")
    s += knight + square("f6")
    s += bishop + square("g5")
    s += comment + text("Blunder, Qe5+ wins the bishop.")
    s += kingside_castling
    s += knight + square("c3")
    s += knight + square("d5")
    s += bishop + square("e7")
    s += knight + square("e7")
    s += queenside_castling
    s += pawn + square("a5")
    s += pawn + square("a3")
    s += pawn + square("a4")
    s += pawn + square("b4")
    s += pawn + square("b3")
    s += comment + text("En passant !!")
    s += king + square("d2")
    s += pawn + square("b2")
    s += rook + square("a1")

    s += promotion + promotion_queen + "" # only one pawn ready to promote
    s += "0" # has 2 choices (so  1 bit), either move forward to b1 or take a1, a1 is the smallest number so it's the 1st choice (index 0)

    s += nag(41)
    s += comment + text("Black has the attack")
    s += pawn + square("a4")
    s += queen + square("c3")
    s += king + square("c3")
    s += knight + square("d5")
    s += king + square("c4")
    s += queen + square("b4")
    s += end_of_game + black_wins

    write_file("Example", "cpgn/readme_example.cpgn", s)
