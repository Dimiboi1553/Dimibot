#include "C:\\Users\\User\\Desktop\\chess-library-master\\include\\chess.hpp"
#include <map>
#include "Eval.h"
#include <algorithm>

using namespace chess;
using namespace std;

int ValueMoves(const Move move,const Board &board,Movelist KillerMoves,Move BestMoveFromIter){
    std::map<PieceType, int> Pieces;

    Pieces[PieceType::PAWN] = 100;
    Pieces[PieceType::KNIGHT] = 300;
    Pieces[PieceType::BISHOP] = 300;
    Pieces[PieceType::ROOK] = 500;
    Pieces[PieceType::QUEEN] = 900;
    Pieces[PieceType::KING] = 0;
    const int killerMoveBonus = 900;
    const int BestMoveBonus = 1000;

    auto killerMoveIt = std::find(KillerMoves.begin(), KillerMoves.end(), move);

    if (move == BestMoveFromIter){
        return BestMoveBonus;
    }

    if (killerMoveIt != KillerMoves.end()) {
        // Assign a higher value to moves in the KillerMoves list
        return killerMoveBonus;
    }

    if (board.isCapture(move)){
        Piece AttackingPiece = board.at(move.from());
        Piece CapturePiece = board.at(move.to());

        return Pieces[CapturePiece.type()] - Pieces[AttackingPiece.type()];
    }
    return 0;
}

bool compareMoves(const Move& a, const Move& b, const Board& board,Movelist KillerMoves,Move BestMoveFromIter) {
    int valueA = ValueMoves(a, board,KillerMoves,BestMoveFromIter);
    int valueB = ValueMoves(b, board,KillerMoves,BestMoveFromIter);

    return valueA > valueB;
}

Movelist OrderMoves(const Movelist& moves, const Board& board,Movelist KillerMoves,Move BestMoveFromIter) {
    Movelist orderedMoves = moves;

    std::sort(orderedMoves.begin(), orderedMoves.end(), [&](const Move& a, const Move& b) {
        return compareMoves(a, b, board,KillerMoves,BestMoveFromIter);
    });

    Movelist orderedMovelist{orderedMoves};

    return orderedMovelist;
}

const int immediateMateScore = 100000;
const int positiveInfinity = 9999999;
const int negativeInfinity = -positiveInfinity;

int QSearch(Board& board, int alpha, int beta,Movelist KillerMoves,Move BestMoveFromIter){
    int StandPat = Eval(board);

    if (StandPat >= beta){
        return beta;
    }
    if (StandPat > alpha){
        alpha = StandPat;
    }

    Movelist Captures;
    movegen::legalmoves<movegen::MoveGenType::CAPTURE>(Captures,board);
    Movelist OrderedCaptures = OrderMoves(Captures,board,KillerMoves,BestMoveFromIter);

    for (const auto& move : OrderedCaptures){
        board.makeMove(move);
        int eval = -QSearch(board,-beta,-alpha,KillerMoves,BestMoveFromIter);
        board.unmakeMove(move);

        if (eval >= beta){
            return beta;
        }

        if (eval > alpha){
            alpha = eval;
        }
    }
    return alpha;
}

pair<int, Move> AlphaBeta(Board& board,int plyFromRoot, int depth, int alpha, int beta,Movelist KillerMoves, std::unordered_map<unsigned long long int,pair<int,int>>& TranspositionTable, Move BestMoveFromIter,int NumExtensions)
{
    if (plyFromRoot > 0){
        alpha = max(alpha, -immediateMateScore + plyFromRoot);
        beta = min(beta,immediateMateScore - plyFromRoot);

        if (alpha >= beta){
            return {alpha, Move()};
        }
    }

    unsigned long long int Zobrist = board.zobrist();
    auto it = TranspositionTable.find(Zobrist);

    if (it != TranspositionTable.end()) {
        int storedAlpha = it->second.first;
        int storedDepth = it->second.second;
        if (storedDepth >= depth) {
            return {storedAlpha, Move()};
        }
    }

    if (depth == 0 || (board.isGameOver().first == GameResultReason::CHECKMATE)){
        return {QSearch(board,alpha,beta,KillerMoves,BestMoveFromIter),Move()};
    }

    Move BestMove{};
    Movelist moves;
    movegen::legalmoves(moves, board);
    Movelist OrderedMoves = OrderMoves(moves,board,KillerMoves,BestMoveFromIter);

    for (const auto& move : OrderedMoves){
        board.makeMove(move);
        int Extension = (NumExtensions <= 2 && board.inCheck()) ? 0.5 : 0;
        int eval = -AlphaBeta(board,plyFromRoot+1,depth-1+Extension,-beta,-alpha,KillerMoves,TranspositionTable,BestMoveFromIter,NumExtensions + Extension).first;
        board.unmakeMove(move);

        if (eval >= beta){
            //Adding Killer Moves
            //Killer Moves are moves that cause the beta cutoff and if they are still in play it's a good idea to look at them
            KillerMoves.add(move);
            TranspositionTable[Zobrist] = {beta,depth};
            return {beta,Move()};
        }

        if (eval > alpha){
            alpha = eval;
            BestMove = move;
        }

    }
    TranspositionTable[Zobrist] = {alpha,depth};
    //cout << "TranspositionTable: Zobrist = " << Zobrist << ", alpha = " << alpha << ", BestMove = " << BestMove << endl;
    return{alpha,BestMove};
}

pair<int, Move> SearchStart(Board& board) {
    Movelist KillerMoves{};
    int BestEval = negativeInfinity;
    Move BestMove;
    //pair<int, Move> result = AlphaBeta(board, 0, 4, negativeInfinity, positiveInfinity, KillerMoves, TranspositionTable, BestMove, 0);
    unordered_map<unsigned long long int, pair<int,int> > TranspositionTable;
    for (int depth = 0; depth < 4; depth++) {
        pair<int, Move> result = AlphaBeta(board, 0, depth+1, negativeInfinity, positiveInfinity, KillerMoves, TranspositionTable, BestMove, 0);
        Move bestMove = result.second;
        int eval = result.first;

        cout << "Depth " << depth + 1 << " - Eval: " << eval << ", Best Move: " << bestMove << endl;
        if (bestMove != Move()) {
            BestEval = eval;
            BestMove = bestMove;
        }
    }
//    Move bestMove = result.second;
//    int eval = result.first;
//    BestEval = eval;
//    BestMove = bestMove;
    return {BestEval, BestMove};
}
