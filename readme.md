In Chess.cpp, FentoBoard is used to transcribe a FEN string to a playable board. Castling, en passant and half move clock have not been implemented.

A bool called isValid() checks if a move is within the board. If so, addMove takes the state of the board, moves, the row & column the piece is moving from and the row & column the piece is moving to, and then places the piece accordingly. 

knightMoves(), pawnMoves() and kingMoves() are used to generate movements for knights, pawns and kings respectively. generateMoves() uses switch cases to handle movement generation based on which piece the player picks up.


<img width="1260" height="790" alt="Screenshot 2026-02-26 181408" src="https://github.com/user-attachments/assets/bb7aa9f0-5b51-4852-86eb-ea99d60ac871" />


<img width="1916" height="1032" alt="Screenshot 2026-02-26 190106" src="https://github.com/user-attachments/assets/457c1555-90bc-4d29-bc92-e93b6b5a5018" />
