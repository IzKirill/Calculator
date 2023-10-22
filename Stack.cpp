#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <stdint.h>
#include "Stack.h"
#include <cstddef>

#ifndef RELEASE // избыточно
static Error CheckHash(Stack* stk);
static Error CheckCanary(Stack* stk);
static HashType AddHashStk(Stack* stk);
static HashType AddHashData(Stack* stk);  
static error AddCanary(Stack* stk);
static error ChangeHashStk(Stack* stk);
static error ChangeHashData(Stack* stk);
#endif// избыточно

FILE* LogFile = 0;

error StackCtor(Stack* stk, const size_t Capacity, const char* name,
                const size_t birth_line, const char* birth_file, const char* birth_function)
{
    LogFile = fopen(NameLogFile, "w");

    CHECKCONDITION(stk == NULL, NULL_STR_STK,  " ERROR: StackCtor: Incorrect value of adress stack massive == NULL.\n");
    CHECKCONDITION(Capacity <= 0, INCORRECT_CAPACITY, " ERROR: StackCtor: Stack Incorrect value of Capacity.\n");

    stk->isStackCtor = true;               

    stk->StackInfo.stk_name = name;
    stk->StackInfo.birth_function = birth_function;
    stk->StackInfo.birth_line = birth_line;
    stk->StackInfo.birth_file = birth_file;

    stk->Size = 0;
    stk->Capacity = Capacity;
    stk->data = (Elemt*) ((char*) calloc(2*sizeof(CanaryType)+Capacity*sizeof(Elemt)+sizeof(HashType)
                                , sizeof(char))              // разделить калок и сайзоф 
                          +sizeof(CanaryType)); // if release

    if (stk->data-sizeof(CanaryType) == nullptr)  
    {
       printf("ERROR: StackCtor: Stack(%s) Allocation failure.\n",
                stk->StackInfo.stk_name);
        return NO_DIN_MEMORY; 
    }

    AddCanary(stk);

    AddHashStk(stk);
    AddHashData(stk);

    if(STACKOK(stk) != OK) return ERROR;

    return OK;
}

error StackPush (Stack* stk, Elemt value)
{
    CHECKCONDITION(stk == NULL, NULL_STR_STK,  " ERROR: StackPush: Incorrect value of adress stack massive == NULL.\n");
    CHECKCONDITION(!stk->isStackCtor, STACK_NOT_CTOR,  " ERROR: StackPush: Stack does not created. Use funcion StackCtor first.\n");
    CHECKCONDITION(stk->isStackDtor, STACK_DTOR,  " ERROR: StackDtor: Stack is already deleted.\n");

    if(STACKOK(stk) != OK) return ERROR;

    if (stk->Size == stk->Capacity)
    {
        stk->Capacity *= 2;

        Elemt* massive = (Elemt*) ((char*) realloc((char*) stk->data - sizeof(CanaryType),
        sizeof(HashType)+stk->Capacity*sizeof(Elemt)+2*sizeof(CanaryType))+sizeof(CanaryType));

        if (massive - sizeof(CanaryType) == NULL)
        {
            printf("ERROR: StackPush: Stack(%s) Allocation failure.\n",
                    stk->StackInfo.stk_name);
            return NO_DIN_MEMORY;
        }

        stk->data = massive;

        AddCanary(stk);

        for (size_t nelemnt = stk->Size; nelemnt < stk->Capacity; nelemnt++)
            stk->data[nelemnt] = 0; // что то по интереснее 
    }

    stk->data[stk->Size++] = value;

    ChangeHashStk(stk);
    ChangeHashData(stk);

    if(STACKOK(stk) != OK) return ERROR;

    return OK;
}

error StackPop (Stack* stk, Elemt* refValue)
{
    CHECKCONDITION(stk == NULL, NULL_STR_STK,  " ERROR: StackPop: Incorrect value of adress stack massive == NULL.\n");
    CHECKCONDITION(!stk->isStackCtor, STACK_NOT_CTOR,  " ERROR: StackPop: Stack does not created. Use funcion StackCtor first.\n");
    CHECKCONDITION(stk->isStackDtor, STACK_DTOR,  " ERROR: StackPop: Stack is already deleted.\n");

    CHECKCONDITION(refValue == NULL, ADRESS_NULL,  " ERROR: StackPop: Stack Incorrect adress of refValue == NULL");

    if(STACKOK(stk) != OK) return ERROR;

    CHECKCONDITION(stk->Size == 0, SIZE_EQUAL_ZERO,  " ERROR: StackPop: Stack Size == 0\n");

    if (2*stk->Size <= stk->Capacity)
    {
        stk->Capacity /= 2;
        Elemt* massive = (Elemt*) ((char*) realloc((char*) stk->data - sizeof(CanaryType),
        sizeof(HashType)+stk->Capacity*sizeof(Elemt)+2*sizeof(CanaryType))+sizeof(CanaryType));

        if (massive - sizeof(CanaryType) == NULL)
        {
            printf(" ERROR: StackPush: Stack(%s) Allocation failure.\n",
                    stk->StackInfo.stk_name);
            return NO_DIN_MEMORY;
        }

        stk->data = massive;

        AddCanary(stk);
    }


    *refValue = stk->data[--stk->Size];
    stk->data[stk->Size] = 0;

    ChangeHashStk(stk);
    ChangeHashData(stk);

    if(STACKOK(stk) != OK) return ERROR;

    return OK;
}

error StackDtor (Stack* stk)
{
    CHECKCONDITION(stk == NULL, NULL_STR_STK,  " ERROR: StackDtor: Stack == NULL\n");
    CHECKCONDITION(!stk->isStackCtor, NULL_STR_STK,  " ERROR: StackDtor: Stack does not created. Use funcion StackCtor first.\n");
    CHECKCONDITION(stk->isStackDtor, NULL_STR_STK,  " ERROR: StackDtor: Stack is already deleted.\n");

    if(STACKOK(stk) != OK) return ERROR;

    for (size_t nelemnt = 0; nelemnt < stk->Capacity; nelemnt++)
    {
        stk->data[nelemnt] = 0;
    }

    stk->Size = 0;
    stk->Capacity = 0;


    free((char*)stk->data-sizeof(CanaryType));
    stk->data = 0;
    
    #ifndef RELEASE
    stk->StkHash = 0;
    #endif

    stk->isStackDtor = true;
    stk = 0;

    fclose(LogFile);
    return OK;
}

error StackOK(Stack* stk, const size_t Line,
              const char* NameFile, const char* func) // add check hash, add canary 
{
    CHECKCONDITION(stk == NULL, NULL_STR_STK, " ERROR: Stack == NULL\n");
    
    char Error = 0;

    if (stk->Capacity < stk->Size)
    {
        fprintf(LogFile," %s:%ld: error in function %s: Stack(%s) capacity less stack size: "
                "capacity = %ld, size = %ld\n", NameFile, Line, func, stk->StackInfo.stk_name,
                                            stk->Capacity, stk->Size);
        Error = Error | (1<<(CAPACITY_LESS_SIZE-1));
    }
    if (stk-> Capacity == 0)
    {
        fprintf(LogFile," %s:%ld: error in function %s: "
               "Stack(%s) capacity equal zero.\n", NameFile, Line,
                                                func, stk->StackInfo.stk_name);
        Error = Error | (1<<(CAPACITY_EQUAL_ZERO-1));
    }
    if (stk->data == NULL)
    {
        fprintf(LogFile," %s:%ld: error in function %s: "
               "In the stack(%s) adress of massive equal zero.\n", NameFile, Line,
                                                             func, stk->StackInfo.stk_name);
        Error = Error | (1<<(NULL_STACK_DATA-1));
    }

    char StatusCanary = CheckCanary(stk);
    Error = Error | (StatusCanary<<(STACK_CANARY_DESTROY-1));


    char StatusHash = CheckHash(stk);
    Error = Error | (StatusHash<<(INC_HASH_STACK-1));
    
    
    if (Error != 0)
    {
        STACK_DUMP(stk);

        if ((Error & ((1<<(INC_HASH_STACK-1)) + (1<<(INC_HASH_DATA-1)))) > 0)
            exit(INC_HASH);

        stk->ErrorsInfo = Error;
        return ERROR;
    }
    else
    {
        fprintf(LogFile, " %s: All is good (StackOK) in function %s.\n",NameFile, func);
        return OK;
    }
}

#ifndef RELEASE
error StackDump (Stack* stk, const size_t nline, const char* NameFile, const char* func)
{  
    CHECKCONDITION(stk == NULL, NULL_STR_STK, " ERROR: Stack == NULL\n");

    fprintf(LogFile,"\n\nStack[%p] %s from %s(%ld) %s", stk, stk->StackInfo.stk_name, stk->StackInfo.birth_file,
                                          stk->StackInfo.birth_line, stk->StackInfo.birth_function);
    fprintf(LogFile," called from %s(%ld) %s\n", NameFile, nline, func);
    fprintf(LogFile,"  {\n   Size = %ld\n   Capacity = %ld\n   LeftCanary = %0llx\n   RightCanary = %0llx\n   Hash = %llu\n   data[%p]\n"
    "\t{\n\t", stk->Size, stk->Capacity, stk->left_canary, stk->right_canary, stk->StkHash, (void*) stk->data);

    if (stk->Size > 0)
    {
        for (size_t nelemnt = 0; nelemnt < stk->Size; nelemnt++)
            fprintf(LogFile,"*[%ld]=%d\n\t", nelemnt, stk->data[nelemnt]);

        if (stk->Capacity > 0)
        {
            for (size_t nelemnt = stk->Size; nelemnt < stk->Capacity; nelemnt++)
                fprintf(LogFile," [%ld]=%d\n\t", nelemnt, stk->data[nelemnt]);
        }
    }

    fprintf(LogFile,"}\n  }\n\n");

    return ERROR;
}

static error AddCanary(Stack* stk)
{
    CHECKCONDITION(stk == NULL, NULL_STR_STK,  " ERROR: AddCanary: Stack == NULL\n");

    stk->left_canary = left_canary;
    stk->right_canary = right_canary;

    *(CanaryType*)((char*) stk->data - sizeof(CanaryType)) = left_canary;
    *(CanaryType*) (stk->data+stk->Capacity) = right_canary;

    return OK;
}

static Error CheckCanary(Stack* stk)
{
    CHECKCONDITION(stk == NULL, NULL_STR_STK,  " ERROR: CheckCanary: Stack == NULL\n");

    char NAttacks = 0;

    if (stk->left_canary != left_canary || stk->right_canary != right_canary)
    {
        NAttacks = 1;
        fprintf(LogFile," Attack on Stack(%s) canary!!\nCanary left = %0llx\n",
                                stk->StackInfo.stk_name, stk->left_canary);
        fprintf(LogFile,"right = %0llx\n", stk->right_canary);
    }

    if (((CanaryType*)stk->data)[-1] != left_canary ||
       *(CanaryType*) ((Elemt*)stk->data + stk->Capacity) != right_canary)
    {
        NAttacks+=2;
        fprintf(LogFile," Attack on Stack(%s) Data massive canary!!\nCanary left = %0llx\n",
                                  stk->StackInfo.stk_name,((CanaryType*)stk->data)[-1]);  // func left canary ...
        fprintf(LogFile,"right = %0llx\n",  *(CanaryType*) ((Elemt*)stk->data+stk->Capacity));
    }   

    return NAttacks;
}

static HashType AddHashStk(Stack* stk) 
{
    CHECKCONDITION(stk == NULL, NULL_STR_STK,  " ERROR: AddHashStk: Stack == NULL\n");

    size_t Hash = 5381;

    for (size_t i = 0; i < (sizeof(*stk) - sizeof(HashType)); ++i)
    {                                                                                // copePASSTE
        Hash = ((Hash << 5) + Hash) + ((char*)stk)[i];
    }
    stk->StkHash = Hash;

    return Hash;
}

static HashType AddHashData(Stack* stk)
{
    CHECKCONDITION(stk == NULL, NULL_STR_STK,  " ERROR: AddHashData: Stack == NULL\n");

    size_t Hash = 5381;

    for (size_t i = 0; i < (2*sizeof(CanaryType) + stk->Capacity*sizeof(Elemt)); ++i)
    {
        Hash = ((Hash << 5) + Hash) + ((char*)stk->data-sizeof(CanaryType))[i];
    }
    *(HashType*)((char*)stk->data+sizeof(Elemt)*stk->Capacity+sizeof(CanaryType)) = Hash;

    return Hash;
}

static Error CheckHash(Stack* stk)
{
    CHECKCONDITION(stk == NULL, NULL_STR_STK,  " ERROR: CheckHash: Stack == NULL\n");

    char NAttacks = 0;

    HashType PreviousHashStk = stk->StkHash;
    HashType PreviousHashData = *(HashType*)((char*)stk->data+
                                sizeof(Elemt)*stk->Capacity+sizeof(CanaryType));

    HashType NowHashStk = AddHashStk(stk);
    HashType NowHashData = AddHashData(stk);
    if (PreviousHashStk != NowHashStk)
    {
        NAttacks = 1;
        fprintf(LogFile," ERROR: DDOS ATTACK on %s!!!! MEOW\n", stk->StackInfo.stk_name);
        fprintf(LogFile,"Previous hash stack = %0llx\nHash stack now = %0llx\n", PreviousHashStk, NowHashStk);
    }

    if (PreviousHashData != NowHashData)
    {
        NAttacks+=2;
        fprintf(LogFile," ERROR: DDOS ATTACK on %s DATA!!!! MEOW\n", stk->StackInfo.stk_name);
        fprintf(LogFile,"Previous hash stack data %0llx\nNow hash stack data %0llx\n", PreviousHashData, NowHashData);
    }

    return NAttacks;
}

static error ChangeHashStk(Stack* stk)
{
    CHECKCONDITION(stk == NULL, NULL_STR_STK,  "ERROR: ChangeHashStk: Stack == NULL\n");

    stk->StkHash = AddHashStk(stk);
    return OK;
}

static error ChangeHashData(Stack* stk)
{
    CHECKCONDITION(stk == NULL, NULL_STR_STK,  "ERROR: ChangeHashData: Stack == NULL\n");

    *(HashType*)((char*)stk->data+sizeof(Elemt)*stk->Capacity+
                        sizeof(CanaryType)) = AddHashData(stk);
    return OK;
}
#endif
