#ifndef __STACK_H__
#define __STACK_H__

#ifndef RELEASE
    #define STACK_DUMP(stk) StackDump((stk), __LINE__,__FILE__,__PRETTY_FUNCTION__ )
#else
    #define STACK_DUMP(stk) ERROR
    #define CheckCanary(stk) OK
    #define AddCanary(stk)
    #define AddHashStk(stk)
    #define AddHashData(stk)
    #define CheckHash(stk)  OK
    #define ChangeHashStk(stk)
    #define ChangeHashData(stk)
#endif

#define STACK_CTOR(stk, capacity) StackCtor((stk), (capacity), #stk, \
                                  __LINE__,__FILE__,__PRETTY_FUNCTION__)

#define STACKOK(stk) StackOK((stk), __LINE__,__FILE__,__PRETTY_FUNCTION__) // debug mode


#define CHECKCONDITION(condition, ErrorCode, ErrorMessage) do {              \
                                                            if (condition)  \
                                                            {               \
                                                                if (LogFile == 0)               \
                                                                    LogFile = fopen(NameLogFile, "w"); \
                                                                fprintf(LogFile, "%s", ErrorMessage);\
                                                                return ErrorCode; \
                                                            }    }  while(0);    

typedef unsigned long long CanaryType;
typedef unsigned long long HashType;

typedef char Error;

typedef int Elemt;

const CanaryType right_canary = 0xB0BABABE;
const CanaryType left_canary  = 0xDEADFEED;

const char NameLogFile[] = "StackLogs.txt"; 

enum error {
    CAPACITY_LESS_SIZE = 1,  // !
    CAPACITY_EQUAL_ZERO = 2,// !
    NULL_STACK_DATA = 3,  // !    
    STACK_CANARY_DESTROY = 4,   // !  // left and right
    DATA_CANARY_DESTROY = 5,  // ! 
    INC_HASH_STACK = 6,  // !
    INC_HASH_DATA = 7,   // !

    NULL_STR_STK = 50,
    INCORRECT_CAPACITY = 51,
    NO_DIN_MEMORY = 52,
    SIZE_EQUAL_ZERO = 53,
    STACK_NOT_CTOR = 54,
    STACK_DTOR = 55, 
    LOG_ERROR = 56,
    ADRESS_NULL = 57,
    INC_HASH = 58,

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
    Elemt* data;
    size_t Capacity;
    Error ErrorsInfo;
    CanaryType right_canary;
    HashType StkHash;
};


error StackOK(Stack* stk, const size_t Line,
              const char* NameFile, const char* func);
error StackCtor(Stack* stk, const size_t Capacity, const char* name,
                const size_t birth_line, const char* birth_file, const char* birth_function);
error StackDtor(Stack* stk);
error StackPop(Stack* stk, Elemt* refValue);
error StackPush(Stack* stk, Elemt value);
error StackDump(Stack* stk, const size_t nline, // const
                const char* NameFile, const char* func);

#endif
