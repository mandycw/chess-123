#pragma once

#include "Game.h"
#include "Grid.h"
#include "Bitboard.h"

constexpr int pieceSize = 80;

// enum ChessPiece
// {
//     NoPiece,
//     Pawn ,
//     Knight ,
//     Bishop ,
//     Rook,
//     Queen,
//     King
// };

class Chess : public Game
{
public:
    bool gameHasAI() override { return true; }
    Chess();
    ~Chess();

    void setUpBoard() override;
    
    bool canBitMoveFrom(Bit &bit, BitHolder &src) override;
    bool canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst) override;
    bool actionForEmptyHolder(BitHolder &holder) override;
    ;
    void stopGame() override;

    Player *checkForWinner() override;
    bool checkForDraw() override;

    std::string initialStateString() override;
    std::string stateString() override;
    void setStateString(const std::string &s) override;

    Grid* getGrid() override { return _grid; }

    bool addMove(const char *state, std::vector<BitMove>& moves, int fromRow, int fromCol, int toRow, int toCol);
    std::vector<BitMove> generateMoves(const char *state, char color);

    void pawnMoves(const char *state, std::vector<BitMove>& moves, int row, int col, int colorAsInt);
    void knightMoves(const char *state, std::vector<BitMove>& moves, int row, int col);
    void kingMoves(const char *state, std::vector<BitMove>& moves, int row, int col);
    void rookMoves(const char *state, std::vector<BitMove>& moves, int row, int col);
    void bishopMoves(const char *state, std::vector<BitMove>& moves, int row, int col);
    void queenMoves(const char *state, std::vector<BitMove>& moves, int row, int col);
    bool testForTerm(const std::string &state);
    void updateAI();
    int AIBoardEval(const std::string &state);
    int negamax( std::string &state, int depth, int alpha, int beta, int playerColor);

private:
    Bit* PieceForPlayer(const int playerNumber, ChessPiece piece);
    Player* ownerAt(int x, int y) const;
    void FENtoBoard(const std::string& fen);
    char pieceNotation(int x, int y) const;

    Grid* _grid;
};