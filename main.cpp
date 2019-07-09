/* Console executable, responsible of dealing with user's input.

This Minesweeper version requires the user to interact via command lines.
There are 5 possible difficulties, which vary the size of the board and the number of mines.
Boards are generated randomly, to increase replayability.
When the user selects a cell with no adjacent mines, the board automatically unveils all the empty cells adjacent to it.
Future work: include record of previous games, include time to beat the game, check if the best time has been beaten for the selected difficulty.

Game logic is included on Minesweeper class.

Created by: Angel del Ojo Jimenez, July 2019
 */

#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include "Minesweeper.h"

#define MAX_DIFFICULTY 5    // Check Minesweeper.cpp if this parameter has to be changed.
#define MIN_DIFFICULTY 1
        

/// Function prototypes
void PrintIntro();
void AskForDifficulty();
bool IsDifficultyValid(int);
bool PrepareMemory();
void PrintGameParams();
void PlayGame();
void PrintBoard();
int  GetValidCoordinates();
bool AreCoordinatesValid(int, int, int);
bool ShallPlayAgain();          
void PrintFinalBoard();
void PrintGameSummary();

FMineSweeper Game;            // New game instance, to be reused on each play-through

/// Main loop
int main()
{   
    bool bPlayAgain = false;
    do              
    {
        PrintIntro();
        AskForDifficulty();
        if (PrepareMemory())                // Check that the memory allocation has worked
        {                                   
            PrintGameParams();
            PlayGame();
            PrintGameSummary();
            bPlayAgain = ShallPlayAgain();
        }
        else                                // In case it did nor work, exit the game
        {
            return 0;
        }
        
    }
    while (bPlayAgain);
    return 0;                               // Exit the game
}

/// Print introductory messages    
void PrintIntro()
{
    std::cout << "\n\n\nWelcome to the game:\n\n";
    std::cout << "M     M  II  N     N  E E E E  S S S S  W            W  E E E E  E E E E  P P P P  E E E E  R R R  " << std::endl;
    std::cout << "M M M M  II  N N   N  E        S         W          W   E        E        P     P  E        R     R" << std::endl;
    std::cout << "M  M  M  II  N  N  N  E E E    S S S S    W   WW   W    E E E    E E E    P P P P  E E E    R R R  " << std::endl;
    std::cout << "M     M  II  N   N N  E              S     W W  W W     E        E        P        E        R    R " << std::endl;
    std::cout << "M     M  II  N     N  E E E E  S S S S      W    W      E E E E  E E E E  P        E E E E  R     R" << std::endl;
    std::cout << std::endl << std::endl ;
    std::cout << "Version created by: Angel del Ojo Jimenez, July 2019\n\n\n";                                           
}

/// Ask the player for the desired difficulty until it is a valid one
void AskForDifficulty()
{
    int Difficulty;
    do{                                             // Loop to ensure the player input is acceptable
        std::cout << "Introduce desired difficulty (" << MIN_DIFFICULTY << "-" << MAX_DIFFICULTY << "): ";
        std::cin >> Difficulty;                     // Get difficulty from user input and discard the rest of the buffer
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    } while (!IsDifficultyValid(Difficulty));
    std::cout << std::endl;
    
    Game.SetGameParams(Difficulty);
}

/// Check that the selected difficulty is on the desired range
bool IsDifficultyValid(int Difficulty)
{
    if(Difficulty < MIN_DIFFICULTY || Difficulty > MAX_DIFFICULTY)
    {
        std::cout << "Incorrect difficulty, please try again\n\n\n";
        return false;
    }
    else
    {
        return true;
    }
    
}


/// Allocate memory for the required boards
bool PrepareMemory()
{
    if (Game.Reset())           // Important to check that the memory allocation is successful
    {
        return true;
    }
    else
    {
        std::cout << "Error allocating memory, please restart application \n";
        return false;
    }
}

/// Let the player know how many mines are on this board
void PrintGameParams()
{
    std::cout << Game.GetNumMines() << " mines on this board\n\n";
}

/// Single game playthrough, 
void PlayGame() 
{   
    int Input = 0;

    PrintBoard();                               // Print the user board for the first time so the player has some guidance
    do                                          // Iterate on the game until it is over (either victory or defeat)
    {
        Input = GetValidCoordinates();
        Game.SetCellUserBoard(Input);
        Game.SetCellUserVisitedBoard(Input); 
        Game.SetGameStatus(Input);
        PrintBoard();
    } while (Game.GetGameStatus() == EGameStatus::KeepPlaying);
    PrintFinalBoard();                          // Print board with all the mines
    Game.EraseMemory();                         // Delete all boards from memory
}

/// Print current user board
void PrintBoard()
{
    char *Board = Game.GetUserBoard();
    int Size = Game.GetBoardSize();
    int InxNextLine = Game.GetBoardOrder();
    int RowNum = 0;
    
    if (Board)                      // Print the user board on a clean and stylish way, indicating (x, y) index      
    {
        std::cout << "\n   (x)";
        for (int i = 0; i < InxNextLine; i++) { std::cout << ' ' << i ;}
        std::cout << std::endl;
        std::cout << "(y)   \n";
        std::cout << " 0    " ;
        for (int i = 1; i <= Size; i++)
        {
            RowNum = i / InxNextLine;
            std::cout << '|' << Board[i-1] ;
             if ((i) % InxNextLine  == 0)
            {
                std::cout << '|' << std::endl;
                if (RowNum < InxNextLine)
                {
                std::cout << ' ' << RowNum << "    ";
                }
            }
        }
        std::cout << std::endl << std::endl;
    }
    else                            // The board should always be obtained since it was correctly created before
    {
        std::cout << "Error obtaining user board, please exit\n\n";
    }
}

/// Continuous loop until valid input is detected
int GetValidCoordinates() 
{
    int XIn, YIn;
    int Index;

    do{             // Loop to ensure the coordinates entered are within the board range and not repeated
        std::cout << "Introduce desired point's coordinates x and y separated by a space: ";
        std::cin >> XIn;
        std::cin >> YIn;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        Index = XIn + Game.GetBoardOrder() * YIn; // Boards are defined as arrays, so it is needed to transform from matrix to array index
    } while(!AreCoordinatesValid(XIn, YIn, Index)); 
    
    return Index;
}

/// Check that the input coordinates are on the valid range and not repeated from previous game turns
bool AreCoordinatesValid(int XIn, int YIn, int Index)
{
    char* Board = Game.GetUserBoard();
    if (XIn < 0 || XIn > Game.GetBoardOrder() || YIn < 0 || YIn > Game.GetBoardOrder())         // Index out of range
    {
        std::cout << "\nPlease introduce coordinates in range [0, " 
        << Game.GetBoardOrder() - 1 << "]" << std::endl << std::endl << std::endl;
        return false;
    }
    else if (Board[Index] != '-')                                                               // Cell already visited
    {
        std::cout << "\nPlease introduce an element that is not repeated\n\n\n";
        return false;
    }
    else
    {
        return true;
    }
}

/// Ask the player if game should exit or restart
bool ShallPlayAgain() 
{
    bool bReplay;
    bool bGoodInput = false;
    std::string Input;
    
    do{             // Loop to ensure the input is valid
        std::cout << "Want to play again? (Y/N): ";
        std::cin >> Input;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (Input == "Y" || Input == "y")
        {
            bReplay = true;
            bGoodInput = true;
        }
        else if (Input == "N" || Input == "n")
        {
            bReplay = false;
            bGoodInput = true;
        }
        else
        {
            std::cout << "\nI'm sorry but I did not understand you.\n\n";
        }
        
    } while(bGoodInput == false);
    std::cout << std::endl;
    
    return bReplay;
}

/// Print board with all the mines and all the results 
void PrintFinalBoard()
{
    char *Board = Game.GetNearbyMinesBoard();
    int Size = Game.GetBoardSize();
    int InxNextLine = Game.GetBoardOrder();
    int RowNum = 0;
    
    if (Board)      // If the board was obtained, display it on a clean and stylish way
    {
        std::cout << "Board with all the results\n\n";
        std::cout << "   (x)";
        for (int i = 0; i < InxNextLine; i++) { std::cout << ' ' << i ;}
        std::cout << std::endl;
        std::cout << "(y)   \n";
        std::cout << " 0    " ;
        for (int i = 1; i <= Size; i++)
        {
            RowNum = i / InxNextLine;
            std::cout << '|' << Board[i-1] ;
             if ((i) % InxNextLine  == 0)
            {
                std::cout << '|' << std::endl;
                if (RowNum < InxNextLine)
                {
                std::cout << ' ' << RowNum << "    ";
                }
            }
        }
        std::cout << std::endl;
    }
    else
    {   // The board should always be obtained since it was correctly created before
        std::cout << "Error obtaining user board, please exit\n\n";
    }
    
}

/// Print game results. To be expanded on the future with more items such as play time or game statistics.
void PrintGameSummary()
{   
    if(Game.GetGameStatus() == EGameStatus::GameLost)
    {
        std::cout << "You Lost! \n\n\n";
    }
    else
    {
        std::cout << "You Won! \n\n\n";
    }

}
