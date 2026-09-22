# PGN Compressor

<ins>P</ins>ortable <ins>G</ins>ame <ins>N</ins>otation (PGN) is a file format used to represent chess games.  
It contains every move, the result of the game, and may include extra information about, but not limited to, the place or the players.  

This project is an attempt of lossless (*) binary compression of those files, alongside with decompression.  

(*) Extra spaces, tabulations and newlines will be erased though. Therefore the decompressed file will have less spaces than an original file with extra spaces.  

You can see the algorithm in [ALGORITHM.md](ALGORITHM.md).  

## Uselessness warning

This project is basically useless because :  
- PGN files are very lightweight because chess games don't have millions of moves (up to kilobytes for real games and megabytes for longest theoretical games)
- [Lichess does it better](https://lichess.org/@/lichess/blog/developer-update-275-improved-game-compression/Wqa7GiAA), [and even better](https://lichess.org/@/marcusbuffett/blog/compressing-chess-moves-even-further-to-37-bits-per-move/YgugQc42) (discovered those articles very recently despite my project being a few years old)

I just wanted to try to implement a simple binary compression algorithm using chess rules as much as possible to optimize.  
It's mainly for fun and doesn't aim to try to be the best method at all.  

## Build

Requirements: a C99-compliant compiler.  
Optional requirements: [Criterion tests library](https://github.com/Snaipe/Criterion), [ASan](https://github.com/google/sanitizers/wiki/addresssanitizer) + [UBSan](https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html)

```bash
make # compiles to an executable (release)
make debug # compiles to an executable (debug)
make sanitize # compiles to an executable (debug) + sanitizers (asan + ubsan)
make tests && ./tests/test # Builds and run the tests
```
These commands suppose you use Clang.  
If not, use the `CC=xxx` to force the Makefile to use your compiler :  
```bash
make CC=gcc debug
```
Builds a debug executable using GCC.  
Using an other compiler than Clang may fail (due to compiler flags not being the same), however it should work with GCC as it's close to Clang.  

## AI policy

Nothing (code, documentation, etc.) has been AI-generated in this project.  
