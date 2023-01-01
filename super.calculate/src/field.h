#ifndef FIELD_H
#define FIELD_H
#include "pch.h"
//#include "tree.h"

#define _CRT_SECURE_NO_WARNINGS

using namespace std;

#define MAX_LAYER 15	  //�?句块层数最大�?
#define MAX_ITEM_NUM 1024 //每个作用域最多可以声�?100�?变量

enum CHECKTYPE
{
	Notype = 0,
	Integer,
	Boolean,
	Char,
	String,
	Void,
};

struct checktypelink
{
	CHECKTYPE param;
	checktypelink *next;
};

class fielditem
{
public:
	string name;
	CHECKTYPE type;
	string label;	//标�??
	int var_func;	//0代表变量�?1代表函数,1代表�?设置
	string str_val; //常量字�?�串存储
	int int_val;	//全局变量整数存储
	char char_val;	//全局变量char存储
	int is_const;
	checktypelink *head; //参数列表
	fielditem();

	int offset;

	int set; //�?否有初�?
	bool param;
	bool is_array;

	vector<int> dimen_size;
};

class table
{
public:
	table *father; //指向父作用域
	table *child, *sibling;
	fielditem item[MAX_ITEM_NUM]; //该作用域下，每个变量对应一个item元素，注意从1开始存�?

	int index; //该表相�?�于父表的索引数,�?0开始标�?
	int num;   //该表有几�?子表
	int size;  //当前已经存储了几个item
	string attribute;
	//全局变量，初值为0
	// string lexms;//ID词素存储
	// int lexmspointer;//词素数组下一位的索引（长度）
	int maxoffset;
	table() //用于根的生成
	{
		maxoffset = 0;
		father = NULL;
		child = NULL;
		sibling = NULL;
		index = 0;
		num = 0;
		size = 0;
		//scope = this;
		attribute = "0";
	}
	table(table *f)
	{
		//对表f生成子表
		maxoffset = 0;
		child = NULL;
		sibling = NULL;
		father = f;
		father->num++;
		this->index = father->num;
		size = 0;
		num = 0;
		char s[7];
		sprintf(s, "%d", this->index);
		attribute += f->attribute + '_' + s;
		if (father->child == nullptr)
		{
			father->child = this;
		}
		else
		{
			table *t = f->child;
			for (; t->sibling != nullptr; t = t->sibling)
				;
			t->sibling = this;
		}

		//scope = this;//作用域切�?
	}
	static table *scoperoot; //生成根，用于存储全局变量和静态变量（静态变量待实现�?
	static table *keyword;	 //存储关键�?
	static table *scope;
	static void print(table *root); //打印符号�?
	//static void insertID(TreeNode* root,table* scope);//向当前作用域插入IDint

	static table *conststringtable;
	static table *functable;
};
#endif

int checkID(string yytext, table *scope);	//在当前作用域查找�?否存在�??ID
table *search(string yytext, table *scope); //查找所在作用域
//void print_attribute(int item_number);//打印节点属�?
//void insertID(string yytext);//向当前作用域插入IDint
//int lookupID(char* yytext);//在当前作用域查找�?否存在�??ID
//void insert_table::keyword(const char*);//插入关键�?
//bool lookupKW(char*);//查找声明ID�?否与table::keyword重名
