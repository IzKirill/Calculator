#include <stdio.h>
#include "Calculator.h"
#include <string.h>
#include "Stack.h"

CalcError Compil (const char* namefile)
{
	FILE* CodeASM = fopen(namefile, "r");
	FILE* ByteCode = fopen("ByteCode.txt", "w");
	
	while (!feof(CodeASM) && !ferror(CodeASM))
	{
		char command[MAX_SIZE_COMMAND] = {};

		if (fscanf(CodeASM, "%s", command) != 1)
		{
			printf("Incorrect input, correct your ASM code.\n");
			return ERRORINPUTCOM;
		}
		
		if (!strcmp(command, "push"))
		{
			Elemt PushElement = 0;
			if (fscanf(CodeASM, "%d", &PushElement) != 1)
			{
				printf("Incorrect ASM code. Can not scan push element.\n");
				return NOTENOUGHNUMBERS;
			}

			fprintf(ByteCode, "%d %d\n", push, PushElement);
		}
		else if (!strcmp(command,"in"))
		{
			fprintf(ByteCode, "%d\n", in);
		}
		else if (!strcmp(command,"add"))
		{
			fprintf(ByteCode, "%d\n", add);	
		}
		else if (!strcmp(command,"sub"))
		{
			fprintf(ByteCode, "%d\n", sub);	
		}
		else if (!strcmp(command,"mul"))
		{
			fprintf(ByteCode, "%d\n", mul);	
		}
		else if (!strcmp(command,"divis"))
		{
			fprintf(ByteCode, "%d\n", divis);	
		}
		else if (!strcmp(command,"out"))
		{
			fprintf(ByteCode, "%d\n", out);	
		}
		else if (!strcmp(command,"sqrt"))
		{
			fprintf(ByteCode, "%d\n", sqrt);	
		}
		else if (!strcmp(command,"sin"))
		{
			fprintf(ByteCode, "%d\n", sin);	
		}
		else if (!strcmp(command,"cos"))
		{
			fprintf(ByteCode, "%d\n", cos);	
		}
		else if (!strcmp(command,"HLT"))
		{
			fprintf(ByteCode, "%d\n", HLT);	
		}
		else 
		{
			printf("Unknown command.\n");
			return UNKNOWNCOMMAND;
		}
	}

	if (ferror(CodeASM))
	{
		printf("File error.\n");
		return FILERROR;
	}

	return END;
}