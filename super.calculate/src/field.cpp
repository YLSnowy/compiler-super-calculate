#include "field.h"

//先遍历得到作用域�?
//再�?�添符号表项添加

table *table::scoperoot = new table; //生成根，用于存储全局变量和静态变量（静态变量待实现�?
table *table::keyword = new table;	 //存储关键�?
table *table::scope = table::scoperoot;
table *table::conststringtable = new table;
table *table::functable = new table;
//string table::lexms="";//存储id�?
//int table::lexmspointer=0;

fielditem::fielditem()
{
	type = Notype;
	var_func = -1;
	set = 0;
}

int checkID(string yytext, table *scope)
{
	for (int i = 0; i < scope->size; i++)
	{
		if (yytext == scope->item[i].name)
			return i;
	}
	return -1; //如果找到对应表项返回索引，没有找到返�?-1
}

table *search(string yytext, table *scope)
{
	table *t = scope;
	while (t != nullptr)
	{
		if (checkID(yytext, t) != -1)
		{
			//当前作用域中�?
			return t;
		}
		t = t->father;
	}
	return nullptr;
}
void table::print(table *root)
{
	cout << root->attribute << " ";

	for (int i = 0; i < root->size; i++)
	{

		if (root->item[i].var_func == 0)
		{
			cout << root->item[i].name << " ";
			cout << "变量#";
		}
		else if (root->item[i].var_func == 1)
		{
			cout << root->item[i].name << " ";
			cout << "函数#";
		}
		else if (root->item[i].var_func == 2)
		{
			cout << root->item[i].str_val << " ";
			cout << "常量";
		}
	}
	cout << endl;
	table *t = root->child;
	while (t != nullptr)
	{
		print(t);
		t = t->sibling;
	}
}