/* Minesweeper game functionality and implementation details.

Created by: Angel del Ojo Jimenez, July 2019
*/

#pragma once
#include "Minesweeper.h"
#include <iostream>
#include <map>
#include <ctime>


FMineSweeper::FMineSweeper() { }    // Constructor not needed at this point

/// Getters
int FMineSweeper::GetBoardSize() const { return BoardSize; };
int FMineSweeper::GetBoardOrder() const { return BoardOrder; }
int FMineSweeper::GetNumMines() const { return NumMines; };
char* FMineSweeper::GetUserBoard() const { return UserBoard; };
char* FMineSweeper::GetNearbyMinesBoard() const {return NearbyMinesBoard; };
EGameStatus FMineSweeper::GetGameStatus() const { return GameStatus; }
FGameStats FMineSweeper::GetResults() const { return Results;}

/// Setters

/// Modify number of mines and size board depending on the selected difficulty
void FMineSweeper::SetGameParams(int UserDifficulty) 
{ 
    Difficulty = UserDifficulty;
    switch(Difficulty)
    {
        case 1:             // 12.5 % chances of mine on first selected cell
            NumMines = 2;
            BoardOrder = 4;
            BoardSize = BoardOrder * BoardOrder;
            break;
        case 2:             // 16 % chances of mine on first selected cell
            NumMines = 4;
            BoardOrder = 5;
            BoardSize = BoardOrder * BoardOrder;
            break;
        case 3:             // 20.4 % chances of mine on first selected cell
            NumMines = 10;
            BoardOrder = 7;
            BoardSize = BoardOrder * BoardOrder;
            break;
        case 4:             // 23.5 % chances of mine on first selected cell
            NumMines = 19;
            BoardOrder = 9;
            BoardSize = BoardOrder * BoardOrder;
            break;
        case 5:             // 28.4 % chances of mine on first selected cell
            NumMines = 23;
            BoardOrder = 9;
            BoardSize = BoardOrder * BoardOrder;
            break;
        default:            // Same as case 1
            NumMines = 2;
            BoardOrder = 4;
            BoardSize = BoardOrder * BoardOrder;
            break;
    }
}

/// Initialize Board: it contains mines (1) and spaces (0). This is randomly generated on each game.
bool FMineSweeper::SetBoardInit()
{   
    int i = 0; 
    Board = (int *) calloc (sizeof(int), BoardSize);    // calloc since we need initialization to 0
    if (Board != nullptr)
    {   
        srand(time(0));
        Results.NumMinesLeft = 0;
        do                                              // Generate mine at random empty location
	    {
            do                                          // Find a random location that is empty
            {
                i = rand() % (BoardSize + 1) + 0;
            } while (Board[i] !=0 ); 
		    Board[i] = 1;                               // Place mine
		    Results.NumMinesLeft ++; 
	    } while (Results.NumMinesLeft < NumMines);      // Repeat until number of planted mines is reached
        return true;
    } 
    else
    {
        return false;
    }
    
}

/// Initialize User board and game parameters. Each cell contains '-' (by default) or X (mine exploded) at initialization
bool FMineSweeper::SetUserBoardInit()
{
    UserBoard = (char *) malloc (BoardSize * sizeof(char));
    if (UserBoard!= nullptr)
    {
        for(int i = 0; i < BoardSize; i++)
        {
            UserBoard[i] = '-';
        }
        GameStatus = EGameStatus::KeepPlaying;
        Results.NumMinesLeft = NumMines;
        Results.NumSpacesLeft = BoardSize - NumMines;
        return true;
    }
    else
    {
        return false;
    }
}

/// Initialize the User Visited Board. Set 1 on cells with mines, 0 elsewhere (on initialization)
bool FMineSweeper::SetUserVisitedBoardInit()
{
    UserVisitedBoard = (int *) calloc (sizeof(int), BoardSize);
    if (UserVisitedBoard!= nullptr)
    {
        for (int i = 0; i < BoardSize; i++)
        {
            if (Board[i] == 1)
            {
                UserVisitedBoard[i] = 1;
            }
        }
        return true;
    }
    else
    {
        return false;
    }
}

/// Initialize the NearbyMinesBoard. Calculate the number of adjacent mines of each cell on the Board.
bool FMineSweeper::SetNearbyMinesBoardInit()
{
    int Index = 0; 
    NearbyMinesBoard = (char *) malloc (BoardSize * sizeof(char)); 
    if (NearbyMinesBoard != nullptr)
    {   
        for (Index = 0; Index < BoardSize; Index++)
        {
            if(Board[Index] == 0)                   // If there is no mine on this cell, count the number of bombs next to it
            {
                NearbyMinesBoard[Index] = '0' + CountNearbyMines(Index);    // ASCII transformation
            }
            else
            {
                NearbyMinesBoard[Index] = 'X';      // If there is a mine on this cell, set it to 'X'
            }
        }
        return true;
    } 
    else
    {
        return false;
    }
}

/// Initialize all the boards and game parameters
bool FMineSweeper::Reset()
{ 
    return SetBoardInit() * SetUserBoardInit() * SetUserVisitedBoardInit() * SetNearbyMinesBoardInit();
}            

/// Identify whether the cell is on one corner, on a border or elsewhere. Remember that the "boards" are implemented as arrays, so extra calculations are needed.
ECellType FMineSweeper::CalcCellType(int Index)
{
    if (Index == 0)
    {
        return ECellType::UpperLeftCorner;
    }
    else if (Index == BoardOrder - 1)
    {
        return ECellType::UpperRightCorner;
    }
    else if (Index == BoardOrder * (BoardOrder - 1))
    {
        return ECellType::LowerLeftCorner;
    }
    else if (Index == BoardOrder * (BoardOrder - 1) + BoardOrder - 1)
    {
        return ECellType::LowerRightCorner;
    }
    else if ((Index > 0) && (Index < (BoardOrder - 1)))
    {
        return ECellType::UpperHorizontal;
    }
    else if ((Index > (BoardSize - BoardOrder)) && (Index < (BoardSize - 1)))
    {
        return ECellType::LowerHorizontal;
    }
    else if (Index % BoardOrder == 0)
    {
        return ECellType::LeftVertical;
    }
    else if ((Index - BoardOrder + 1) % BoardOrder == 0)
    {
        return ECellType::RightVertical;
    }
    else
    {
        return ECellType::Central;
    }
}

/// Count the number of nearby mines. Code has been organized so it is visually easy to which cells need to be checked on each case.
int FMineSweeper::CountNearbyMines(int Index)
{
   int NMines = 0;
   ECellType CellType = CalcCellType(Index);
   switch(CellType)
   {
        case ECellType::UpperLeftCorner:
            NMines =             0             + Board[Index + 1]             + 
                     Board[Index + BoardOrder] + Board[Index + BoardOrder + 1];
            break;
        case ECellType::UpperRightCorner:
            NMines = Board[Index - 1]              +            0             +
                     Board[Index + BoardOrder - 1] + Board[Index + BoardOrder];
            break;
        case ECellType::LowerLeftCorner:
            NMines = Board[Index - BoardOrder] + Board[Index - BoardOrder + 1] + 
                                 0             + Board[Index + 1];
            break;
        case ECellType::LowerRightCorner:
            NMines = Board[Index - BoardOrder - 1] + Board[Index - BoardOrder] + 
                     Board[Index - 1]              +             0;
            break;
        case ECellType::UpperHorizontal:
            NMines = Board[Index - 1]              +             0             + Board[Index + 1]             +
                     Board[Index + BoardOrder - 1] + Board[Index + BoardOrder] + Board[Index + BoardOrder + 1];
            break;
        case ECellType::LowerHorizontal:
            NMines = Board[Index - BoardOrder - 1] + Board[Index - BoardOrder] + Board[Index - BoardOrder + 1] +
                     Board[Index - 1]              +             0             + Board[Index + 1];
            break;
        case ECellType::LeftVertical:
            NMines = Board[Index - BoardOrder] + Board[Index - BoardOrder + 1] +
                                   0           + Board[Index + 1]              +
                     Board[Index + BoardOrder] + Board[Index + BoardOrder + 1];
            break;
        case ECellType::RightVertical:
            NMines = Board[Index - BoardOrder - 1] + Board[Index - BoardOrder] +
                     Board[Index - 1]              +             0             +
                     Board[Index + BoardOrder - 1] + Board[Index + BoardOrder];
            break;
        case ECellType::Central:
            NMines = Board[Index - BoardOrder - 1] + Board[Index - BoardOrder] + Board[Index - BoardOrder + 1] +
                     Board[Index - 1]              +             0             + Board[Index + 1]              +
                     Board[Index + BoardOrder - 1] + Board[Index + BoardOrder] + Board[Index + BoardOrder + 1];
            break;
        default:
            break;
   }
   return NMines;
}

/// Set "UserBoard" cell to the same as "NearbyMinesBoard" since the number of nearby mines was precalculated on this board
void FMineSweeper::SetCellUserBoard(int Index)
{ 
    UserBoard[Index] = NearbyMinesBoard[Index];
}

/// Check whether the selected cell is empty or has a mine on it. Last argument is modified inside this function.
int FMineSweeper::CheckSingleCell(int InxEmptyCells, int InxToCheck, int EmptyCells[])
{
    if(NearbyMinesBoard[InxToCheck] != 'X')
    {
        EmptyCells[InxEmptyCells] = InxToCheck;
        Results.NumMinesLeft --;
        return (InxEmptyCells+1);
    }
    else
    {
        return InxEmptyCells;
    } 
}

/// Check which of the surrounding cells are empty, depending on the cell location. Returns the number of empty cells next to a given cell.
int  FMineSweeper::CheckEmptySurroundingCells(int Index, int EmptyCells[]) 
{
    int InxEmptyCells = 0;  // Index of one empty cell next to the desired one
    int InxToCheck = 0;     // Index of the cells to check emptyness

    switch(CalcCellType(Index))
    {
        case ECellType::UpperLeftCorner:
            InxToCheck = Index + 1;
            InxEmptyCells = CheckSingleCell(InxEmptyCells, InxToCheck, EmptyCells);
            InxToCheck = Index + BoardOrder;
            InxEmptyCells = CheckSingleCell(InxEmptyCells, InxToCheck, EmptyCells);
            InxToCheck = Index + BoardOrder + 1;
            InxEmptyCells = CheckSingleCell(InxEmptyCells, InxToCheck, EmptyCells);
            break;
        case ECellType::UpperRightCorner:
            InxToCheck = Index - 1;
            InxEmptyCells = CheckSingleCell(InxEmptyCells, InxToCheck, EmptyCells);
            InxToCheck = Index + BoardOrder;
            InxEmptyCells = CheckSingleCell(InxEmptyCells, InxToCheck, EmptyCells);
            InxToCheck = Index + BoardOrder - 1;
            InxEmptyCells = CheckSingleCell(InxEmptyCells, InxToCheck, EmptyCells);
            break;
        case ECellType::LowerLeftCorner:
            InxToCheck = Index - BoardOrder;
            InxEmptyCells = CheckSingleCell(InxEmptyCells, InxToCheck, EmptyCells);
            InxToCheck = Index - BoardOrder + 1;
            InxEmptyCells = CheckSingleCell(InxEmptyCells, InxToCheck, EmptyCells);
            InxToCheck = Index + 1;
            InxEmptyCells = CheckSingleCell(InxEmptyCells, InxToCheck, EmptyCells);
            break;
        case ECellType::LowerRightCorner:
            InxToCheck = Index - BoardOrder - 1;
            InxEmptyCells = CheckSingleCell(InxEmptyCells, InxToCheck, EmptyCells);
            InxToCheck = Index - BoardOrder;
            InxEmptyCells = CheckSingleCell(InxEmptyCells, InxToCheck, EmptyCells);
            InxToCheck = Index - 1;
            InxEmptyCells = CheckSingleCell(InxEmptyCells, InxToCheck, EmptyCells);
            break;
        case ECellType::UpperHorizontal:
            InxToCheck = Index - 1;
            InxEmptyCells = CheckSingleCell(InxEmptyCells, InxToCheck, EmptyCells);
            InxToCheck = Index + 1;
            InxEmptyCells = CheckSingleCell(InxEmptyCells, InxToCheck, EmptyCells);
            InxToCheck = Index + BoardOrder - 1;
            InxEmptyCells = CheckSingleCell(InxEmptyCells, InxToCheck, EmptyCells);
            InxToCheck = Index + BoardOrder;
            InxEmptyCells = CheckSingleCell(InxEmptyCells, InxToCheck, EmptyCells);
            InxToCheck = Index + BoardOrder + 1;
            InxEmptyCells = CheckSingleCell(InxEmptyCells, InxToCheck, EmptyCells);
            break;
        case ECellType::LowerHorizontal:
            InxToCheck = Index - BoardOrder - 1;
            InxEmptyCells = CheckSingleCell(InxEmptyCells, InxToCheck, EmptyCells);
            InxToCheck = Index - BoardOrder;
            InxEmptyCells = CheckSingleCell(InxEmptyCells, InxToCheck, EmptyCells);
            InxToCheck = Index - BoardOrder + 1;
            InxEmptyCells = CheckSingleCell(InxEmptyCells, InxToCheck, EmptyCells);
            InxToCheck = Index - 1;
            InxEmptyCells = CheckSingleCell(InxEmptyCells, InxToCheck, EmptyCells);
            InxToCheck = Index + 1;
            InxEmptyCells = CheckSingleCell(InxEmptyCells, InxToCheck, EmptyCells);
            break;
        case ECellType::LeftVertical:
            InxToCheck = Index - BoardOrder;
            InxEmptyCells = CheckSingleCell(InxEmptyCells, InxToCheck, EmptyCells);
            InxToCheck = Index - BoardOrder + 1;
            InxEmptyCells = CheckSingleCell(InxEmptyCells, InxToCheck, EmptyCells);
            InxToCheck = Index + 1;
            InxEmptyCells = CheckSingleCell(InxEmptyCells, InxToCheck, EmptyCells);
            InxToCheck = Index + BoardOrder;
            InxEmptyCells = CheckSingleCell(InxEmptyCells, InxToCheck, EmptyCells);
            InxToCheck = Index + BoardOrder + 1;
            InxEmptyCells = CheckSingleCell(InxEmptyCells, InxToCheck, EmptyCells);
            break;
        case ECellType::RightVertical:
            InxToCheck = Index - BoardOrder - 1;
            InxEmptyCells = CheckSingleCell(InxEmptyCells, InxToCheck, EmptyCells);
            InxToCheck = Index - BoardOrder;
            InxEmptyCells = CheckSingleCell(InxEmptyCells, InxToCheck, EmptyCells);
            InxToCheck = Index - 1;
            InxEmptyCells = CheckSingleCell(InxEmptyCells, InxToCheck, EmptyCells);
            InxToCheck = Index + BoardOrder - 1;
            InxEmptyCells = CheckSingleCell(InxEmptyCells, InxToCheck, EmptyCells);
            InxToCheck = Index + BoardOrder;
            InxEmptyCells = CheckSingleCell(InxEmptyCells, InxToCheck, EmptyCells);
            break;
        case ECellType::Central:
            InxToCheck = Index - BoardOrder - 1;
            InxEmptyCells = CheckSingleCell(InxEmptyCells, InxToCheck, EmptyCells);
            InxToCheck = Index - BoardOrder;
            InxEmptyCells = CheckSingleCell(InxEmptyCells, InxToCheck, EmptyCells);
            InxToCheck = Index - BoardOrder + 1;
            InxEmptyCells = CheckSingleCell(InxEmptyCells, InxToCheck, EmptyCells);
            InxToCheck = Index - 1;
            InxEmptyCells = CheckSingleCell(InxEmptyCells, InxToCheck, EmptyCells);
            InxToCheck = Index + 1;
            InxEmptyCells = CheckSingleCell(InxEmptyCells, InxToCheck, EmptyCells);
            InxToCheck = Index + BoardOrder - 1;
            InxEmptyCells = CheckSingleCell(InxEmptyCells, InxToCheck, EmptyCells);
            InxToCheck = Index + BoardOrder;
            InxEmptyCells = CheckSingleCell(InxEmptyCells, InxToCheck, EmptyCells);
            InxToCheck = Index + BoardOrder + 1;
            InxEmptyCells = CheckSingleCell(InxEmptyCells, InxToCheck, EmptyCells);
            break;
        default:
            break;
    }
    return *EmptyCells;
}

/// Recursive function to explore all the empty cells next to a given cell 
void FMineSweeper::SetCellUserVisitedBoard(int Index) 
{
    int EmptyNeighbours[8] = {-1, -1, -1, -1, -1, -1, -1, -1};  // Index of the empty neighbours
    if (UserVisitedBoard[Index] == 0)                           // If cell has not been visited before
    {
        UserVisitedBoard[Index] = 1;                            // Mark as visited 
        Results.NumSpacesLeft--;                                // Decrease the number of spaces left on the game
        if (UserBoard[Index] == '0')                            // If there are no mines nearby
        {
            CheckEmptySurroundingCells(Index, EmptyNeighbours); // Check if there are empty neighbours. EmptyNeighbours is modified in this function
            for (int i = 0; i < 8; i++)                         // Iterate on all the empty neighbours
            {
                if (EmptyNeighbours[i] != -1)
                {
                    SetCellUserBoard(EmptyNeighbours[i]);       // Update cell on UserBoard 
                    SetCellUserVisitedBoard(EmptyNeighbours[i]);// Update cell on UserVisitedBoard
                }
                else
                {
                    return;
                }
            }
        }
    }
    else //If cell was already visited, do not check anything
    {
        return;
    }
    
    
}

/// Calculate game status depending on the last selected cell
void FMineSweeper::SetGameStatus(int Index) 
{ 
    if (UserBoard[Index] == 'X')                    // If a mine was found, game is lost
    {
        GameStatus = EGameStatus::GameLost;
    }
    else if ( Results.NumSpacesLeft == 0)           // If all the empty spaces have been checked, game is won
    {
        GameStatus = EGameStatus::GameWon;
    }
    else                                            // Else, continue playing
    {
        GameStatus = EGameStatus::KeepPlaying;
    }
}

/// Erase all the boards from memory
void FMineSweeper::EraseMemory()
{
    free(Board);
    free(UserBoard);
    free(UserVisitedBoard);
    free(NearbyMinesBoard);
}
