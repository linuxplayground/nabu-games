# Port notes

In hbc-56, troy stores the game field in a data structure in memory.  Each element in the data structure represents 1 alien.  Something like this:

```text
3 3 3 3 3 3 3 3 3 3 3 x x x x x
2 2 2 2 2 2 2 2 2 2 2 x x x x x
2 2 2 2 2 2 2 2 2 2 2 x x x x x
1 1 1 1 1 1 1 1 1 1 1 x x x x x
1 1 1 1 1 1 1 1 1 1 1 x x x x x
x x x x x x x x x x x
```

where the numbers represent an invader pattern name and the `x` represents an empty location.

The game loop then updates the pattern for each pattern name to move the aliens along by 2 pixels within the tile.

Once the patterns have been updated 4 times, the patterns are all reset to their starting pattern and the gamefield is shifted.
