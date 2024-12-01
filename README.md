# ECE6122 Final Project: 3D Chess Game Custom Classes & OpenGL

## Nov 18th, 2024
* Initially build the project from the solution of Lab3
* Process with `LightPower`, `LightPosition`, `Camera Position` through adding `commandControl` in control.cpp
* Use another `thread` and `atomic flag` to listen to the command input so that the listen to winodow `poll event` can execute simultaneously.

## Nov 25th, 2024
* Create a new `ChessGame` class to record the chess lifespan.
* There should a `ChessGame controller`, (similar to control.hpp) to control and interact with chess game Module and render interface.
    * This need to split the whole programs into serveral parts: **render**, **module** and **control**. The module shoule include ChessGame and GameEngine, the control should include render, command, and game engine control. 

## Nov 26th, 2024
* Integrate the movement control and alter the `tModelMap` to facilitate the update from movement control
* The render may need to consider about **dynamic renderation**, e.g. one movement may take about 1 second instead of directly disappear and appear at another place.
    * Think about the **capture**, where not only the offender will move to the defender's position but also the defender wil move outside the chessboard to a given position. They will be rended simultaneously.

## Nov 27th, 2024
* Add a `DualMovement` struct in `ChessGame` to facilitate the rend for both offender and defender.
* Finish **dynamic renderation** within 2 seconds and the deleted chess will be put outside the chessboard.
* Consider about include **GameEngineer** and make the chess into a round-by-round game.
    * You may consider about the `initialization`, `sendrequire` and `getresponse` from `Komodo` and make a new class to manage that.
    * Think about the winner criteria.

## Nov 29th, 2024
* Finish almost the foundamential functions: **renderation**, **user command interface**, **chessgame state control** and **chess engine interface**.
* Add the z-axis renderation for `Knight` and `capturing`.
* Some potential bug:
    * The komodo chess engine may not know which side it takes, which may lead it to operate its opponent's chess.
    * Define the winning criteria or deliemma
    * Finish some special rules like pown's promotion and En Passant, and king's casting.

## Nov 30th, 2024
* Finish the `gameover` critera
* Almost finish the `En passant`, `Pawn Promotion` and `King Castling`
    * `King Castling` is done.
        * Example of `King Castling`:
        * White(User)   Black(Komodo)
        * e2e4          d7d5
        * e4d5          d8d5
        * g1e2          e7e5
        * d2d3          g8f6
        * b1c3          d5e6
        * b2b4          f8b4
        * d1d2      **e8g8**
    * The `En passant` exits logic error, and `Pawn Promotion` hasn't been tested.

## Dec 1st, 2024
* Update `CMakeLists.txt` so that the whole program can run at Linux as well.
    * Make `ECE_ChessEngine` class compile at the Linux using **macro** and
    * In linux, use `chmod +x ./Linux/komodo-14.1-linux` to enable write file in Linux before enter `./finalProject`
* Add the *user's choice* of chess at the beginning of game
* One possible **Check** test case:  
    * White(User)       Black(Komodo)
    * e2e4              d7d5
    * f2f3              e7e5
    * d1e2              d5d4
    * d2d3              b8c6
    * b1a3              g8f6
    * c1g5              f8b4
    * e1f2              f6e4
    * **Check**         **CheckMate** 

* The **demo** test case:
    * *User*            *Komodo*
    * ***White***       ***Black***
    * e2e4              d7d5
    * e4d5              d8d5
    * g1e2              e7e5
    * d2d3              g8f6
    * b1c3              d5e6
    * b2b4              f8b4
    * d1d2              e8g8
    * a1b1              b8c6
    * d3d4              e5d4
    * d2f4              f6d5
    * b1b4              d5f4
    * e2f4              **Checkmate**
