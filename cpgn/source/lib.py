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

def write_file(description : str, filepath : str, buf : str):
    length = len(buf) % 8
    if length > 0:
        buf += "0" * (8 - length)

    length = len(buf)

    file = open(filepath, "wb")
    while buf != "":
        t = buf[:8]
        n = int(t, 2)

        file.write(bytes([n]))
        buf = buf[8:]
    print(f"{description}: wrote {length // 8} bytes.")

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
