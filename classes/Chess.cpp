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

void Chess::knightMoves(const char *state, std::vector<BitMove>& moves, int row, int col){
    const int knightOffsets[8][2] = {
        {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
        {1, 2}, {1, -2}, {-1, 2}, {-1, -2}
    };

    for(auto &c : knightOffsets){
        int newRow = row + c[0];
        int newCol = col + c[1];

        if(isValid(newCol, newRow)){
            addMove(state, moves, row, col, newRow, newCol);
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

    for (int i = -1; i <= 1; i += 2) {
        int targetCol = col + i;
        if (isValid(targetCol, nextRow)) {
            char targetPiece = state[nextRow * 8 + targetCol];
            if (targetPiece != '0') {
                bool isEnemy = (colorAsInt == 1) ? (targetPiece >= 'a') : (targetPiece < 'a');
                if (isEnemy) {
                    addMove(state, moves, row, col, nextRow, targetCol);
                }
            }
        }
    }
    
}

void Chess::kingMoves(const char *state, std::vector<BitMove>& moves, int row, int col){
    const int kingOffsets[8][2] = {
        {-1, -1}, {-1, 0}, {-1, 1}, 
        {0, -1}, {0, 1},
        {1, -1}, {1, 0}, {1, 1}
    };

    for(auto &c : kingOffsets){
        int newRow = row + c[0];
        int newCol = col + c[1];

        if(isValid(newCol,newRow)){
            addMove(state,moves,row,col,newRow,newCol);
        }
    }
}

void Chess::rookMoves(const char *state, std::vector<BitMove>& moves, int row, int col){
    int rookOffsets[4][2] = {
        {-1, 0}, {1, 0}, {0, -1}, {0, 1}
    };

    for(int i = 0; i < 4; ++i) {
        for(int j = 1; j < 8; ++j) {
            int endRow = row + j * rookOffsets[i][0];
            int endCol = col + j * rookOffsets[i][1];

            if(isValid(endCol, endRow)) {
                if (!addMove(state, moves, row, col, endRow, endCol)) {
                    break;
                }
            } else {
                break; 
            }
        }
    }
}

void Chess::bishopMoves(const char *state, std::vector<BitMove>& moves, int row, int col){
    int bishopOffsets[4][2] ={
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
    };
    for(int i = 0; i < 4; ++i) {
        for(int j = 1; j < 8; ++j) {
            int endRow = row + j * bishopOffsets[i][0];
            int endCol = col + j * bishopOffsets[i][1];

            if(isValid(endCol, endRow)) {
                if (!addMove(state, moves, row, col, endRow, endCol)) {
                    break;
                }
            } else {
                break;
            }
        }
    }

}

void Chess::queenMoves(const char *state, std::vector<BitMove>& moves, int row, int col){
    int queenOffsets[8][2] ={
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1},
        {-1, 0}, {1, 0}, {0, -1}, {0, 1}
    };
    for(int i = 0; i < 8; ++i) {
        for(int j = 1; j < 8; ++j) {
            int endRow = row + j * queenOffsets[i][0];
            int endCol = col + j * queenOffsets[i][1];

            if(isValid(endCol, endRow)) {

                if (!addMove(state, moves, row, col, endRow, endCol)) {
                    break;
                }
            } else {
                break; 
            }
        }
    }
}

bool Chess::addMove(const char *state, std::vector<BitMove>& moves, int fromRow, int fromCol, int toRow, int toCol){
    char piece = state[fromRow * 8 + fromCol];
    char dest = state[toRow * 8 + toCol];

    if (dest == '0') {
        moves.emplace_back(fromRow * 8 + fromCol, toRow * 8 + toCol, static_cast<ChessPiece>(toupper(piece)));
        return true; 
    }
    bool sameColor = ((piece < 'a') == (dest < 'a'));
    if (!sameColor) {
        moves.emplace_back(fromRow * 8 + fromCol, toRow * 8 + toCol, static_cast<ChessPiece>(toupper(piece)));
    }
    return false;
}


std::vector<BitMove> Chess::generateMoves(const char *state, char color){
    std::vector<BitMove> moves;
    moves.reserve(128);

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
            case 'N': knightMoves(state, moves, row, col); break;
            case 'K': kingMoves(state, moves, row, col); break;
            case 'R': rookMoves(state, moves, row, col); break;
            case 'B': bishopMoves(state, moves, row, col); break;
            case 'Q': queenMoves(state, moves, row, col); break;
            }

        }
    }
    return moves;
}

//assign point values to pieces
const int pieceValues[] ={
    0,
    100, //pawn
    320, //knight
    330, //bishop
    500, //rook
    900, //queen
    100000, //king
};

//helper to get pieces
int getPieceIndex(char c) {
    switch(toupper(c)) {
        case 'P': return 1;
        case 'N': return 2;
        case 'B': return 3;
        case 'R': return 4;
        case 'Q': return 5;
        case 'K': return 6;
        default: return 0;
    }
}

bool Chess::testForTerm(const std::string &state){
    //end state
    //king captured
    //if a king does not exist on board
    //end game
    bool wKing = false, bKing = false;
    for(char c : state){
        if(c == 'K') wKing = true;
        if(c == 'k') bKing = true;
    }
    return !wKing || !bKing;
}

int Chess::AIBoardEval(const std::string &state){
    //state of board
    //give score based on what move
    int score = 0;

    //bias for center
    const int centerBonus[64] = {
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  5, 10, 10, 10, 10,  5,  0,
        0, 10, 20, 20, 20, 20, 10,  0,
        0, 10, 20, 30, 30, 20, 10,  0,
        0, 10, 20, 30, 30, 20, 10,  0,
        0, 10, 20, 20, 20, 20, 10,  0,
        0,  5, 10, 10, 10, 10,  5,  0,
        0,  0,  0,  0,  0,  0,  0,  0
    };

    for(int i = 0; i < 64; ++i){
        char c = state[i];
        if(c == '0') continue;
        int pieceIndex = getPieceIndex(c);
        int pieceValue = pieceValues[pieceIndex] + centerBonus[i]; 
        if( c < 'a') score -= pieceValue; //w
        else score += pieceValue; //b
    }
    return score;

}

int Chess::negamax(std::string &state, int depth, int alpha, int beta, int playerColor){
    //recursive call for ai
    //check for end
    if(depth == 0 || testForTerm(state)){
        return AIBoardEval(state) * playerColor;
    }

    int bestVal = -99999;

    char color = (playerColor == 1) ? 'W' : 'B';
    auto moves = generateMoves(state.c_str(), color);
    if(moves.empty()){
        return AIBoardEval(state) * playerColor;
    }

    for(auto &move : moves){
        //save both pieces
        char movingPiece = state[move.from];
        char capturedPiece = state[move.to];
        //make move
        state[move.to] = movingPiece;
        state[move.from] = '0';

        int val = -negamax(state, depth - 1, -beta, -alpha, -playerColor);
        //undo move
        state[move.from] = movingPiece;
        state[move.to] = capturedPiece;

        if(val > bestVal){
            bestVal = val;
        }
        alpha = std::max(alpha, val);
        if(alpha >= beta){
            break;
        }
    }

    return bestVal;
}

void Chess::updateAI(){
    std::string state = stateString();
    auto moves = generateMoves(state.c_str(), 'B');

    int bestVal = -99999;
    BitMove bestMove = moves[0];
    if(moves.empty()){
            return;
        }

    for(auto &move: moves){
        char movingPiece = state[move.from];
        char capturedPiece = state[move.to];
        state[move.to] = movingPiece;
        state[move.from] = '0';
        int val = -negamax(state, 4, -99999, 99999, -1);
        state[move.from] = movingPiece;
        state[move.to] = capturedPiece;
        if(val >bestVal){
            bestVal = val;
            bestMove = move;
            }
    }
    ChessSquare* fromSquare = _grid->getSquare(bestMove.from % 8, bestMove.from / 8);
    ChessSquare* toSquare = _grid->getSquare(bestMove.to % 8, bestMove.to / 8);

    Bit* activePiece = fromSquare->bit();
    toSquare->setBit(activePiece);
    fromSquare->setBit(nullptr);
    activePiece->moveTo(toSquare->getPosition());
    endTurn();
}