#include <stdio.h>
#include "Calculator.h"
#include "Stack.h"

CalcError DizAssembl (const char* namefile)
{
	FILE* CodeASM = fopen("ASMcode.txt", "w");
	FILE* ByteCode = fopen(namefile, "r");
	
	while (!feof(ByteCode) && !ferror(ByteCode))
	{
		int command = 0;

		if (fscanf(ByteCode, "%d", &command) != 1)
		{
			printf("Incorrect input, correct your byte code.\n");
			return ERRORINPUTCOM;
		}
			
		if (command == push)
		{
			Elemt PushElement = 0;
			if (fscanf(ByteCode, "%d", &PushElement) != 1)
			{
				printf("Incorrect byte code. Can not scan push element.\n");
				return NOTENOUGHNUMBERS;
			}

			fprintf(CodeASM, "%s %d\n", "push", PushElement);
		}
		else if (command == in)
		{
			fprintf(CodeASM, "%s\n", "in");
		}
		else if (command == add)
		{
			fprintf(CodeASM, "%s\n", "add");	
		}
		else if (command == sub)
		{
			fprintf(CodeASM, "%s\n", "sub");	
		}
		else if (command == mul)
		{
			fprintf(CodeASM, "%s\n", "mul");	
		}
		else if (command == divis)
		{
			fprintf(CodeASM, "%s\n", "divis");	
		}
		else if (command == out)
		{
			fprintf(CodeASM, "%s\n", "out");	
		}
		else if (command == sqrt)
		{
			fprintf(CodeASM, "%s\n", "sqrt");	
		}
		else if (command == sin)
		{
			fprintf(CodeASM, "%s\n", "sin");	
		}
		else if (command == cos)
		{
			fprintf(CodeASM, "%s\n", "cos");	
		}
		else if (command == HLT)
		{
			fprintf(CodeASM, "%s\n", "HLT");	
		}
		else 
		{
			printf("Unknown command %d.\n", command);
			return UNKNOWNCOMMAND;
		}
	}

	if (ferror(ByteCode))
	{
		printf("File error.\n");
		return FILERROR;
	}

	return END;
}