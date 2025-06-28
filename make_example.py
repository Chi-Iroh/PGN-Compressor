null_byte = "0" * 8

def text(str):
    s = ""
    for c in str:
        s += bin(ord(c))[2:].rjust(8, '0')
    s += null_byte
    return s

def tag(name, value):
    return text(name) + text(value)

def tags(tags):
    s = ""
    for (name, value) in tags:
        s += tag(name, value)
    s += null_byte
    return s

king = "000"
queen = "001"
bishop = "010"
knight = "011"
rook = "100"
pawn = "101"

def square(sq):
    file, rank = sq.lower()
    s = bin(ord(file) - ord('a'))[2:].rjust(3, '0')
    s += bin(int(rank) - 1)[2:].rjust(3, '0')
    return s

def value(i):
    return bin(i)[2:].rjust(8, '0')

comment = "11100"
_nag = "11110"
end_of_game = "11111"
white_wins = "00"
black_wins = "01"
draw = "10"
alternative_moves_start = "111011"
alternative_moves_end = "111010"
kingside_castling = "11000"
queenside_castling = "11001"
promotion = "1101"

promotion_queen = "00"
promotion_bishop = "01"
promotion_knight = "10"
promotion_rook = "11"

def nag(i):
    return _nag + value(i)

def version(i):
    return value(i)

def en_passant(n, en_passant_bits):
    return bin(n)[2:].rjust(4, '0') + en_passant_bits

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
s += pawn + square("b3")
s += comment + text("En passant !!")
s += king + square("d2")
s += pawn + square("b2")
s += rook + square("a2")

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

# s += "001010"
# s += "101100011"
# s += "101100101"
# s += "101100100"
# s += "101011100"
# s += "111011"
# s += "0000"
# s += "101011101"
# s += "11100"
# s += comment + text("Avoids en passant")
# s += "111010"
# s += "101011101"
# s += "001011101"
# s += "001101010"
# s += "010100110"
# s += "101011010"
# s += "011101101"
# s += "010110100"
# s += "11100"
# s += comment + text("Blunder, Qe5+ wins the bishop")
# s += "11000"
# s += "011010010"
# s += "011011100"
# s += "010100110"
# s += "011100110"
# s += "11001"
# s += "101000100"
# s += "101000010"
# s += "101000011"
# s += "101001011"
# s += "101001010"
# s += "11100"
# s += comment + text("En passant !!")
# s += "000011001"
# s += "101001001"
# s += "100000000"
# s += "110100"
# s += "1111000101001"
# s += "11100"
# s += comment + text("Black has the attack")
# s += "101000011"
# s += "001010010"
# s += "000010010"
# s += "011011100"
# s += "000010011"
# s += "001001011"

l = len(s) % 8
if l > 0:
    s += "0" * (8 - l)

l = len(s)

file = open("example.cpgn", "wb")
while s != "":
    t = s[:8]
    n = int(t, 2)

    file.write(bytes([n]))
    s = s[8:]
print(f"Wrote {l // 8} bytes.")