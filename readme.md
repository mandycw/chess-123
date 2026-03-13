In Chess.cpp, FentoBoard is used to transcribe a FEN string to a playable board. Castling, en passant and half move clock have not been implemented.

A bool called isValid() checks if a move is within the board. If so, addMove() takes the state of the board, moves, the row & column the piece is moving from and the row & column the piece is moving to, and then places the piece accordingly. 

knightMoves(), pawnMoves() and kingMoves(), rookMoves(), bishopMoves() and queenMoves() are used to generate movements for knights, pawns, kings, rooks, bishops and queens respectively. generateMoves() uses switch cases to handle movement generation based on which piece the player picks up.

The AI works by using updateAI, testForTerm, AIBoardEval and negamax. testForTerm checks if the game is over, AIBoardEval assigns a score to each piece and gives extra points of being in the center and returns a score. negamax recusively calls itself to find the best move and returns a score. updateAI actually makes the move, using negamax to get the best move. isCheck and isCheckmate is used to check for win conditions. 

A challenge I had was during the implementation of the AI, I had to redesign the way the piece moves were generating and the addMove() function for the AI to properly work. A depth of 4 was achieved and the AI plays like a moderate beginner.  

<img width="1260" height="790" alt="Screenshot 2026-02-26 181408" src="https://github.com/user-attachments/assets/bb7aa9f0-5b51-4852-86eb-ea99d60ac871" />


<img width="1916" height="1032" alt="Screenshot 2026-02-26 190106" src="https://github.com/user-attachments/assets/457c1555-90bc-4d29-bc92-e93b6b5a5018" />


