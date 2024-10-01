# sargon3_disassembly

In order to do an unofficial port of Sargon III to the Oric, I had to do a disassembly of the Apple II version.

Disclaimer : the listing is not fully commented, I only commented some parts for my understanding...

Basically, the move generator is fully commented because I needed to emulate it very precisely in order to decode the Openings Library
(a move in the Openings Library is coded in a single byte, with a 6 bit numerical value which is the index of the move when generating the moves
in a given position).

A few quick & dirty C programs are provided here, they are just some Openings Book tools to help rebuilding the whole library... 
