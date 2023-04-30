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

The gameloop keeps track of the pixel offset on the X axis.  the cell location for the first item in a row is `floor(x/4)`.  The pattern to use in that cell is x % 4 + first of 8 consecutive patterns.  The reason this is `% 4` and not `% 8` as might be more intuitively guessed is because the invader graphics in this game span two tiles on the horizontal axis.  This is also why the cell location is not `floor(x/8)`. In other words, every alien has a left and right pattern.  There are 4 groups of these left + right patterns for each alien type.  Each group draws the pixels within the tiles in slightly shifted positions to create an animation effect.

Your own graphics routines might only use a single tile in which case, you need to use `floor(x/8)` to select the memory cell and `x % 8 + pattern offset` to select the pattern.

By converting the pixel location to a tile location (x / 8) and the correct left pattern to start with (x % 4) we can update the game field every animation frame and it works.

## Some notes on maths functions used.

8 Bit assembly shortcuts.

### multiply by 32

add hl to hl 5 times.  Do this for any power of 2 multiplcation.  wanna times by 4.  Just add HL to itself, twice.

### Divide by 8.

shift right 3 times.

Divide by 4 is shift right 2 times.

### Mod 4

Just and the last 3 bits.  0x33 % 4 == 0x33 AND 0x03.
in binary this works out like this.

```text
    00110011
AND 00000011
============
    00000011

Result is 3.  or 0x33 == 51.  
  51 % 4 
  ~= (51/4 - floor(51/4) )* 4 
  ~= (12.75 - 12) * 4 
  ~= .75 * 4 
  = 3.
```