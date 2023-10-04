#ifndef __CALCULATOR_H__
#define __CALCULATOR_H__

const size_t MAX_SIZE_COMMAND = 10;

enum CalcError {
	END = 0,
	ERROROPENFILE = 1,
	NOTENOUGHNUMBERS = 2,
	UNKNOWNCOMMAND = 3,
	INCORRECTPUSHELEMENT = 4,
	FILERROR = 5,
	ERRORINPUTCOM = 6
};

enum Commands {
	HLT = -1,				// end programm
	in = 0,					// push on console
	push = 1,   			//add
	divis = 2,				// /
	sub = 3,				// -
	add = 4, 				// +
	mul = 5,				// *
	out = 6,				// output
	sqrt = 7,				// sqrt
	sin = 8,				// sin
	cos = 9					// cos
};

CalcError VM (const char* namefile);
CalcError Compil (const char* namefile);
CalcError DizAssembl (const char* namefile);

#endif