#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <stdint.h>
#include "Stack.h"
#include <cstddef>

static error StackOK(const Stack* stk, const size_t line,
              const char* namefile, const char* func);
#ifndef RELEASE
static error CheckHash(Stack* stk);
static error CheckCanary(Stack* stk);
static HashType AddHashStk(Stack* stk);
static HashType AddHashData(Stack* stk);
static error AddCanary(Stack* stk);
static error ChangeHashStk(Stack* stk);
static error ChangeHashData(Stack* stk);
#endif

error StackCtor(Stack* stk, const size_t Capacity, const char* name,
                const size_t birth_line, const char* birth_file, const char* birth_function)
{
    if (stk == NULL)
    {
        printf("ERROR: StackCtor: Incorrect value of adress stack massive == NULL");
        return NULLSTRSTK;
    }

    if (Capacity <= 0)
    {
        printf("ERROR: StackCtor: Stack Incorrect value of Capacity - %ld.\n", Capacity);
        return INCORRECTCAPACITY;
    }

    stk->isStackCtor = 1;

    stk->stk_name = name;
    stk->birth_function = birth_function;
    stk->birth_line = birth_line;
    stk->birth_file = birth_file;

    stk->Size = 0;
    stk->Capacity = Capacity;
    stk->data = (Elemt*) ((char*) calloc(2*sizeof(CanaryType)+Capacity*sizeof(Elemt)+sizeof(HashType)
                                , sizeof(char))
                          +sizeof(CanaryType));

    if (stk->data-sizeof(CanaryType) == nullptr)
    {
       printf("ERROR: StackCtor: Stack(%s) Allocation failure.\n",
                stk->stk_name);
        return NODINMEMORY; 
    }

    AddCanary(stk);

    AddHashStk(stk);
    AddHashData(stk);

    if(STACKOK(stk))
        return STACK_DUMP(stk);

    return OK;
}

error StackPush (Stack* stk, Elemt value)
{
    if (stk == NULL)
    {
        printf("ERROR: StackPush: Stack == NULL\n");
        return NULLSTRSTK;
    }

    if (!stk->isStackCtor)
    {
        printf("ERROR: StackPush: Stack does not created. Use funcion StackCtor first.\n");
        return STACKNOTCTOR;
    }

    if (stk->isStackDtor)
    {
        printf("ERROR: StackDtor: Stack(%s) is already deleted.\n", stk->stk_name);
        return STACKDTOR;
    }

    CheckHash(stk);

    if (CheckCanary(stk) == ATTACKCANARY)
    {
        STACK_DUMP(stk);
        return ATTACKCANARY;
    }

    if (STACKOK(stk) == ERROR)
        return STACK_DUMP(stk);

    if (stk->Size == stk->Capacity)
    {
        stk->Capacity *= 2;

        Elemt* massive = (Elemt*) ((char*) realloc((char*) stk->data - sizeof(CanaryType),
        sizeof(HashType)+stk->Capacity*sizeof(Elemt)+2*sizeof(CanaryType))+sizeof(CanaryType));

        if (massive - sizeof(CanaryType) == NULL)
        {
            printf("ERROR: StackPush: Stack(%s) Allocation failure.\n",
                    stk->stk_name);
            return NODINMEMORY;
        }

        stk->data = massive;

        AddCanary(stk);

        for (size_t nelemnt = stk->Size; nelemnt < stk->Capacity; nelemnt++)
            stk->data[nelemnt] = 0;
    }

    stk->data[stk->Size++] = value;

    ChangeHashStk(stk);
    ChangeHashData(stk);


    if(STACKOK(stk) == ERROR)
        return STACK_DUMP(stk);

    return OK;
}

error StackPop (Stack* stk, Elemt* refValue)
{
    if (stk == NULL)
    {
        printf("ERROR: StackPop: Stack == NULL\n");
        return NULLSTRSTK;
    }

    if (!stk->isStackCtor)
    {
        printf("ERROR: StackPush: Stack does not created. Use funcion StackCtor first.\n");
        return STACKNOTCTOR;
    }

    if (stk->isStackDtor)
    {
        printf("ERROR: StackDtor: Stack(%s) is already deleted.\n", stk->stk_name);
        return STACKDTOR;
    }

    CheckHash(stk);

    if (CheckCanary(stk) == ATTACKCANARY)
    {
        STACK_DUMP(stk);
        return ATTACKCANARY;
    }

    if (STACKOK(stk) == ERROR)
        return STACK_DUMP(stk);

    if (refValue == NULL)
    {
        printf("ERROR: StackPop: Stack(%s) Incorrect adress of refValue == NULL",
                stk->stk_name);
        return ADRESSNULL;
    }

    if (stk->Size == 0)
    {
        printf("ERROR: StackPop: Stack(%s) Size == 0\n", stk->stk_name);
        return SIZEEQUALZERO;
    }

    if (2*stk->Size <= stk->Capacity)
    {
        stk->Capacity /= 2;
        Elemt* massive = (Elemt*) ((char*) realloc((char*) stk->data - sizeof(CanaryType),
        sizeof(HashType)+stk->Capacity*sizeof(Elemt)+2*sizeof(CanaryType))+sizeof(CanaryType));

        if (massive - sizeof(CanaryType) == NULL)
        {
            printf("ERROR: StackPush: Stack(%s) Allocation failure.\n",
                    stk->stk_name);
            return NODINMEMORY;
        }

        stk->data = massive;

        AddCanary(stk);
    }


    *refValue = stk->data[--stk->Size];
    stk->data[stk->Size] = 0;

    ChangeHashStk(stk);
    ChangeHashData(stk);


    if(STACKOK(stk))
        return STACK_DUMP(stk);

    return OK;
}

error StackDtor (Stack* stk)
{
    if (stk == NULL)
    {
        printf("ERROR: StackDtor: Stack == NULL\n");
        return NULLSTRSTK;
    }

    if (!stk->isStackCtor)
    {
        printf("ERROR: StackPush: Stack does not created. Use funcion StackCtor first.\n");
        return STACKNOTCTOR;
    }

    if (stk->isStackDtor)
    {
        printf("ERROR: StackDtor: Stack(%s) is already deleted.\n", stk->stk_name);
        return STACKDTOR;
    }

    CheckHash(stk);

    if (CheckCanary(stk) == ATTACKCANARY)
    {
        STACK_DUMP(stk);
        return ATTACKCANARY;
    }

    if(STACKOK(stk) == ERROR)
        return STACK_DUMP(stk);

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

    stk->isStackDtor = 1;
    stk = 0;

    return OK;
}

static error StackOK(const Stack* stk, const size_t line,
              const char* namefile, const char* func)
{
    size_t n_error = 0;

    if (stk == nullptr)
    {
        printf("%s:%ld: error in function %s: "
               "Stack pointer equal zero.\n", namefile, line,
                                             func);
        n_error++;
        return ERROR;
    }
    if (stk->Size < 0)
    {
        printf("%s:%ld: error in function %s: "
               "Stack(%s) size less zero and equal %ld\n", namefile, line,
                                      func, stk->stk_name, stk->Size);
        n_error++;
    }
    if (stk->Capacity < 0)
    {
        printf("%s:%ld: error in function %s: "
               "Stack(%s) capacity less zero and equal %ld\n", namefile, line,
                                      func, stk->stk_name, stk->Capacity);
        n_error++;
    }
    if (stk->Capacity < stk->Size)
    {
        printf("%s:%ld: error in function %s: Stack(%s) capacity less stack size: "
                "capacity = %ld, size = %ld\n", namefile, line, func, stk->stk_name,
                                            stk->Capacity, stk->Size);
        n_error++;
    }
    if (stk-> Capacity == 0)
    {
        printf("%s:%ld: error in function %s: "
               "Stack(%s) capacity equal zero.\n", namefile, line,
                                                func, stk->stk_name);
    }
    if (stk->data == NULL)
    {
        printf("%s:%ld: error in function %s: "
               "In the stack(%s) adress of massive equal zero.\n", namefile, line,
                                                                func, stk->stk_name);
        n_error++;
    }

    if (n_error != 0)
    {
        printf("Total errors - %ld\n", n_error);
        return ERROR;
    }

    return OK;
}

#ifndef RELEASE
error StackDump (Stack* stk, const size_t nline, const char* namefile, const char* func)
{  
    if (stk == NULL)
    {
        printf("ERROR: Stack == NULL\n");
        return NULLSTRSTK;
    }

    printf("Stack[%p] %s from %s(%ld) %s", stk, stk->stk_name, stk->birth_file,
                                          stk->birth_line, stk->birth_function);
    printf(" called from %s(%ld) %s\n", namefile, nline, func);
    printf("  {\n   Size = %ld\n   Capacity = %ld\n   data[%p]\n"
    "\t{\n\t", stk->Size, stk->Capacity, (void*) stk->data);

    if (stk->Size > 0)
    {
        for (size_t nelemnt = 0; nelemnt < stk->Size; nelemnt++)
            printf("*[%ld]=%d\n\t", nelemnt, stk->data[nelemnt]);

        if (stk->Capacity > 0)
        {
            for (size_t nelemnt = stk->Size; nelemnt < stk->Capacity; nelemnt++)
                printf(" [%ld]=%d\n\t", nelemnt, stk->data[nelemnt]);
        }
    }

    printf("}\n  }\n");
    
    FILE* logs = fopen("StackLogs.txt", "w");

    if (logs == nullptr)
    {
        printf("ERROR: Can not create log file.\n");
        return LOGERROR;
    }

    fprintf(logs,"Stack[%p] %s from %s(%ld) %s", stk, stk->stk_name, stk->birth_file,
                                          stk->birth_line, stk->birth_function);
    fprintf(logs," called from %s(%ld) %s\n", namefile, nline, func);
    fprintf(logs,"  {\n   Size = %ld\n   Capacity = %ld\n LeftCanary = %0llx\n RightCanary = %0llx\n Hash = %llu\n   data[%p]\n"
    "\t{\n\t", stk->Size, stk->Capacity, stk->left_canary, stk->right_canary, stk->StkHash, (void*) stk->data);

    if (stk->Size > 0)
    {
        for (size_t nelemnt = 0; nelemnt < stk->Size; nelemnt++)
            fprintf(logs,"*[%ld]=%d\n\t", nelemnt, stk->data[nelemnt]);

        if (stk->Capacity > 0)
        {
            for (size_t nelemnt = stk->Size; nelemnt < stk->Capacity; nelemnt++)
                fprintf(logs," [%ld]=%d\n\t", nelemnt, stk->data[nelemnt]);
        }
    }

    fprintf(logs,"}\n  }\n");

    fclose(logs);

    return ERROR;
}

static error AddCanary(Stack* stk)
{
    if (stk == NULL)
    {
        printf("ERROR: AddCanary: Stack == NULL\n");
        return NULLSTRSTK;
    }

    stk->left_canary = left_canary;
    stk->right_canary = right_canary;

    *(CanaryType*)((char*) stk->data - sizeof(CanaryType)) = left_canary;
    *(CanaryType*) (stk->data+stk->Capacity) = right_canary;

    return OK;
}

static error CheckCanary(Stack* stk)
{
    if (stk == NULL)
    {
        printf("ERROR: CheckCanary: Stack == NULL\n");
        return NULLSTRSTK;
    }

    size_t nattacks = 0;

    if (stk->left_canary != left_canary || stk->right_canary != right_canary)
    {
        nattacks++;
        printf("Attack on Stack(%s) canary!! Canary left = %0llx ",
                                stk->stk_name, stk->left_canary);
        printf("right = %0llx\n", stk->right_canary);
    }

    if (((CanaryType*)stk->data)[-1] != left_canary ||
       *(CanaryType*) (stk->data+stk->Capacity) != right_canary)
    {
        nattacks++;
        printf("Attack on Stack(%s) Data massive canary!! Canary left = %0llx ",
                                  stk->stk_name,((CanaryType*)stk->data)[-1]);
        printf("right = %0llx\n",  *(CanaryType*) (stk->data+stk->Capacity));
    }

    if (nattacks != 0)
    {
        return ATTACKCANARY;
    }

    return OK;
}

static HashType AddHashStk(Stack* stk)
{
    if (stk == NULL)
    {
       printf("ERROR: AddHash: Stack == NULL\n");
        return NULLSTRSTK;
    }

    size_t Hash = 5381;

    for (size_t i = 0; i < (sizeof(*stk) - sizeof(HashType)); ++i)
    {
        Hash = ((Hash << 5) + Hash) + ((char*)stk)[i];
    }
    stk->StkHash = Hash;

    return Hash;
}

static HashType AddHashData(Stack* stk)
{
    if (stk == NULL)
    {
       printf("ERROR: AddHash: Stack == NULL\n");
        return NULLSTRSTK;
    }

    size_t Hash = 5381;

    for (size_t i = 0; i < (2*sizeof(CanaryType) + stk->Capacity*sizeof(Elemt)); ++i)
    {
        Hash = ((Hash << 5) + Hash) + ((char*)stk->data-sizeof(CanaryType))[i];
    }
    *(HashType*)((char*)stk->data+sizeof(Elemt)*stk->Capacity+sizeof(CanaryType)) = Hash;

    return Hash;
}

static error CheckHash(Stack* stk)
{
    if (stk == NULL)
    {
        printf("ERROR: CheckHash: Stack == NULL\n");
        return NULLSTRSTK;
    }

    HashType PreviousHashStk = stk->StkHash;
    HashType PreviousHashData = *(HashType*)((char*)stk->data+
                                sizeof(Elemt)*stk->Capacity+sizeof(CanaryType));

    if (PreviousHashStk != AddHashStk(stk))
    {
        printf("%0llx\n %0llx\n", PreviousHashStk, AddHashStk(stk));
        printf("ERROR: DDOS ATTACK on %s!!!! MEOW\n", stk->stk_name);
        exit(INCHASHSTACK);
    }

    if (PreviousHashData != AddHashData(stk))
    {
        printf("%0llx\n %0llx\n", PreviousHashData, AddHashData(stk));
        printf("ERROR: DDOS ATTACK on %s DATA!!!! MEOW\n", stk->stk_name);
        exit(INCHASHDATA);
    }

    return OK;
}

static error ChangeHashStk(Stack* stk)
{
    if (stk == NULL)
    {
        printf("ERROR: ChangeHash: Stack == NULL\n");
        return NULLSTRSTK;
    }

    stk->StkHash = AddHashStk(stk);
    return OK;
}

static error ChangeHashData(Stack* stk)
{
    if (stk == NULL)
    {
        printf("ERROR: ChangeHash: Stack == NULL\n");
        return NULLSTRSTK;
    }

    *(HashType*)((char*)stk->data+sizeof(Elemt)*stk->Capacity+
                        sizeof(CanaryType)) = AddHashData(stk);
    return OK;
}
#endif
