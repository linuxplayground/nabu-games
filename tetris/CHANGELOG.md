# Tetris Change Log

v3.0:

Based on https://tetris.wiki/Tetris_Guideline#Indispensable_rules

NOTE: Not all the guidelines are implimented in this version of the game.

_Rotations can not result in a tetronimo being kicked into a new position if the basic rotation target space is obstructed._

* Play area reduced from 12 to 10 cells wide. - (Playfield)
* Tetronimos all start with flat side down. (Tetromino starting positions)
* Tetronimos have their rotations now set according to: https://tetris.wiki/Super_Rotation_System#Spawn_Orientation_and_Location
* Scoring is based on the original BPS system.
    * General Line Clears

      |Line Clear|Points
      |---|---
      |1 (single) | 40
      |2 (double) | 100
      |3 (triple) | 300
      |4 (tetris) | 1200
    
    * Drops

      If the piece was hard dropped, an amount equal to the number of rows covered by the hard drop plus 1 is added. Otherwise, no score for dropping is added.
