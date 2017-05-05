#ifndef ASM_TABLE
#define ASM_TABLE
#include"ERRMES.HPP"
#include"header.hpp"
class AsmCode{

public:
    AsmCode*next;
    class ErrMessage{
        private:
            string Messenge;
            ErrMessage*next;
        public:
            void set_next(ErrMessage*);
            void printErrMes(fstream&);
            ErrMessage(string);
            ~ErrMessage();
    };
    AsmCode(TYPE,string,string,string,string,int,string);
    //AsmCode(int,string,string,string,string,int);
    void set_Address(unsigned int);
    void add_ErrMes(int,string);
    void add_ErrMes(string);
    void set_next(AsmCode*);
    void set_objcode(string);
    void set_objcode(unsigned int);
    TYPE get_type();
    unsigned int get_Address();
    string get_label();
    string get_opcode();
    string get_data();
    string get_objcode();
    string toString();
    ~AsmCode();
    bool has_ErrMes();
    void printErrMes(fstream&);
    void printData();
private:
    TYPE type;              //����
    string lable;           //����
    string opcode;          //���O-upper
    string opcode_old;      //���O-��l
    string data;            //���
    string commit;          //����
    int line;               //���
    unsigned int address;   //��}
    string objcode;         //objcode
    ErrMessage*head;        //���~�T�����쵲��C
};

class AsmTable{
    private:
        AsmCode*head;
        AsmCode*find_end();
        int lines;
    public:
        AsmCode*findLine(int);
        AsmTable();
        ~AsmTable();
        void append(AsmCode*);
        void printData();
        int get_lines();
};

#endif // ASM_TABLE
