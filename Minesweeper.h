/* Minesweeper game logic.

There are 4 boards:
1. Board: contains mines (1) and spaces (0). This is randomly generated on each game.
2. UserBoard: used for user interaction. Displays '-' by default, number of mines nearby or X (if a mine has exploded).
3. UserVisitedBoard: contains the information of which cells have been visited (1 means visited, 0 no). Mines are marked as 1.
4. NearbyMinesBoard: contains the number of mines adjacent to each cell. It is generated when the board is generated.

For further functionality and implementation details, check Minesweeper.cpp

Created by: Angel del Ojo Jimenez, July 2019
*/

#pragma once    
#include <string>

/// Game elements needed to create the boards and to check when the game is finished
struct FGameStats
{
    int NumSpacesLeft = 255;
    int NumMinesLeft = 255;
};

/// Status of the current game
enum class EGameStatus
{
    GameWon,
    GameLost,
    KeepPlaying
};

/// Type (location) of a selected cell
enum class ECellType
{
    UpperLeftCorner,
    UpperRightCorner,
    LowerLeftCorner,
    LowerRightCorner,
    UpperHorizontal,
    LowerHorizontal,
    LeftVertical,
    RightVertical,
    Central
};

/// Class that contains all the information of the current game
class FMineSweeper
{
    public:                 // Functions that can be accessed from the outside of the class
        FMineSweeper();     // Constructor, not needed at this point

        /// Getters
        int GetBoardSize() const;
        int GetBoardOrder() const;
        int GetNumMines() const;
        char* GetUserBoard() const;
        char* GetNearbyMinesBoard() const;
        EGameStatus GetGameStatus() const;
        FGameStats GetResults() const;

        ///Setters  
        void SetGameParams(int);     
        void SetCellUserBoard(int);
        void SetCellUserVisitedBoard(int);
        void SetCellNearbyMinesBoard(int);
        void SetGameStatus(int);

        /// Rest of functions
        bool Reset();    
        void EraseMemory();


    private:                // Functions and variables that help the public ones
        /// Boards
        int* Board;    
        char* UserBoard;
        int* UserVisitedBoard;
        char* NearbyMinesBoard;

        /// Game Parameters
        int Difficulty = 0; 
        int BoardOrder = 0;
        int BoardSize = 0;
        int NumMines = 0;
        bool bIsGameWon = false;
        EGameStatus GameStatus;
        FGameStats Results;

        /// Setters
        bool SetBoardInit();
        bool SetUserBoardInit();
        bool SetUserVisitedBoardInit();
        bool SetNearbyMinesBoardInit();

        /// Rest of functions
        ECellType CalcCellType(int);
        int  CountNearbyMines(int);
        int  CheckEmptySurroundingCells(int, int[]);
        int  CheckSingleCell(int, int, int* );
};
