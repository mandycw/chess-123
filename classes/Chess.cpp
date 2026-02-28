#include "Chess.h"
#include <string>
#include <limits>
#include <cmath>
#include "Bitboard.h"


Chess::Chess()
{
    _grid = new Grid(8, 8);
}

Chess::~Chess()
{
    delete _grid;
}

char Chess::pieceNotation(int x, int y) const
{
    const char *wpieces = { "0PNBRQK" };
    const char *bpieces = { "0pnbrqk" };
    Bit *bit = _grid->getSquare(x, y)->bit();
    char notation = '0';
    if (bit) {
        notation = bit->gameTag() < 128 ? wpieces[bit->gameTag()] : bpieces[bit->gameTag()-128];
    }
    return notation;
}

Bit* Chess::PieceForPlayer(const int playerNumber, ChessPiece piece)
{
    const char* pieces[] = { "pawn.png", "knight.png", "bishop.png", "rook.png", "queen.png", "king.png" };

    Bit* bit = new Bit();
    // should possibly be cached from player class?
    const char* pieceName = pieces[piece - 1];
    std::string spritePath = std::string("") + (playerNumber == 0 ? "w_" : "b_") + pieceName;
    bit->LoadTextureFromFile(spritePath.c_str());
    bit->setOwner(getPlayerAt(playerNumber));
    bit->setSize(pieceSize, pieceSize);

    return bit;
}

void Chess::setUpBoard()
{
    setNumberOfPlayers(2);
    _gameOptions.rowX = 8;
    _gameOptions.rowY = 8;

    _grid->initializeChessSquares(pieceSize, "boardsquare.png");
    FENtoBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    

    startGame();
}

void Chess::FENtoBoard(const std::string& fen) {
    // convert a FEN string to a board
    // FEN is a space delimited string with 6 fields
    // 1: piece placement (from white's perspective)
    // NOT PART OF THIS ASSIGNMENT BUT OTHER THINGS THAT CAN BE IN A FEN STRING
    // ARE BELOW
    // 2: active color (W or B)
    // 3: castling availability (KQkq or -)
    // 4: en passant target square (in algebraic notation, or -)
    // 5: halfmove clock (number of halfmoves since the last capture or pawn advance)
    _grid->forEachSquare([](ChessSquare* square, int x, int y){
        square->setBit(nullptr);
    });
    
    int row = 7, col = 0;
    for(char symbol : fen){
        
        if(symbol ==  ' '){
            break;
        }
        if (symbol == '/') {
            row--;
            col = 0;
        }
        else  if (symbol >= '1' && symbol <= '8') {
            col += symbol - '0';
        }
        else {

            ChessPiece p;
            switch (toupper(symbol)) {
            case 'P': p = Pawn; break;    
            case 'K': p = King; break;
            case 'N': p = Knight; break;
            case 'B': p = Bishop; break;
            case 'R': p = Rook; break;
            case 'Q': p = Queen; break;
            default: break;
            }

            int playerNumber = isupper(symbol) ? 0 : 1;
            Bit* piece = PieceForPlayer(playerNumber, p);
            ChessSquare* square = _grid->getSquare(col, row);
            piece->setGameTag(isupper(symbol) ? p : (p + 128));
            piece->setPosition(square->getPosition());
            square->setBit(piece);
            

            col++;
        }
    }
    
}

bool Chess::actionForEmptyHolder(BitHolder &holder)
{
    return false;
}

bool Chess::canBitMoveFrom(Bit &bit, BitHolder &src)
{
    // need to implement friendly/unfriendly in bit so for now this hack
    int currentPlayer = getCurrentPlayer()->playerNumber() * 128;
    int pieceColor = bit.gameTag() & 128;
    if (pieceColor == currentPlayer) return true;
    
    // ChessSquare* square = (ChessSquare *)&src;
    // int squareIndex = square->getSquareIndex();
    // for(auto move : _moves){
    //     if(move.from == squareIndex){
    //         return true;
    //     }
    // }
    return false;
}

bool Chess::canBitMoveFromTo(Bit &bit, BitHolder &from, BitHolder &to)
{
    ChessSquare* fromSquare = dynamic_cast<ChessSquare*>(&from);
    ChessSquare* toSquare = dynamic_cast<ChessSquare*>(&to);

    if(!fromSquare || !toSquare) return false;

    std::string state = stateString();
    char color = (bit.gameTag() < 128) ? 'W' : 'B';

    auto moves = generateMoves(state.c_str(), color);

    int fromIndex = fromSquare->getRow() * 8 + fromSquare->getColumn();
    int toIndex = toSquare->getRow() * 8 + toSquare->getColumn();

    for(auto &move : moves){
        if(move.from == fromIndex && move.to == toIndex) return true;
    }
    return false;

    return true;
}

void Chess::stopGame()
{
    _grid->forEachSquare([](ChessSquare* square, int x, int y) {
        square->destroyBit();
    });
}

Player* Chess::ownerAt(int x, int y) const
{
    if (x < 0 || x >= 8 || y < 0 || y >= 8) {
        return nullptr;
    }

    auto square = _grid->getSquare(x, y);
    if (!square || !square->bit()) {
        return nullptr;
    }
    return square->bit()->getOwner();
}

Player* Chess::checkForWinner()
{
    return nullptr;
}

bool Chess::checkForDraw()
{
    return false;
}

std::string Chess::initialStateString()
{
    return stateString();
}

std::string Chess::stateString()
{
    std::string s;
    s.reserve(64);
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
            s += pieceNotation( x, y );
        }
    );
    return s;}

void Chess::setStateString(const std::string &s)
{
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        int index = y * 8 + x;
        char playerNumber = s[index] - '0';
        if (playerNumber) {
            square->setBit(PieceForPlayer(playerNumber - 1, Pawn));
        } else {
            square->setBit(nullptr);
        }
    });
}

bool isValid(int x, int y) {
    return (x >= 0 && x < 8 && y >= 0 && y < 8);
}

void Chess::knightMoves(const char *state, std::vector<BitMove>& moves){
    std::pair<int, int> knightOffsets[] = {
        {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
        {1, 2}, {1, -2}, {-1, 2}, {-1, -2}
    };

    char knightPiece = getCurrentPlayer()->playerNumber() == 0 ? 'N' : 'n';

    for(int y = 0; y<_gameOptions.rowY; ++y){
        for(int x = 0; x<_gameOptions.rowX; ++x){
            int index = y * _gameOptions.rowX + x;
            if(state[index] == knightPiece){
                for(auto& offset : knightOffsets){
                    int newX = x + offset.first;
                    int newY = y + offset.second;
                    if(isValid(newX, newY)){
                        addMove(state, moves, y, x, newY, newX);
                    }
                }
            }
            index++;
        }
    }
}


void Chess::pawnMoves(const char *state, std::vector<BitMove>& moves, int row, int col, int colorAsInt){
    int direction = (colorAsInt == 1) ? 1 : -1;
    int startRow = (colorAsInt == 1) ? 1 : 6;
    
    int nextRow = row + direction;

    if (isValid(col, nextRow) && state[nextRow * 8 + col] == '0' ) {
        addMove(state, moves, row, col, nextRow, col);
        
        if (row == startRow) {
            int doubleRow = row + 2 * direction;
            if (isValid(col, doubleRow) && state[doubleRow * 8 + col] == '0') {
                addMove(state, moves, row, col, doubleRow, col);
            }
        }
     }

    for( int i = -1; i <=1; i +=2){
        int newCol = col + i;
        int index = state[nextRow * 8 + newCol];
        if(isValid(newCol, nextRow) && index != '0' && (index < '0' || index > '9')){
            addMove(state, moves, row, col, nextRow, newCol);
        }
    }
    
}

void Chess::kingMoves(const char *state, std::vector<BitMove>& moves){
    std::pair<int, int> kingOffsets[] = {
        {-1, -1}, {-1, 0}, {-1, 1}, 
        {0, -1}, {0, 1},
        {1, -1}, {1, 0}, {1, 1}
    };

    char kingPiece = getCurrentPlayer()->playerNumber() == 0 ? 'K' : 'k';

    for(int y = 0; y<_gameOptions.rowY; ++y){
        for(int x = 0; x<_gameOptions.rowX; ++x){
            int index = y * _gameOptions.rowX + x;
            if(state[index] == kingPiece){
                for(auto& offset : kingOffsets){
                    int newX = x + offset.first;
                    int newY = y + offset.second;
                    if(isValid(newX, newY)){
                        addMove(state, moves, y, x, newY, newX);
                    }
                }
            }
            index++;
        }
    }
}

void Chess::rookMoves(const char *state, std::vector<BitMove>& moves, int row, int col){
    int rookOffsets[4][2] = {
        {-1, 0}, {1, 0}, {0, -1}, {0, 1}
    };
    

    for(int i = 0; i < 4; ++i){
        int dr = rookOffsets[i][0];
        int dc = rookOffsets[i][1];

        for(int j = 1; j < 8; ++j){
            int endRow = row + j * dr;
            int endCol = col + j * dc;

            if(isValid(endCol, endRow)){
                addMove(state, moves, row, col, endRow, endCol);
                if(state[endRow * 8 + endCol] != '0'){
                    break;
                }
            }
        }
    }
}

void Chess::bishopMoves(const char *state, std::vector<BitMove>& moves, int row, int col){
    int bishopOffsets[4][2] ={
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
    };
    for(int i = 0; i < 4; ++i){
        int dr = bishopOffsets[i][0];
        int dc = bishopOffsets[i][1];

        for(int j = 1; j < 8; ++j){
            int endRow = row + j * dr;
            int endCol = col + j * dc;

            if(isValid(endCol, endRow)){
                addMove(state, moves, row, col, endRow, endCol);
                if(state[endRow * 8 + endCol] != '0'){
                    break;
                }
            }
        }
    }

}

void Chess::queenMoves(const char *state, std::vector<BitMove>& moves, int row, int col){
    int queenOffsets[8][2] ={
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1},
        {-1, 0}, {1, 0}, {0, -1}, {0, 1}
    };
    for(int i = 0; i < 8; ++i){
        int dr = queenOffsets[i][0];
        int dc = queenOffsets[i][1];

        for(int j = 1; j < 8; ++j){
            int endRow = row + j * dr;
            int endCol = col + j * dc;

            if(isValid(endCol, endRow)){
                addMove(state, moves, row, col, endRow, endCol);
                if(state[endRow * 8 + endCol] != '0'){
                    break;
                }
            }
        }
    }
}

void Chess::addMove(const char *state, std::vector<BitMove>& moves, int fromRow, int fromCol, int toRow, int toCol){
    char piece = state[fromRow * 8 + fromCol];
    if (piece == '0') return;
    moves.emplace_back(fromRow * 8 + fromCol, toRow * 8 + toCol, static_cast<ChessPiece>(toupper(piece) - '0'));

}


std::vector<BitMove> Chess::generateMoves(const char *state, char color){
    std::vector<BitMove> moves;
    int colorAsInt = (color == 'W') ? 1 : -1;
    for( int i = 0; i < 64; ++i){
        int row = i / 8;
        int col = i % 8;
        char piece = state[i];
        int pieceColor = (piece == '0') ? 0 : (piece < 'a') ? 1 : -1;
        if(pieceColor == colorAsInt){
            if (piece > 'a') piece = piece - ('a' - 'A');
            switch(piece){
            case 'P': pawnMoves(state, moves, row, col, colorAsInt); break;
            case 'N': knightMoves(state, moves); break;
            case 'K': kingMoves(state, moves); break;
            case 'R': rookMoves(state, moves, row, col); break;
            case 'B': bishopMoves(state, moves, row, col); break;
            case 'Q': queenMoves(state, moves, row, col); break;
            }

        }
    }
    
    

    return moves;
}

