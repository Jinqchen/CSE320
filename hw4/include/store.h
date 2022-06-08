struct storeMap{
    char *var;
    int type;//string =0, int =1
    union{
        char *Sval;
        int Ival;
    }val;
    //char *val;
    //int lineNumber;
    struct storeMap *next;

};
struct storeMap *firststore = NULL;
struct storeMap *tailstore = NULL;