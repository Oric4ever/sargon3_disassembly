# sargon3_disassembly

In order to do an [unofficial port of Sargon III for the Oric](https://github.com/Oric4ever/oric_sargon3), a disassembly of the Apple II version was needed.

Disclaimer : the listing is not fully commented, I only commented some parts for my understanding...

Basically, the move generator is fully commented because I needed to do implement a move generator that generates the move in exactly the same order in order to decode the Openings Library
(a move in the Openings Library is coded in a single byte, with a 6 bit numerical value which is the index of the move when generating the moves in a given position).

Two quick & dirty C programs are provided here, they are just my Openings Book tools to help rebuilding the whole library... 
