#ifndef __STACK_H__
#define __STACK_H__

#ifndef RELEASE
    #define STACK_DUMP(Stk) StackDump((Stk), __LINE__,__FILE__,__PRETTY_FUNCTION__ )
#else
    #define STACK_DUMP(Stk) 
    #define CheckCanary(Stk) 0
    #define AddCanary(Stk)
    #define AddHashStk(Stk)
    #define AddHashData(Stk)
    #define CheckHash(Stk)  0
    #define ChangeHashStk(Stk)
    #define ChangeHashData(Stk)
#endif

#define STACK_CTOR(Stk, capacity) StackCtor((Stk), (capacity), #Stk, \
                                  __LINE__,__FILE__,__PRETTY_FUNCTION__)

#define STACKOK(Stk) StackOK((Stk), __LINE__,__FILE__,__PRETTY_FUNCTION__) // debug mode

#define CHECKCONDITION(condition, ErrorCode, ErrorMessage) do {              \
                                                            if (condition)  { \
                                                                if (LogFile == NULL) \
                                                                    LogFile = fopen(NameLogFile, "w"); \
                                                                fprintf(LogFile, "%s",ErrorMessage); \
                                                                return ErrorCode; \
                                                            }    }  while(0)       

typedef unsigned long long CanaryType;
typedef unsigned long long HashType;

typedef short Error;

typedef int Elemt;

const CanaryType right_canary = 0xB0BABABE;
const CanaryType left_canary  = 0xDEADFEED;

const char NameLogFile[] = "StackLogs.txt"; 

enum error {
    CAPACITY_LESS_SIZE = 1,  // !
    CAPACITY_EQUAL_ZERO = 2,// !
    NULL_STACK_DATA = 3,  // !    
    STACK_LEFT_CANARY_DESTROY = 4,   // !  // left and right
    STACK_RIGHT_CANARY_DESTROY = 5,
    DATA_LEFT_CANARY_DESTROY = 6,  // !
    DATA_RIGHT_CANARY_DESTROY = 7,
    INC_HASH_STACK = 8,  // !
    INC_HASH_DATA = 9,   // !

    NULL_STR_STK = 1000,
    INCORRECT_CAPACITY = 1001,
    NO_DIN_MEMORY = 1002,
    SIZE_EQUAL_ZERO = 1003,
    STACK_NOT_CTOR = 1004,
    STACK_DTOR = 1005, 
    LOG_ERROR = 1006,
    ADRESS_NULL = 1007,
    INC_HASH = 1008,
    NULL_POINTER = 1009,

    ERROR = -1,
    OK = 0
};

struct BirthInfo {
    const char* stk_name;  
    const char* birth_function;
    size_t birth_line;
    const char* birth_file;
}; 

struct Stack {
    CanaryType left_canary;    
    bool isStackCtor;
    bool isStackDtor; // strack debug
    BirthInfo StackInfo;
    size_t Size;
    Elemt* Data;
    size_t Capacity;
    Error ErrorsInfo;
    CanaryType right_canary;
    HashType StkHash;
};


error StackOK(Stack* Stk, const size_t Line,
              const char* NameFile, const char* Func);
error StackCtor(Stack* Stk, const size_t Capacity, const char* Name,
                const size_t birth_line, const char* birth_file, const char* birth_function);
error StackDtor(Stack* Stk);
error StackPop(Stack* Stk, Elemt* refValue);
error StackPush(Stack* Stk, Elemt Value);
error StackDump(Stack* Stk, const size_t nline, // const
                const char* NameFile, const char* Func);

#endif
