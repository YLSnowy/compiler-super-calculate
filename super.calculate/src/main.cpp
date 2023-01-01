#include "common.h"
#include <fstream>

extern TreeNode *root;
extern FILE *yyin;
extern int yyparse();

using namespace std;

ofstream *out;

int parseOptions(int argc, char **argv, string &input, string &output, bool &optimize, bool &assemble)
{
    int i = 0;
    if (argc == 1)
    {
        return -1;
    }
    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-S") == 0)
        {
            assemble = true;
        }
        else if (strcmp(argv[i], "-O2") == 0)
        {
            optimize = true;
        }
        else if (strcmp(argv[i], "-o") == 0)
        {
            output = argv[++i];
        }
        else
            input = argv[i];
    }

    return 0;
}

int main(int argc, char *argv[])
{
    string input, output;
    bool optimize = 0, assemble = 0;

    parseOptions(argc, argv, input, output, optimize, assemble);

    FILE *fin = fopen(input.c_str(), "r");
    if (fin != nullptr)
    {
        yyin = fin;
    }
    else
    {
        cerr << "failed to open file: " << input.c_str() << endl;
        return -1;
    }

    out = new ofstream(output.c_str());

    if (out->fail())
    {
        cerr << "failed to open file: " << output.c_str() << endl;
    }

    yyparse();

    table *scope = new table;
    table::scoperoot = scope;
    if (root != NULL)
    {
        root->genNodeId();

        getBlock(root, scope); //获得作用域树
        if (!getVarField(root, scope))
        {
            cout << "作用域检查出错";
            return 0; //将变量加入作用域
        }

        if (!root->typechecking())
        { //return 0;//类型检查
            cout << "类型检查出错";
            return 0;
        }

        TreeNode *t = root->child;
        //root->printAST();//打印语法树
        //cout << endl;
        while (t != nullptr)
        {
            t->recursivegenlabel();
            t = t->sibling;
        }
        //table::print(table::scoperoot);//打印符号表
        //table::print(table::conststringtable);
        //table::print(table::functable);
        updateSize(scope);
        root->asmout();
    }
    out->close();
    return 0;
}