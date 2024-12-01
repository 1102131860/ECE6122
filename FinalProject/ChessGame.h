/*
Author: Rui Wang
Class : ECE6122
Last Date Modified : Dec 1st, 2024

Description:
The class and structs of Chess Game
*/

#ifndef CHESS_GAME_H
#define GHESS_GAME_H

#include <string>
#include <vector>
#include <utility>

#define BOARD_SIZE 8

// Define chess Type
typedef enum ChessType
{
	NoChess = 0,
	Pawn = 1,
	Rook = 2,
	Knight = 3,
	Bishop = 4,
	King = 5,
	Queen = 6
} ChessType;

// Define Chess struct
typedef struct Chess 
{
	ChessType cType;
	bool isWhite;
	int sequence;
} Chess;

// Define a dual chess struct 
typedef struct DualChess
{
	Chess pawn, promotedChess;
} DualChess;

// Define a movement
typedef struct Movement
{
	Chess chess;
	int fromRow, fromCol, toRow, toCol;
} Movement;

// Define a dual movement
typedef struct DualMovement 
{
	Movement src, dest;
} DualMovement;

class ChessGame
{
public:
	// Default constuctor
	ChessGame();
	// Dynamic rending
	const DualMovement getLastMovement();
	// Check whether need to update chess components types and numbers in rending
	const bool checkPawnPromotion();
	// Update chess components types and numbers in rending
	const DualChess getPawnPromotion();
	// Move chess according to the given command and color
	bool moveChess(std::string moveCmd, bool userColor);
	// Check whether check a given king
	bool doesCheck(bool isKingWhite);
	// Check whether a given king cannot move any more
	bool isDeadlyChecked(bool isKingWhite);

private:
	// Find king's position through its color
	std::pair<int, int> findKingPosition(bool isKingWhite);
	// Find all possible destiniations given the source positions
	std::vector<std::pair<int, int>> findPotentialPositions(int fromRow, int fromCol);
	// Check whether any of oppoents can capture the chess at the given position
	bool checkPotentialCapture(int toRow, int toCol);

	// Record dual movement to update cMovements and help get last movement
	void recordDualMovement(int fromRow, int fromCol, int toRow, int toCol);
	// Process the pawn's promotion to other types of chess
	void dealPawnPromotion(int toRow, int toCol);
	// Process the king's castling with rook 
	void dealKingCastling();
	// Process the pawn's en passant
	void dealEnPassant();

	// Validate a movement
	bool checkMoveValidly(int fromRow, int fromCol, int toRow, int toCol);
	// Validate a pawn's movement
	bool checkPawnMovesValidly(int fromRow, int fromCol, int toRow, int toCol);
	// Validate a rook's movement
	bool checkRookMovesValidly(int fromRow, int fromCol, int toRow, int toCol);
	// Validate a knight's movement
	bool checkKnightMovesValidly(int fromRow, int fromCol, int toRow, int toCol);
	// Validate a bishop's movement
	bool checkBishopMovesValidly(int fromRow, int fromCol, int toRow, int toCol);
	// Validate a king's movement
	bool checkKingMovesValidly(int fromRow, int fromCol, int toRow, int toCol);
	// Validate a queen's movement
	bool checkQueenMovesValidly(int fromRow, int fromCol, int toRow, int toCol);

	static std::vector<std::vector<Chess>> board;
	std::vector<DualMovement> cMovements;
	bool enablePromotion;
	DualChess pawnPromotion;
	bool enableKingCastling;
	Movement kingCastlingMovement;
	bool enableEnPassant;
	Movement enPassantMovement;
	bool checkCapture;	// Decide whether function checkMoveValidly is checkCaptureValidly
};

#endif // !CHESS_GAME_H
