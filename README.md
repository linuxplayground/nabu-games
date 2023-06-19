# Collection of games written for the Nabu

## Published on DJ Sures Cloud CP/M

* Snake
* Tetris
* Invaders ?

## Snake

Control the snake to get it to eat the apples.  Each time you eat an apple, you
grow.  Every 5 apples you eat, the game speeds up.

Do not crash into your own tail or the walls.  The goal is to stay alive as long
as possible.  You get 1 point for each apple you eat.

## Tetris

Fairly typical tetris clone.  Every 10 lines completed advances your level.  The
game speeds up every time you level up.  If you clear 4 lines at once, you earn
10 bonus points.

Controls:

z rotate counter clockwise
x rotate clockwise
, move left
. move right
SPACE drop

Or use the Joystick

## Invaders

A standard Space Invaders Clone.  You have 3 lives, and gain one extra life when
you pass 1500 points.

Points are scored for shooting down the invaders which are marching across the
screen.  When they reach the edge of the screen the drop down one line.
When the invaders reach the ground the game is over.

A UFO will appear occasionally which grants more points if you are able to shoot
it down.

Use the joystick to play.  LEFT AND RIGHT AND FIRE TO SHOOT.  That's it.

## NABU-LIB

The games in this repo depend on NABU-LIB developed by DJSures. [https://github.com/DJSures/NABU-LIB](https://github.com/DJSures/NABU-LIB)

## z80retro-lib

A port of NABU-LIB is available at [https://github.com/linuxplayground/z80retro-lib](https://github.com/linuxplayground/z80retro-lib) and is used in this project to cross-compile these games for the Z80_Retro.  
For more inforamtion on the Z80_Retro board see: [https://github.com/Z80-Retro](https://github.com/Z80-Retro)

## Development Guides

More inforamtion on how to set up your development environment and how to build and load the games into the NABU and Z80 Retro platforms can be found in the [docs](docs) folder.

* [docs/BUILD.md](./docs/BUILD.md) - General setup and build guide
* [docs/NABU.md](./docs/NABU.md) - How to load the games into the NABU platform
* [docs/RETRO.md](./docs/RETRO.md) - How to load the games into the Z80-Retro platform.
