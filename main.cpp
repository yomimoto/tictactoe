#include <iostream>
#include <string>
#include <sstream>

using namespace std;

struct BoardSquare
{
    enum E
    {
        X,
        O,
        Empty
    };
};

struct WinningPlayer
{
    enum E
    {
        X,
        O,
        Stalemate,
        None
    };
};

class Board
{
    private:
        int _width;
        BoardSquare::E* _squares;

    public:
        Board(int width) : _width(width)
        {
            _squares = new BoardSquare::E[width * width];
            for (auto i = 0; i < GetTotalSquares(); i++)
                _squares[i] = BoardSquare::Empty;

        }

        ~Board()
        {
            delete [] _squares;
        }

        int GetWidth(){ return _width;}
        int GetTotalSquares(){return _width * _width;}

        BoardSquare::E GetSquare(int index)
        { 
            return _squares[index];
        }

        void SetSquare(int index, BoardSquare::E square)
        { 
            _squares[index] = square;
        }

        WinningPlayer::E GetConverted(BoardSquare::E square)
        {
            switch (square)
            {
                case BoardSquare::X: return WinningPlayer::X;
                case BoardSquare::O: return WinningPlayer::O;
            }
            return WinningPlayer::None; //just incase?
        }

};

class IRuleEngine
{
    public:
        virtual WinningPlayer::E GetWinningPlayer(Board& board) = 0;
};

class Game
{
    private:
    Board& _board;
    IRuleEngine& _ruleEngine;

    public:
        Game(Board& board, IRuleEngine& ruleEngine) : _board(board), _ruleEngine(ruleEngine)
        {

        }   
        WinningPlayer::E Run() 
        {
            WinningPlayer::E winningPlayer, currentPlayer = WinningPlayer::X;
            while ((winningPlayer = _ruleEngine.GetWinningPlayer(_board)) == WinningPlayer::None)
            {
                Render();
                cout << "\n";

                int input;
                cout << "Move for " << (currentPlayer == WinningPlayer::X ? 'X' : 'O') << ": ";
                cin >> input;
                cin.ignore();

                input -= 1;
                if (input < 0 || input > _board.GetTotalSquares() || _board.GetSquare(input) != BoardSquare::Empty)
                {
                    cout << "Invalid Move!\n";
                    continue;
                }

                _board.SetSquare(input, currentPlayer == WinningPlayer::X ? BoardSquare::X : BoardSquare::O);
                currentPlayer = currentPlayer == WinningPlayer::X ? WinningPlayer::O : WinningPlayer::X;

            }
            
            return winningPlayer;
        }

    public:
    void Render()
    {
        for (auto i = 1; i <= _board.GetTotalSquares(); i++)
        {
            cout << GetSquareChar(i, _board.GetSquare(i - 1)) << " ";
            if ((i % _board.GetWidth()) == 0)
                cout << "\n";
        }
    }
    
    private:

    string GetSquareChar(int index, BoardSquare::E square)
    {
        switch (square)
        {
            case BoardSquare::X: return " X ";
            case BoardSquare::O: return " O ";
        }
        //eturn index + '0';
        string tempstring = "   ";
        stringstream ss;
        ss << index;
        return tempstring.replace(1,ss.str().length(),ss.str());  //create space
    }
};

class NullRuleEngine : public IRuleEngine
{
    public:
    virtual WinningPlayer::E GetWinningPlayer(Board& board)
    {
        WinningPlayer::E _win = WinningPlayer::None;
        bool isStalemateIfNoWin = true;

        for (int i = 0; i < board.GetWidth(); i++)
        {

            BoardSquare::E SquareTest = BoardSquare::Empty;

            //first test
            for (int i2 = 0; i2 < board.GetWidth(); i2++) //---------this checks for horizontal wins
            {
                int AddedNumber = 0;
                if (i != 0) AddedNumber = i * board.GetWidth(); //this is so we can check the first row, then check other rows easy
                
                if (board.GetSquare(i2 + AddedNumber) == BoardSquare::Empty) //check quickly to skip win condition b4 doing other checks
                {
                    isStalemateIfNoWin = false;
                    SquareTest = BoardSquare::Empty;
                    break;
                }

                if (i2 == 0 && board.GetSquare(i2 + AddedNumber) != BoardSquare::Empty) 
                {
                    SquareTest = board.GetSquare(i2 + AddedNumber);
                    continue;
                }

                if (SquareTest == board.GetSquare(i2 + AddedNumber)) continue;
                else
                {
                    SquareTest = BoardSquare::Empty;
                    break;
                }

            } //end sub test loop

            if (SquareTest != BoardSquare::Empty) //check sub test loop 1 for win
            {
                _win = board.GetConverted(SquareTest);
                break;
            } /////////////////////////////

            SquareTest = BoardSquare::Empty;

            //second test
            for (int i2 = 0; i2 < board.GetWidth() * board.GetWidth(); i2+=board.GetWidth()) //---------this checks for horizontal wins
            {
                int AddedNumber = i; //add the main iterator to offset which column we want to look at
                
                if (board.GetSquare(i2 + AddedNumber) == BoardSquare::Empty) //check quickly to skip win condition b4 doing other checks
                {
                    isStalemateIfNoWin = false;
                    SquareTest = BoardSquare::Empty;
                    break;
                }

                if (i2 == 0 && board.GetSquare(i2 + AddedNumber) != BoardSquare::Empty) 
                {
                    SquareTest = board.GetSquare(i2 + AddedNumber);
                    continue;
                }

                if (SquareTest == board.GetSquare(i2 + AddedNumber)) continue;
                else
                {
                    SquareTest = BoardSquare::Empty;
                    break;
                }

            } //end sub test loop

            if (SquareTest != BoardSquare::Empty) //check sub test loop 2 for win
            {
                _win = board.GetConverted(SquareTest);
                break;
            } /////////////////////////////

            if (i == 0) //make sure we only do these diagonal tests once
            {
            
                SquareTest = BoardSquare::Empty;

                //third test diagonal left
                int diagonalcounter = 0;
                for (int i2 = 0; i2 < board.GetWidth() * board.GetWidth(); i2+=board.GetWidth()) //---------this diagonal wins
                {
                    int AddedNumber = diagonalcounter; //diagonal left to right check
                    if (board.GetSquare(i2 + AddedNumber) == BoardSquare::Empty) //check quickly to skip win condition b4 doing other checks
                    {
                        isStalemateIfNoWin = false;
                        SquareTest = BoardSquare::Empty;
                        break;
                    }

                    if (i2 == 0 && board.GetSquare(i2 + AddedNumber) != BoardSquare::Empty) 
                    {
                        SquareTest = board.GetSquare(i2 + AddedNumber);
                        diagonalcounter+=1;
                        continue;
                    }

                    if (SquareTest == board.GetSquare(i2 + AddedNumber)) {diagonalcounter+=1; continue;}
                    else
                    {
                        SquareTest = BoardSquare::Empty;
                        break;
                    }

                } //end sub test loop

                if (SquareTest != BoardSquare::Empty) //check sub test loop 3 for win
                {
                    _win = board.GetConverted(SquareTest);
                    break;
                } /////////////////////////////
            
                SquareTest = BoardSquare::Empty;

                //third test diagonal right
                diagonalcounter = 0;
                for (int i2 = 0 ; i2 < board.GetWidth() * board.GetWidth(); i2+=board.GetWidth()) //---------this diagonal right
                {
                    
                    int AddedNumber = board.GetWidth() - 1;
                    if (i2 != 0) AddedNumber = board.GetWidth() - 1 - diagonalcounter; //diagonal right to left check
                    
                    if (board.GetSquare(i2 + AddedNumber) == BoardSquare::Empty) //check quickly to skip win condition b4 doing other checks
                    {
                        isStalemateIfNoWin = false;
                        SquareTest = BoardSquare::Empty;
                        break;
                    }

                    if (i2 == 0 && board.GetSquare(i2 + AddedNumber) != BoardSquare::Empty) 
                    {
                        SquareTest = board.GetSquare(i2 + AddedNumber);
                        diagonalcounter+=1;
                        continue;
                    }

                    if (SquareTest == board.GetSquare(i2 + AddedNumber)) {diagonalcounter+=1; continue;}
                    else
                    {
                        SquareTest = BoardSquare::Empty;
                        break;
                    }

                } //end sub test loop

                if (SquareTest != BoardSquare::Empty) //check sub test for win
                {
                    _win = board.GetConverted(SquareTest);
                    break;
                } /////////////////////////////


            }//ending do once tests */
   
        }//ending for loop tests

        if (_win != WinningPlayer::None) return _win;
        if (_win == WinningPlayer::None)
        {
            if (isStalemateIfNoWin) return WinningPlayer::Stalemate;
            else return WinningPlayer::None;
        }
        return WinningPlayer::None; //just incase
    }

};


int main()
{
    int input;
    while (!input || input < 0 || input > 9)
    {
        cout << "what dimension board do u want to play on (x * x)\n";
        while (!(cin >> input))
        {
            std::cin.clear(); // clear the error flags
            std::cin.ignore(INT32_MAX, '\n'); // discard the row
            std::cout << "Invalid input! Try again: ";
        }
        
        if (!input || input < 0 || input > 9)
            {
                cout << "invalid board number\n" << "9 is max value, value must be greater than 0 \n";
                input = 0;
            }
    }

    Board board(input);
    NullRuleEngine ruleEngine;

    Game game(board,ruleEngine);
    WinningPlayer::E EndGameCondition = game.Run();
    game.Render();
    cout << "\n";

    switch(EndGameCondition)
    {
        case WinningPlayer::X: cout << "Player X won \n"; break;
        case WinningPlayer::O: cout << "Player Y won \n"; break;
        case WinningPlayer::Stalemate: cout << "Stalemate between the two players \n"; break;
        default: cout << "weird error \n"; break;
    }

    return 0;

}