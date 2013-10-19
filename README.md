interrupt-handling-on-Intel-80x86-architecture
==============================================

Packman game was made for interrupt handling on Intel 80x86 architecture

In this experiment you are going to learn how to use interrupt handling on Intel 80x86 architecture.
You will implement a simple game,Pacman.One player versus to computer plays this game.The game area is considered as a labyrinth,which is given from an input file and output prefix from console while the program will be called 
(PACMAN.EXE <input> <output_prefix>). The input file consists of numbers contain information about the labyrinth of the game. The first line of the input file is dimensions of the labyrinth. The  rest of the file contains the matrix that represents the labyrinth.

A sample game input which contains two monsters and four diamonds is given below: 
12x6
111111111111
000111000131
112003410000
111110111011
000300130002
111111111111

The number “1” denotes the walls, the number “0” denotes the paths, the number “2” denotes monsters, the number “3” denotes diamonds, and the number “4” denotes the pacman.At the start of the program, the input file is loaded and the
game starts automatically in text screen mode. The pacman can be moved by the arrow keys. The monsters continuously in the labyrinth and their movement is random. they should be able to choose other (non-­‐blocked) paths when path is blocked by a wall. pacman reaches an exit, the game ends after count of diamonds collected. If any monster touches to pacman, the game be over. Also the key “D” (not “d”) should dump the snapshot of game (labyrinth, monsters and into incrementally named files (<output_prefix>0.txt, <output_prefix>1.txt…) as in the input file format.Thus, a dump file can be used as an input file (allows resuming the game).

You do not use scrolling text output to the screen. You should draw the whole labyrinth into the same place. The speed
of the game should be 1 move/second that means every monster can move at 1 point in second. You should prepare a required timer for the speed. The size of a labyrinth will not exceed the screen resolution for text mode (80x25).
However the size of the labyrinth can change.
