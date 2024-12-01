/*
Author: Rui Wang
Class : ECE6122
Last Date Modified : Dec 1st, 2024

Description:
The class and structs of Chess Game
*/

#include "ChessGame.h"

#include <iostream>
#include <sstream>

// Initialize the static board member with default Chess objects
std::vector<std::vector<Chess>> ChessGame::board(
	BOARD_SIZE,
	std::vector<Chess>(BOARD_SIZE, { NoChess, false, -1})
);

// Default constuctor
ChessGame::ChessGame()
{
	// Set up white pieces
	for (int i = 0; i < BOARD_SIZE; ++i) {
		board[1][i] = { Pawn, true, i };  // White pawns
	}
	board[0][0] = { Rook, true, 0 };
	board[0][1] = { Knight, true, 0 };
	board[0][2] = { Bishop, true, 0 };
	board[0][3] = { Queen, true, 0 };
	board[0][4] = { King, true, 0 };
	board[0][5] = { Bishop, true, 1 };
	board[0][6] = { Knight, true, 1 };
	board[0][7] = { Rook, true, 1 };

	// Set up black pieces
	for (int i = 0; i < BOARD_SIZE; ++i) {
		board[6][i] = { Pawn, false, i };  // Black pawns
	}
	board[7][0] = { Rook, false, 0 };
	board[7][1] = { Knight, false, 0 };
	board[7][2] = { Bishop, false, 0 };
	board[7][3] = { Queen, false, 0 };
	board[7][4] = { King, false, 0 };
	board[7][5] = { Bishop, false, 1 };
	board[7][6] = { Knight, false, 1 };
	board[7][7] = { Rook, false, 1 };

	enablePromotion = false;
	pawnPromotion = { {NoChess, false, -1}, {NoChess, false, -1} };
	enableKingCastling = false;
	kingCastlingMovement = { {NoChess, false, -1}, -1, -1, -1, -1 };
	enableEnPassant = false;
	enPassantMovement = { {NoChess, false, -1}, -1, -1, -1, -1 };

	// checkCapture make the checkMovement function works as checkCapture function
	checkCapture = false;	// default checkMovement function 
}

// Dynamic rending
const DualMovement ChessGame::getLastMovement()
{
	if (cMovements.empty())	// error happens
		return { {{NoChess, false, -1}, -1, -1, -1, -1}, { {NoChess, false, -1}, -1, -1, -1, -1} };
	return cMovements[cMovements.size() - 1];
}

// Check whether need to update chess components types and numbers in rending
const bool ChessGame::checkPawnPromotion()
{
	// After enable, turn off enable
	bool result = enablePromotion;
	if (enablePromotion)
		enablePromotion = false;
	return result;
}

// Update chess components types and numbers in rending
const DualChess ChessGame::getPawnPromotion()
{
	return pawnPromotion;
}

// Check whether check a given king
bool ChessGame::doesCheck(bool isKingWhite) 
{
	auto kingPosition = findKingPosition(isKingWhite);

	// If not find king, then return true
	if (kingPosition.first == -1 || kingPosition.second == -1)
		return true;

	// Check whether whether is a potential capture can check king
	return checkPotentialCapture(kingPosition.first, kingPosition.second);
}

// Check whether a given king cannot move any more
bool ChessGame::isDeadlyChecked(bool isKingWhite)
{
	auto kingPosition = findKingPosition(isKingWhite);

	// If not find king, then return true
	if (kingPosition.first == -1 || kingPosition.second == -1)
		return true;

	// Find all positions that start from kingPosition
	auto kingPotientialPositions = findPotentialPositions(kingPosition.first, kingPosition.second);

	// Assume all potential movements can be captured by opponent
	bool captured = true;
	// For all steps, check whether it can be captured by one possible opponent
	for (auto& pos : kingPotientialPositions) 
	{
		int potentialKingRow = pos.first;
		int potentialKingCol = pos.second;
		Chess originalChess = board[potentialKingRow][potentialKingCol];

		// Firstly change the original chess at the board to king chess
		board[potentialKingRow][potentialKingCol] = { King, isKingWhite, 0 };
		board[kingPosition.first][kingPosition.second] = { NoChess, false, -1 };

		// If there is a position that no chess can capture it, then return it is not checked
		if (!checkPotentialCapture(potentialKingRow, potentialKingCol))
			captured = false;

		// Change the king chess back to the original chess
		board[kingPosition.first][kingPosition.second] = { King, isKingWhite, 0 };
		board[potentialKingRow][potentialKingCol] = originalChess;

		// If it exists no capture, then return
		if(!captured) break;
	}

	return captured;
}

// Check whether any of oppoents can capture the chess at the given position
bool ChessGame::checkPotentialCapture(int toRow, int toCol)
{
	// Make checkMovement as checkCapture
	checkCapture = true;

	// Find whthere there is a offender chess can capture the position (toRow, toCol)
	for (int fromRow = 0; fromRow < BOARD_SIZE; fromRow++) {
		for (int fromCol = 0; fromCol < BOARD_SIZE; fromCol++) {
			if (checkMoveValidly(fromRow, fromCol, toRow, toCol)) {
				// Set back to checkMovement
				checkCapture = false;
				return true;
			}
		}
	}
	
	// Set back to checkMovement
	checkCapture = false;
	return false;
}

// Find all possible destiniations given the source positions
std::vector<std::pair<int, int>> ChessGame::findPotentialPositions(int fromRow, int fromCol)
{	
	// Make checkMovement as checkCapture
	checkCapture = true;

	// List all the possible steps that starts from fromRow and fromCol
	std::vector<std::pair<int, int>> potientialPositions;
	for (int toRow = 0; toRow < BOARD_SIZE; toRow++) {
		for (int toCol = 0; toCol < BOARD_SIZE; toCol++) {
			if (checkMoveValidly(fromRow, fromCol, toRow, toCol)) {
				// Set back to checkMovement
				checkCapture = false;
				potientialPositions.emplace_back(toRow, toCol);
			}
		}
	}
	
	// Set back to checkMovement
	checkCapture = false;
	return potientialPositions;
}

// Find king's position through its color
std::pair<int, int> ChessGame::findKingPosition(bool isKingWhite)
{
	// Find the checked king
	int kingRow = -1, kingCol = -1;
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (board[i][j].isWhite == isKingWhite && board[i][j].cType == King) {
				kingRow = i; kingCol = j; break;
			}
		}
	}
	return std::make_pair(kingRow, kingCol);
}

// Move chess according to the given command and color
bool ChessGame::moveChess(std::string moveCmd, bool userColor)
{
	if (moveCmd.size() != 4)
		return false;

	int fromCol = moveCmd[0] - 'a'; // Must be 'a' to 'h', and convert it to 0 to 7
	int fromRow = moveCmd[1] - '1'; // Must be '1' to '8', and convert it to 0 to 7
	int toCol = moveCmd[2] - 'a';
	int toRow = moveCmd[3] - '1';
	if (fromRow < 0 || fromRow > BOARD_SIZE - 1 ||
		fromCol < 0 || fromCol > BOARD_SIZE - 1 ||
		toRow < 0 || toRow > BOARD_SIZE - 1 ||
		toCol < 0 || toCol > BOARD_SIZE - 1 ||
		fromCol == toCol && fromRow == toRow)
		return false;

	// Move opponent's chess  
	if (board[fromRow][fromCol].isWhite != userColor)
		return false;

	// Validate chess movement
	if (!checkMoveValidly(fromRow, fromCol, toRow, toCol))
		return false;

	// Record the movement
	recordDualMovement(fromRow, fromCol, toRow, toCol);

	// Movement is valid
	board[toRow][toCol] = board[fromRow][fromCol];
	board[fromRow][fromCol] = { NoChess, false, -1 }; // Clear the source square
	
	// En Passant is enabled
	if (enableEnPassant)
		dealEnPassant();

	// King Castle is enabled
	if (enableKingCastling)
		dealKingCastling();

	// Judge whether it is pawn and arrives the bottom of board
	int bottomRow = board[toRow][toCol].isWhite ? BOARD_SIZE - 1 : 0;
	// Pawn promotion is enabled
	if (board[toRow][toCol].cType == Pawn && toRow == bottomRow)
		dealPawnPromotion(toRow, toCol);

	return true;
}

// Process the pawn's promotion to other types of chess
void ChessGame::dealPawnPromotion(int toRow, int toCol)
{
	Chess pawn = board[toRow][toCol];
	std::string strCmd;
	int cmd;

	// lamda function here to generate a new chess type from pawn
	auto promotesTo = [pawn](ChessType cType)
		{
			int sum = 0;
			for (auto& row : board)
				for (auto& c : row)
					if (c.cType == cType && c.isWhite == pawn.isWhite)
						sum++;
			Chess promotedChess = { cType, pawn.isWhite, sum };
			return promotedChess;
		};

	while (true)
	{
		std::cout << "**************************Pawn Promotion*************************" << std::endl;
		std::cout << "1. Rook\n2. Knight\n3. Bishop\n4. Queen\nPlease select a promotion type by typing an integer between 1 to 4: ";
		std::getline(std::cin, strCmd);
		std::stringstream ss(strCmd);
		ss >> cmd;
		if (ss.fail() || cmd < 1 || cmd > 5) {
			std::cout << "Invalid input promotion type" << std::endl;
			continue;
		}

		// Now apply promotion to board
		switch (cmd)
		{
		case 1:
			board[toRow][toCol] = promotesTo(Rook);
			break;
		case 2:
			board[toRow][toCol] = promotesTo(Knight);
			break;
		case 3:
			board[toRow][toCol] = promotesTo(Bishop);
			break;
		case 4:
			board[toRow][toCol] = promotesTo(Queen);
			break;
		default:
			// Do nothing
			break;
		}

		enablePromotion = true;
		pawnPromotion = { pawn, board[toRow][toCol] };
		break;
	}
}

// Process the king's castling with rook
void ChessGame::dealKingCastling()
{
	// Update the last cMovement record's destination movement
	cMovements[cMovements.size() - 1].dest = kingCastlingMovement;

	// Move rook
	int castleFromRow = kingCastlingMovement.fromRow;
	int castleFromCol = kingCastlingMovement.fromCol;
	int castleToRow = kingCastlingMovement.toRow;
	int castleToCol = kingCastlingMovement.toCol;
	board[castleToRow][castleToCol] = kingCastlingMovement.chess;	// Move rook
	board[castleFromRow][castleFromCol] = { NoChess, false, -1 };	// Clear the source sqaure

	// Turn off enableKingCastling
	enableKingCastling = false;
}

// Process the pawn's en passant
void ChessGame::dealEnPassant()
{
	// Update the last cMovement record's destination movement
	cMovements[cMovements.size() - 1].dest = enPassantMovement;

	int enPassntFromRow = enPassantMovement.fromRow;
	int enPassntFromCol = enPassantMovement.fromRow;
	board[enPassntFromRow][enPassntFromCol] = { NoChess, false, -1 };	// Clear the captured pawn

	// Turn off enableEnPassant
	enableEnPassant = false;
}

// Record dual movement to update cMovements and help get last movement
void ChessGame::recordDualMovement(int fromRow, int fromCol, int toRow, int toCol)
{
	Movement src = { board[fromRow][fromCol], fromRow, fromCol, toRow, toCol };

	int destToRow, destToCol;
	switch (board[toRow][toCol].cType)
	{
	case Pawn:
	{
		destToRow = board[toRow][toCol].isWhite ? -2 : BOARD_SIZE + 1;
		destToCol = board[toRow][toCol].sequence;
		break;
	}
	case Rook:
	{
		destToRow = board[toRow][toCol].isWhite ? -3 : BOARD_SIZE + 2;
		destToCol = (BOARD_SIZE - 1) * board[toRow][toCol].sequence;
		break;
	}
	case Knight:
	{
		destToRow = board[toRow][toCol].isWhite ? -3 : BOARD_SIZE + 2;
		destToCol = (BOARD_SIZE - 2) * board[toRow][toCol].sequence;
		break;
	}
	case Bishop:
	{
		destToRow = board[toRow][toCol].isWhite ? -3 : BOARD_SIZE + 2;
		destToCol = (BOARD_SIZE - 3) * board[toRow][toCol].sequence;
		break;
	}
	case Queen:
	{
		destToRow = board[toRow][toCol].isWhite ? -3 : BOARD_SIZE + 2;
		destToCol = 3 * (board[toRow][toCol].sequence + 1);
		break;
	}
	case King:
	{
		destToRow = board[toRow][toCol].isWhite ? -3 : BOARD_SIZE + 2;
		destToCol = 4 * (board[toRow][toCol].sequence + 1);
		break;
	}
	default:
	{
		destToRow = -1;
		destToCol = -1;
	}
	}

	Movement dest = { board[toRow][toCol], toRow, toCol, destToRow, destToCol };
	cMovements.push_back({ src, dest });
}

// Validate a movement
bool ChessGame::checkMoveValidly(int fromRow, int fromCol, int toRow, int toCol)
{
	Chess srcChess = board[fromRow][fromCol];
	Chess desChess = board[toRow][toCol];

	// Common Checks
	if (srcChess.cType == NoChess ||	// Validate that a piece exists at the source square
		desChess.cType != NoChess && srcChess.isWhite == desChess.isWhite) // Friendly piece in the destination square
		return false;

	// Check movement based on different types of chess
	switch (srcChess.cType)
	{
	case Pawn:
		return checkPawnMovesValidly(fromRow, fromCol, toRow, toCol);
	case Rook:
		return checkRookMovesValidly(fromRow, fromCol, toRow, toCol);
	case Knight:
		return checkKnightMovesValidly(fromRow, fromCol, toRow, toCol);
	case Bishop:
		return checkBishopMovesValidly(fromRow, fromCol, toRow, toCol);
	case King:
		return checkKingMovesValidly(fromRow, fromCol, toRow, toCol);
	case Queen:
		return checkQueenMovesValidly(fromRow, fromCol, toRow, toCol);
	default:
		return false;
	}
}

// Validate a pawn's movement
bool ChessGame::checkPawnMovesValidly(int fromRow, int fromCol, int toRow, int toCol)
{
	// Has checked that the indexes are valid
	Chess& pawn = board[fromRow][fromCol];
	Chess& target = board[toRow][toCol];

	// Determine movement direction based on color
	int direction = pawn.isWhite ? 1 : -1; // White moves up (+1), Black moves down (-1)

	/*******************************Work as Check Capture*************************************/
	if (checkCapture)
		return std::abs(toCol - fromCol) == 1 && toRow == fromRow + direction && target.cType != NoChess;
	/*******************************************************************************************/

	// Check forward movement valid
	if (toCol == fromCol && target.cType == NoChess) 
	{
		// One-step Forward
		if (toRow - fromRow == direction)
			return true;

		// Twos-step Forward
		if (toRow - fromRow == 2 * direction) 
		{
			bool isAtOriginal = pawn.isWhite && fromRow == 1 || !pawn.isWhite && fromRow == BOARD_SIZE - 2;
			return isAtOriginal && board[fromRow + direction][fromCol].cType == NoChess;
		}
	}

	// Check whether capture diagnoally
	if (std::abs(toCol - fromCol) == 1 && toRow == fromRow + direction)
		return target.cType != NoChess;

	// En Passant
	Movement lastMovement = getLastMovement().src;
	// Check whether last movement is a Pawn Movement
	if (lastMovement.chess.cType == Pawn &&
		// Check whether the last movement is a two-step forward
		std::abs(lastMovement.toRow - lastMovement.fromRow) == 2 &&
		// Check whether the pawn is at the same row of last pawn movement
		std::abs(toCol - fromCol) == 1 && fromRow == lastMovement.toRow &&
		// Check whether capturing diagnoally
		toCol == lastMovement.toCol && toRow == lastMovement.toRow + direction &&
		// Check whether destination chess is NoChess
		target.cType == NoChess) 
	{
		enableEnPassant = true;
		int destToRow = board[toRow][toCol].isWhite ? -2 : BOARD_SIZE + 1;
		int destToCol = board[toRow][toCol].sequence;
		enPassantMovement = { lastMovement.chess, lastMovement.toRow, lastMovement.toCol, destToRow, destToCol };
		return true;
	}
		
	return false;
}

// Validate a rook's movement
bool ChessGame::checkRookMovesValidly(int fromRow, int fromCol, int toRow, int toCol)
{
	// Check if the move is along a row or a column
	if (fromRow != toRow && fromCol != toCol)
		return false;

	// Determine the movement direction and check for obstacles
	int rowStep = toRow > fromRow ? 1 : (toRow < fromRow ? -1 : 0); // Vertical movement
	int colStep = toCol > fromCol ? 1 : (toCol < fromCol ? -1 : 0); // Horizontal movement

	int currentRow = fromRow + rowStep;
	int currentCol = fromCol + colStep;

	while (currentRow != toRow || currentCol != toCol) {
		if (board[currentRow][currentCol].cType != NoChess)
			return false; // Obstacle found
		currentRow += rowStep;
		currentCol += colStep;
	}

	return true;
}

// Validate a knight's movement
bool ChessGame::checkKnightMovesValidly(int fromRow, int fromCol, int toRow, int toCol)
{
	// Calculate the difference in rows and columns
	int rowDiff = std::abs(toRow - fromRow);
	int colDiff = std::abs(toCol - fromCol);

	// Validate the knight's "L" shape movement
	return rowDiff == 2 && colDiff == 1 || rowDiff == 1 && colDiff == 2;
}

// Validate a bishop's movement
bool ChessGame::checkBishopMovesValidly(int fromRow, int fromCol, int toRow, int toCol)
{
	// Calculate the difference in rows and columns
	int rowDiff = std::abs(toRow - fromRow);
	int colDiff = std::abs(toCol - fromCol);

	// Validate that the move is diagonal
	if (rowDiff != colDiff)
		return false;

	// Determine the direction of movement
	int rowStep = toRow > fromRow ? 1 : -1;	// diagnoal moevment
	int colStep = toCol > fromCol ? 1 : -1;

	// Check for obstacles along the path
	int currentRow = fromRow + rowStep;
	int currentCol = fromCol + colStep;
	while (currentRow != toRow && currentCol != toCol) {
		if (board[currentRow][currentCol].cType != NoChess)
			return false; // Obstacle found
		currentRow += rowStep;
		currentCol += colStep;
	}

	return true;
}

// Validate a king's movement
bool ChessGame::checkKingMovesValidly(int fromRow, int fromCol, int toRow, int toCol)
{
	// Check King Castling
	Chess king = board[fromRow][fromCol];
	int bottomLine = king.isWhite ? 0 : BOARD_SIZE - 1;
	if (fromRow == bottomLine && fromCol == 4 && toRow == fromRow)
	{
		// Short translocation
		if (toCol - fromCol == 2)
		{
			Chess dest = board[bottomLine][BOARD_SIZE - 1];
			if (dest.cType == Rook && dest.isWhite == king.isWhite && dest.sequence == 1 && board[toRow][toCol - 1].cType == NoChess)
			{
				/******************************Work as checkCapture**********************************/
				if (!checkCapture) 
				{
					enableKingCastling = true;
					kingCastlingMovement = { dest, bottomLine, BOARD_SIZE - 1, bottomLine, toCol - 1 };
				}
				/************************************************************************************/
				return true;
			}
		}
		// Long translocation
		else if (fromCol - toCol == 2)
		{
			Chess dest = board[bottomLine][0];
			if (dest.cType == Rook && dest.isWhite == king.isWhite && dest.sequence == 0 &&
				board[toRow][toCol + 1].cType == NoChess && board[toRow][toCol - 1].cType == NoChess)
			{
				/******************************Work as checkCapture**********************************/
				if (!checkCapture) 
				{
					enableKingCastling = true;
					kingCastlingMovement = { dest, bottomLine, 0, bottomLine, toCol + 1 };
				}
				/************************************************************************************/
				return true;
			}
		}
	}

	// Calculate the difference in rows and columns
	int rowDiff = std::abs(toRow - fromRow);
	int colDiff = std::abs(toCol - fromCol);
		
	// Validate that the move is within one square
	return rowDiff <= 1 && colDiff <= 1;
}

// Validate a queen's movement
bool ChessGame::checkQueenMovesValidly(int fromRow, int fromCol, int toRow, int toCol)
{
	// Calculate the differences in rows and columns
	int rowDiff = std::abs(toRow - fromRow);
	int colDiff = std::abs(toCol - fromCol);

	// Validate that the move is either diagonal, horizontal, or vertical
	if (rowDiff != colDiff && rowDiff != 0 && colDiff != 0)
		return false;

	// Determine the direction of movement
	int rowStep = toRow > fromRow ? 1 : (toRow < fromRow ? -1 : 0);
	int colStep = toCol > fromCol ? 1 : (toCol < fromCol ? -1 : 0);

	// Check for obstacles along the path
	int currentRow = fromRow + rowStep;
	int currentCol = fromCol + colStep;
	while (currentRow != toRow || currentCol != toCol) {
		if (board[currentRow][currentCol].cType != NoChess)
			return false; // Path is not clear
		currentRow += rowStep;
		currentCol += colStep;
	}

	return true;
}
