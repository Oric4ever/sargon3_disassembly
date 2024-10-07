# sargon3_disassembly

In order to do an [unofficial port of Sargon III for the Oric](https://github.com/Oric4ever/oric_sargon3), a disassembly of the Apple II version was needed.

Disclaimer : the listing is not fully commented, I only commented some parts for my understanding...

Basically, the move generator is fully commented because I needed to do implement a move generator that generates the move in exactly the same order in order to decode the Openings Library
(a move in the Openings Library is coded in a single byte, with a 6 bit numerical value which is the index of the move when generating the moves in a given position).

Two quick & dirty C programs are provided here, they are just my Openings Book tools to help rebuilding the whole library... 

The Openings files on the Sargon III disk follow the Encyclopaedia of Chess Openings (ECO) numbering : there are files implementing the 5 volumes of the ECO standard:
- Volume A : BA00, BA10, BA20, BA30, BA40, BA45, BA50, BA60, BA70, BA80, BA90
- Volume B : BB00, BB10, BB20, BB30, BB40, BB50, BB60, BB70, BB80, BB90
- Volume C : BC00, BC10, BC20, BC30, BC40, BC50, BC60, BC70, BC80, BC90
- Volume D : BD00, BD10, BD20, BD30, BD40, BD50, BD60, BD70, BD80, BD90
- Volume E : BE00, BE10, BE20, BE30, BE40, BE50, BE60, BE70, BE80, BE90
... plus a *root* file (B000) whose first moves will select the different ECO files...
