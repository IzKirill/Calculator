#include <stdio.h>
#include <stdlib.h>
#include "Calculator.h"
#include "Stack.h"
#include "Clear_Buffer.h"

CalcError VM (const char* namefile)
{
	FILE* bytecode = fopen(namefile, "r");

	if (bytecode == nullptr)
	{
		printf("Can not open source file(%s).\n", namefile);
		return ERROROPENFILE;
	}

	Stack PitColculations = {};
	STACK_CTOR(&PitColculations, 3);

	while(!feof(bytecode) && !ferror(bytecode))
	{
		int command = 0;

		if (fscanf(bytecode, "%d", &command) != 1)
		{
			printf("Can not scan your command.\n");
			return NOCOMMAND;
		}

		switch(command)
		{
			case(push):
				{
					Elemt PushElement = 0;
					if (fscanf(bytecode, "%d", &PushElement) != 1)
					{
						printf("Can't scan push element in file.\n");
						return INCORRECTPUSHELEMENT;
					}
					StackPush(&PitColculations, PushElement);
				}
				break;

			case(in):
				{
					Elemt PushElement = 0;
					printf("Input the element.\n");
					while (scanf("%d", &PushElement) != 1)
					{
						ClearStatusBuffer();
						printf("Incorrect number to push. Input correct element.\n");
					}
					StackPush(&PitColculations, PushElement);	
				}
				break;
			
			case(add):
				{
					Elemt FirstElement = 0;
					Eletmt SecondElement = 0;
					StackPop(&PitColculations, &FirstElement);
					StackPop(&PitColculations, &SecondElement);
					StackPush(&PitColculations, FirstElement+SecondElement);
				}
				break;

			case(sub):
				{
					int FirstElement = 0;
					int SecondElement = 0;
					StackPop(&PitColculations, &FirstElement);
					StackPop(&PitColculations, &SecondElement);
					StackPush(&PitColculations, SecondElement-FirstElement);
				}
				break;

			case(mul):
				{
					int FirstElement = 0;
					int SecondElement = 0;
					StackPop(&PitColculations, &FirstElement);
					StackPop(&PitColculations, &SecondElement);
					StackPush(&PitColculations, FirstElement*SecondElement);
				}
				break;

			case(divis):
				{
					int FirstElement = 0;
					int SecondElement = 0;
					StackPop(&PitColculations, &FirstElement);
					StackPop(&PitColculations, &SecondElement);
					StackPush(&PitColculations, SecondElement/FirstElement);
				}
				break;

			case(out):
				{
					int result = 0;
					StackPop(&PitColculations, &result);
					printf("Result: %d\n", result);
				}
				break;

			case(HLT):
				{
					fclose(bytecode);
					StackDtor(&PitColculations);
					return END;
				}
				break;
			
			default:
				{
					printf("I don't know command - %d. Please, correct input file.\n", command);
					return UNKNOWNCOMMAND;
				}
		}	
	}

	if (ferror(bytecode))
	{
		printf("File error.\n");
		return FILERROR;
	}

	return END;
}