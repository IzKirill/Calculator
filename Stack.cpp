#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <stdint.h>
#include "Stack.h"
#include <cstddef>

static error ReallocStackData(Stack* Stk);
static Error CheckHash(Stack* Stk);
static Error CheckCanary(Stack* Stk);
static error AddCanary(Stack* Stk);
static error ChangeHashStk(Stack* Stk);
static error ChangeHashData(Stack* Stk);
static HashType AddHash (const void* PtrOnBegin, const size_t Size);

FILE* LogFile = 0;

error StackCtor(Stack* Stk, const size_t Capacity, const char* Name,
                const size_t birth_line, const char* birth_file, const char* birth_function)
{
    LogFile = fopen(NameLogFile, "w");

    CHECKCONDITION(Stk == NULL, NULL_STR_STK,  " ERROR: StackCtor: Incorrect value of adress stack massive == NULL.\n");
    CHECKCONDITION(Capacity <= 0, INCORRECT_CAPACITY, " ERROR: StackCtor: Stack Incorrect value of Capacity.\n");

    Stk->isStackCtor = true;               

    Stk->StackInfo.stk_name = Name;
    Stk->StackInfo.birth_function = birth_function;
    Stk->StackInfo.birth_line = birth_line;
    Stk->StackInfo.birth_file = birth_file;

    Stk->Size = 0;
    Stk->Capacity = Capacity;
    
    Elemt* MassiveData = (Elemt*) calloc(2*sizeof(CanaryType) + Capacity*sizeof(Elemt)
                                        + sizeof(HashType), sizeof(char));
    CHECKCONDITION(MassiveData == NULL, NO_DIN_MEMORY, "ERROR: StackCtor: Stack Allocation failure.\n");
                          // if release
    Stk->Data = (Elemt*) ((char*) MassiveData + sizeof(CanaryType));

    AddCanary(Stk);

    ChangeHashStk(Stk);
    ChangeHashData(Stk);
    
    if(STACKOK(Stk) != OK) return ERROR;

    return OK;
}

error StackPush (Stack* Stk, Elemt Value)
{
    CHECKCONDITION(Stk == NULL, NULL_STR_STK,  " ERROR: StackPush: Incorrect value of adress stack massive == NULL.\n");
    CHECKCONDITION(!Stk->isStackCtor, STACK_NOT_CTOR,  " ERROR: StackPush: Stack does not created. Use funcion StackCtor first.\n");
    CHECKCONDITION(Stk->isStackDtor, STACK_DTOR,  " ERROR: StackDtor: Stack is already deleted.\n");

    if(STACKOK(Stk) != OK) return ERROR;

    if (Stk->Size == Stk->Capacity)
    {
        Stk->Capacity *= 2;
        ReallocStackData(Stk);

        for (size_t NElemnt = Stk->Size; NElemnt < Stk->Capacity; NElemnt++)
            Stk->Data[NElemnt] = 999995; // что то по интереснее 
    }

    Stk->Data[Stk->Size++] = Value;

    ChangeHashStk(Stk);
    ChangeHashData(Stk);

    if(STACKOK(Stk) != OK) return ERROR;

    return OK;
}

error StackPop (Stack* Stk, Elemt* refValue)
{
    CHECKCONDITION(Stk == NULL, NULL_STR_STK,  " ERROR: StackPop: Incorrect value of adress stack massive == NULL.\n");
    CHECKCONDITION(!Stk->isStackCtor, STACK_NOT_CTOR,  " ERROR: StackPop: Stack does not created. Use funcion StackCtor first.\n");
    CHECKCONDITION(Stk->isStackDtor, STACK_DTOR,  " ERROR: StackPop: Stack is already deleted.\n");

    CHECKCONDITION(refValue == NULL, ADRESS_NULL,  " ERROR: StackPop: Stack Incorrect adress of refValue == NULL");

    if(STACKOK(Stk) != OK) return ERROR;

    CHECKCONDITION(Stk->Size == 0, SIZE_EQUAL_ZERO,  " ERROR: StackPop: Stack Size == 0\n");

    if (2*Stk->Size <= Stk->Capacity)
    {
        Stk->Capacity /= 2;

        ReallocStackData(Stk);
    }

    *refValue = Stk->Data[--Stk->Size];
    Stk->Data[Stk->Size] = 999995;

    ChangeHashStk(Stk);
    ChangeHashData(Stk);

    if(STACKOK(Stk) != OK) return ERROR;

    return OK;
}

error StackDtor (Stack* Stk)
{
    CHECKCONDITION(Stk == NULL, NULL_STR_STK,  " ERROR: StackDtor: Stack == NULL\n");
    CHECKCONDITION(!Stk->isStackCtor, NULL_STR_STK,  " ERROR: StackDtor: Stack does not created. Use funcion StackCtor first.\n");
    CHECKCONDITION(Stk->isStackDtor, NULL_STR_STK,  " ERROR: StackDtor: Stack is already deleted.\n");

    if(STACKOK(Stk) != OK) return ERROR;

    for (size_t nelemnt = 0; nelemnt < Stk->Capacity; nelemnt++)
    {
        Stk->Data[nelemnt] = 0;
    }

    Stk->Size = 0;
    Stk->Capacity = 0;


    free((char*)Stk->Data-sizeof(CanaryType));
    Stk->Data = 0;
    
    Stk->StkHash = 0;

    Stk->isStackDtor = true;
    Stk = 0;
    
    return OK;
}

static error ReallocStackData(Stack* Stk)
{
    Elemt* MassiveData = (Elemt*) (realloc((char*) Stk->Data - sizeof(CanaryType),
        sizeof(HashType) + Stk->Capacity*sizeof(Elemt) + 2*sizeof(CanaryType)));

    CHECKCONDITION(MassiveData == NULL, NO_DIN_MEMORY, "ERROR: StackPop: Stack Allocation failure.\n");

    Stk->Data = (Elemt*) ((char*) MassiveData + sizeof(CanaryType));

    AddCanary(Stk);

    return OK;
}

error StackOK(Stack* Stk, const size_t Line,
              const char* NameFile, const char* Func)
{
    CHECKCONDITION(Stk == NULL, NULL_STR_STK, " ERROR: Stack == NULL\n");
    
    short Error = 0;

    if (Stk->Capacity < Stk->Size)
    {
        fprintf(LogFile," %s:%ld: error in function %s: Stack(%s) capacity less stack size: "
                "capacity = %ld, size = %ld\n", NameFile, Line, Func, Stk->StackInfo.stk_name,
                                            Stk->Capacity, Stk->Size);
        Error = Error | (1<<(CAPACITY_LESS_SIZE-1));
    }
    if (Stk-> Capacity == 0)
    {
        fprintf(LogFile," %s:%ld: error in function %s: "
               "Stack(%s) capacity equal zero.\n", NameFile, Line,
                                                Func, Stk->StackInfo.stk_name);
        Error = Error | (1<<(CAPACITY_EQUAL_ZERO-1));
    }
    if (Stk->Data == NULL)
    {
        fprintf(LogFile," %s:%ld: error in function %s: "
               "In the stack(%s) adress of massive equal zero.\n", NameFile, Line,
                                                             Func, Stk->StackInfo.stk_name);
        Error = Error | (1<<(NULL_STACK_DATA-1));
    }

    char StatusCanary = CheckCanary(Stk);
    Error = Error | (StatusCanary<<(STACK_LEFT_CANARY_DESTROY-1));

    char StatusHash = CheckHash(Stk);
    Error = Error | (StatusHash<<(INC_HASH_STACK-1));
    
    if (Error != 0)
    {
        STACK_DUMP(Stk);

        if ((Error & ((1<<(INC_HASH_STACK-1)) + (1<<(INC_HASH_DATA-1)))) > 0)
            exit(INC_HASH);

        Stk->ErrorsInfo = Error;
        return ERROR;
    }
    else
    {
        fprintf(LogFile, " %s: All is good (StackOK) in function %s.\n",NameFile, Func);
        return OK;
    }
}

error StackDump (Stack* Stk, const size_t nline, const char* NameFile, const char* Func)
{  
    CHECKCONDITION(Stk == NULL, NULL_STR_STK, " ERROR: Stack == NULL\n");

    fprintf(LogFile,"\n\nStack[%p] %s from %s(%ld) %s", Stk, Stk->StackInfo.stk_name, Stk->StackInfo.birth_file,
                                          Stk->StackInfo.birth_line, Stk->StackInfo.birth_function);
    fprintf(LogFile," called from %s(%ld) %s\n", NameFile, nline, Func);
    fprintf(LogFile,"  {\n   Size = %ld\n   Capacity = %ld\n   LeftCanary = %0llx\n   RightCanary = %0llx\n   Hash = %llu\n   Data[%p]\n"
    "\t{\n\t", Stk->Size, Stk->Capacity, Stk->left_canary, Stk->right_canary, Stk->StkHash, (void*) Stk->Data);

    if (Stk->Size > 0)
    {
        for (size_t nelemnt = 0; nelemnt < Stk->Size; nelemnt++)
            fprintf(LogFile,"*[%ld]=%d\n\t", nelemnt, Stk->Data[nelemnt]);

        if (Stk->Capacity > 0)
        {
            for (size_t nelemnt = Stk->Size; nelemnt < Stk->Capacity; nelemnt++)
                fprintf(LogFile," [%ld]=%d\n\t", nelemnt, Stk->Data[nelemnt]);
        }
    }

    fprintf(LogFile,"}\n  }\n\n");

    return ERROR;
}

static error AddCanary(Stack* Stk)
{
    CHECKCONDITION(Stk == NULL, NULL_STR_STK,  " ERROR: AddCanary: Stack == NULL\n");

    Stk->left_canary = left_canary;
    Stk->right_canary = right_canary;

    *(CanaryType*)((char*) Stk->Data - sizeof(CanaryType)) = left_canary;
    *(CanaryType*) (Stk->Data+Stk->Capacity) = right_canary;

    return OK;
}

static Error CheckCanary(Stack* Stk)
{
    CHECKCONDITION(Stk == NULL, NULL_STR_STK,  " ERROR: CheckCanary: Stack == NULL\n");

    short NAttacks = 0;

    if (Stk->left_canary != left_canary)
    {
        NAttacks+=1;
        fprintf(LogFile," Attack on Stack(%s)  left canary!!\nCanary left = %0llx\n",
                                Stk->StackInfo.stk_name, Stk->left_canary);
    }
    if (Stk->right_canary != right_canary)
    {
        NAttacks+=2;
        fprintf(LogFile," Attack on Stack(%s) right canary!!\nCanary right = %0llx\n",
                                Stk->StackInfo.stk_name, Stk->right_canary);
    }
    if ((*(CanaryType*)((char*) Stk->Data - sizeof(CanaryType))) != left_canary)
    {
        NAttacks+=4;
        fprintf(LogFile," Attack on Stack(%s) Data massive left canary!!\nCanary left = %0llx\n",
                                  Stk->StackInfo.stk_name,*(CanaryType*)((char*) Stk->Data - sizeof(CanaryType)));  
    } 
    if (*(CanaryType*) ((Elemt*)Stk->Data + Stk->Capacity) != right_canary)
    {
        NAttacks+=8;
        fprintf(LogFile," Attack on Stack(%s) Data massive right canary!!\nCanary right = %0llx\n",
                                  Stk->StackInfo.stk_name,*(CanaryType*) ((Elemt*)Stk->Data+Stk->Capacity));
    }  

    return NAttacks;
}

static HashType AddHash (const void* PtrOnBegin, const size_t Size)
{
    CHECKCONDITION(PtrOnBegin == NULL, NULL_POINTER,  " ERROR: AddHash: Pointer on begin == NULL\n");
    CHECKCONDITION(Size == 0, SIZE_EQUAL_ZERO,  " ERROR: AddHash: Size = 0.\n");

    size_t Hash = 5381;

    for (size_t i = 0; i < Size; i++)
        Hash = ((Hash << 5) + Hash) + ((char*)PtrOnBegin)[i];

    return Hash;
} 

static Error CheckHash(Stack* Stk)
{
    CHECKCONDITION(Stk == NULL, NULL_STR_STK,  " ERROR: CheckHash: Stack == NULL\n");

    char NAttacks = 0;

    HashType PreviousHashStk = Stk->StkHash;
    HashType PreviousHashData = *(HashType*)((char*)Stk->Data+
                                sizeof(Elemt)*Stk->Capacity+sizeof(CanaryType));

    HashType NowHashStk = AddHash(Stk, (sizeof(*Stk) - sizeof(HashType)));
    HashType NowHashData = AddHash((char*)Stk->Data-sizeof(CanaryType), 2*sizeof(CanaryType) + Stk->Capacity*sizeof(Elemt));
    if (PreviousHashStk != NowHashStk)
    {
        NAttacks = 1;
        fprintf(LogFile," ERROR: DDOS ATTACK on %s!!!! MEOW\n", Stk->StackInfo.stk_name);
        fprintf(LogFile,"Previous hash stack = %0llx\nHash stack now = %0llx\n", PreviousHashStk, NowHashStk);
    }

    if (PreviousHashData != NowHashData)
    {
        NAttacks+=2;
        fprintf(LogFile," ERROR: DDOS ATTACK on %s DATA!!!! MEOW\n", Stk->StackInfo.stk_name);
        fprintf(LogFile,"Previous hash stack Data %0llx\nNow hash stack Data %0llx\n", PreviousHashData, NowHashData);
    }

    return NAttacks;
}

static error ChangeHashStk(Stack* Stk)
{
    CHECKCONDITION(Stk == NULL, NULL_STR_STK,  "ERROR: ChangeHashStk: Stack == NULL\n");

    Stk->StkHash =AddHash(Stk, (sizeof(*Stk) - sizeof(HashType)));
    return OK;
}

static error ChangeHashData(Stack* Stk)
{
    CHECKCONDITION(Stk == NULL, NULL_STR_STK,  "ERROR: ChangeHashData: Stack == NULL\n");

    *(HashType*)((char*)Stk->Data+sizeof(Elemt)*Stk->Capacity+
    sizeof(CanaryType)) = AddHash((char*)Stk->Data-sizeof(CanaryType), 2*sizeof(CanaryType) + Stk->Capacity*sizeof(Elemt));
  
    return OK;
}
