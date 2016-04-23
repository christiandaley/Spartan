Readme for Spartan version 1.0

Spartan is an open source UCI compatible chess engine written in C. Spartan is the successor to my earlier chess engine JFresh. A precompiled binary is available for 64-bit mac users. Windows/linux users can compile Spartan themselves using the makefile. To compile on windows use “make win”. Please note that I have not tested Spartan on anything other than my Macbook so I can make no guarantees about its behavior on other machines. Spartan requires POSIX threads (pthreads) to be able to run, so Windows users may need to install the pthread library for windows. If there are any problems with compiling/running Spartan please message me.

Spartan uses Bitboards for the board representation, and Magic Bitboards for move generation. Spartan does not use any form of piece table. Only Bitboards are used for the board representation.

Currently I don’t know the exact playing strength of Spartan, but I do know that it is stronger than JFresh which is a very weak engine. JFresh is terrible at endgames and is prone to draw by repetition even when it is ahead. Spartan plays endgames much better than JFresh does. Also, Spartan seems to have better positional understanding than JFresh, and it has a better search algorithm that allows it to search deeper.

For testing purposes, Spartan has both the Bratko-Kopec test suite and Larry Kaufman CCR 1 Hour Test built into it. To run the Bratko-Kopec test suit simply use the command “bktest [ms]” where [ms] is the allowed search time per position in milliseconds. If no search time is provided, a default of 2 minutes is allowed, meaning it will take 48 minutes to complete the test. To run the Larry Kaufman CCR 1 Hour Test use the command “lktest”. This test takes 50 minutes to complete.

For more information about these tests see the following pages:


https://chessprogramming.wikispaces.com/Bratko-Kopec+Test
https://chessprogramming.wikispaces.com/CCR+One+Hour+Test


Parts of Spartans evaluation function are inspired by the evaluation function of Fruit. In particular, the way that Spartan evaluates king safety and passed pawns is nearly identical to how Fruit does. However, Spartan is not a clone of Fruit.

The search heuristics used in Spartan are:

* Alpha beta pruning with aspiration windows

* PVS search

* Transposition table

* Razoring

* Futility pruning

* Null move pruning

* ProbCut

* History pruning

* Late move reductions

* Killer/History move ordering

* Delta pruning

* Pruning of all losing captures in the quiescence search (while not in check)

* SEE (used for move ordering and pruning)

Commands that Spartan recognizes (other than UCI commands):

* perft [depth] - performs a perft search of the current position to the specified depth

* print - prints all info about the current state of the board. This includes a rendering of the board using text, the FEN string representation of the board, and some other information.

* list - lists all legal moves in the current position using algebraic format

* bktest [ms] - performs the Bratko-Kopec test with a search time of [ms] milliseconds per position

* lktest - performs the Larry Kaufman CCR One Hour Test

* undo - undoes the last move made on the board (if there is a move to undo)

* eval - prints the result of the static evaluation of the board relative to white. Positive values mean an advantage for white. Negative values mean an advantage for black.

Other things to note:

1. As of version 1.0, Spartan does not support pondering. This feature will be added in the future.

2. Spartan keeps a log of all of its communications with the GUI. The log will be written to the working directory of the executable.