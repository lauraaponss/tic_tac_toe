# Tic Tac Toe Game in C

## Description
This is a simple implementation of the classic Tic Tac Toe game written in C. The program allows up to 3 simultaneous games, with features to save, resume, and track game statistics.

## Game Features
- 3x3 game board
- Support for up to 3 different games
- Save and resume functionality
- Game statistics tracking
- Point-based tiebreaking system

## Rules
1. Players take turns placing their symbol (X or O) on the board
2. The first player to get 3 of their symbols in a row (horizontally, vertically, or diagonally) wins
3. If all spaces are filled without a winner, the game ends in a tie
4. In case of a tie, the player with the most points wins
5. If both players have the same points, the second player wins

## Points System
The game uses a points system for tiebreaking:
- 1 point for creating a potential winning position (2 of your symbols in a row with the third spot empty)
- 2 points for blocking an opponent's potential win

## Installation

### Prerequisites
- GCC compiler
- Linux/Unix environment or WSL (Windows Subsystem for Linux)

### Compilation
To compile the program, run the following command:
```bash
gcc tic_tac_toe.c -o tictactoe
