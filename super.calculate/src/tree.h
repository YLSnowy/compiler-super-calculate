#ifndef TREE_H
#define TREE_H

#include "pch.h"
#include "type.h"
#include "field.h"
extern ofstream *out;
enum NodeType
{
    NODE_CONST = 1, //常量
    NODE_VAR,       //变量
    NODE_EXPR,      //表达式
    NODE_TYPE,      //类型
    NODE_CON,       //常量修饰符

    NODE_STMT,  //语句
    NODE_PROG,  //程序
    NODE_PARAM, //参数

    NODE_ARRAY,
    NODE_ARRAY_EXPR,
};

enum ConstType //常量
{
    CON_INT = 1,
    CON_CHAR,
    CON_STRING,
};
enum OperatorType
{
    //OP_ASIGN=1,  // =
    OP_EQUAL = 1, //==
    OP_GREAT,
    OP_LESS,
    OP_GREAT_EQ,
    OP_LESS_EQ,
    OP_NOT_EQ,

    OP_ADD, //+
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_NEG, //负号
    OP_POS, //正号
    OP_AND,
    OP_OR,
    OP_NOT,

    OP_ADDR, //&
    OP_VAL,  //*

    OP_FUNC_USE, //函数调用
};

enum StmtType
{
    STMT_SKIP = 1, //空语句
    STMT_DEFINE,   //定义语句
    STMT_ASSIGN,
    STMT_DECL, //声明
    STMT_WHILE,
    STMT_FOR,
    STMT_RET,
    STMT_PRT, //printf
    STMT_SCF, //scanf
    STMT_BREAK,
    STMT_CONTINUE,

    STMT_FUNC_DECL,
    STMT_FUNC_DEF,

    STMT_SELF_INC_R,
    STMT_SELF_INC_L,
    STMT_SELF_DEC_R,
    STMT_SELF_DEC_L,

    STMT_IF_ELSE, //if语句
    STMT_IF,
    STMT_ELSE,

    STMT_BLOCK, //语句块

    STMT_BOOL, //表示if for else 中的bool部分，没有运算符号的结点

    STMT_DECLATION,
    STMT_DECL_ARRAY,
    STMT_DEFINE_ARRAY,
    STMT_ARRAY_INIT,
};

// #define MAX_CHILDREN 4

// union NodeAttr {
// 	int op;
// 	int vali;
// 	char valc;
// 	int symtbl_seq;

// 	NodeAttr(void) { op = 0; }
// 	NodeAttr(int i)	{ op = i; }
// 	NodeAttr(char c) { valc = c; }
// };

struct Label
{
    int true_label;
    int false_label;
    int begin_label;
    int next_label;
    Label()
    {
        true_label = 0;
        false_label = 0;
        begin_label = 0;
        next_label = 0;
    }
};

// struct Node
// {
// 	struct Node *children[MAX_CHILDREN];
// 	struct Node *sibling;
// 	int lineno;
// 	int kind;
// 	int kind_kind;
// 	NodeAttr attr;
// 	int type;
// 	int seq;
// 	int temp_var;
// 	Label label;

// 	void output(void);
// };

struct TreeNode
{
public:
    int nodeID;        // 用于作业的序号输出
    int lineno;        //行号
    NodeType nodeType; //结点类型

    TreeNode *child = nullptr;
    TreeNode *sibling = nullptr;

    //TreeNode* father=nullptr;

    void addChild(TreeNode *);
    void addSibling(TreeNode *);

    void printNodeInfo();
    void printChildrenId();

    void printAST(); // 先输出自己 + 孩子们的id；再依次让每个孩子输出AST。
    void printSpecialInfo();

    void genNodeId();
    int typechecking();

public:
    ConstType contype;
    OperatorType optype; // 如果是表达式
    Type *type;          // 变量、类型、表达式结点，有类型。
    StmtType stype;
    int int_val;     //整型
    string int_val_; //整型的字符串
    char ch_val;     //字符
    bool b_val;      //布尔
    string str_val;
    string var_name;
    int var_func; //0代表变量，1代表函数,-1代表未设置
    string label;
    Label controllabel;
    bool is_const; // 表示变量是否为常量

    int lex;          //符号表结点指针
    string workfield; //变量作用域
    table *scope;     //所属作用域

    CHECKTYPE checktype; //用于进行类型检查
    Label control;       //用于跳转
    int val;             //结点的值，忽略类型差异，int char bool

    int offset; //相对于ebp的偏移
    int set;    //变量是否有初值；

    static int current_offset;
    static int current_node_id;
    static int current_label;

    int max_dimen;          //如果是数组，存储了数组声明时候的最大的维度
    vector<int> dimen_size; //存储了每一维的最大存储个数
    int row;
    int col;
    int number; //表示一共初始化了多少元素
    bool param;
    bool is_array;

public:
    string nodeType2String(); //结点基本类型
    // static string opType2String (OperatorType type);
    string sType2String();   //语句类型
    string tType2String();   //int bool
    string cType2String();   //常量
    string opType2String();  //表达式
    string varName2String(); //变量名
    string getfield();       //作用域
    string checktype2string();

public:
    TreeNode(int lineno, NodeType type);
    //TreeNode(){}
    bool type_check();
    //int type_set_check();//类型检查结点类型设置和检查,正确返回1，错误0

    void genoffset();        //生成偏移
    TreeNode *leftsibling(); //参数左兄弟
    int pushparam();         //参数从右至左入栈
    int pushparamchild();
    string setval();

    void genlabel();
    void genlabelstmt(); //打标
    void genlabelexpr();
    void recursivegenlabel();
    void genlabeljump(int begin, int next);

public:
    void asmout(); //输出汇编代码
    void asmfile(ofstream &asmout);

    void asmfunc();
    // void asmvar(ofstream &asmout);   //输出局部变量
    void asmconst(); //输出常量
    void asmprintf();
    void asmscanf(); //scanf
    void asmstmt();  //输出语句的汇编
    void asmret();   //return

    void asmopnum();    //+-*/
    void asmopassign(); //+=
    void asmoprel();    //大小比较
    void asmopaddr();   //&
    void asmif();
    void asmfuncuse();

    string asmnode();   //打印结点对应栈中的元素
    void asmstatic();   //打印全局变量
    void asmsetvalue(); //movl	%eax, -12(%ebp)

    void leftparam();
    void rightparam();
};
#endif
int insertID_(TreeNode *root, table *scope);
int insertID(TreeNode *root, table *scope); //向当前作用域插入IDint
//static int checkID(string yytext,table*scope);//在当前作用域查找是否存在该ID
void getBlock(TreeNode *root, table *scope);   //根据ast得到作用域树
int getVarField(TreeNode *root, table *scope); //为变量分配作用域
void updateSize(table *scope);                 //更新每个作用域的size
