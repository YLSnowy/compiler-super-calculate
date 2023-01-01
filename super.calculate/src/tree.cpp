#include "tree.h"

int TreeNode::current_node_id = 0;
int TreeNode::current_offset = -12;
int TreeNode::current_label = 1;
int labelNumber = 0; //表示label的数量，这里的label专门是针对有初始化的数组的个数
int div_label = 0;   //arm没有除法，需要循环判断
vector<vector<int>> arrays_size;
map<string, int> static_vars;
int static_counts = 0;

extern ofstream *out;

void TreeNode::addChild(TreeNode *child)
{

    if (this->child == nullptr)
    {
        this->child = child;
    }
    else
    {
        TreeNode *t = this->child;
        for (; t->sibling != nullptr; t = t->sibling)
            ;
        t->sibling = child;
    }
}

void TreeNode::addSibling(TreeNode *sibling)
{
    TreeNode *p = this;
    while (p->sibling != nullptr)
    {
        p = p->sibling;
    }
    p->sibling = sibling;
}

TreeNode::TreeNode(int lineno, NodeType type)
{
    this->lineno = lineno;
    this->nodeType = type;
    workfield = "";
    scope = nullptr;
    this->checktype = Notype;
    var_name = -1;
    is_const = false;
    set = 0;
    offset = 0;
}

void TreeNode::genNodeId()
{ //自顶向下生成id
    this->nodeID = current_node_id;
    char tmp[20];
    sprintf(tmp, "L_%d", this->nodeID);
    this->label = tmp;
    if (this->child != nullptr)
    {
        current_node_id++;
        this->child->genNodeId();
    }
    if (this->sibling != nullptr)
    {
        current_node_id++;
        this->sibling->genNodeId();
    }

    return;
}

void TreeNode::printNodeInfo()
{
    *out << lineno << "\t@" << nodeID << "\t" << nodeType2String() << "\tchildren:";
    printChildrenId();
    *out << '\t';
    printSpecialInfo();
    *out << '\t' << checktype2string();
    if (this->nodeType == NODE_VAR || this->nodeType == NODE_EXPR)
        *out << '\t' << offset;
    *out << endl;
}

void TreeNode::printChildrenId()
{
    for (TreeNode *t = this->child; t != nullptr; t = t->sibling)
        *out << " @" << t->nodeID;
}

void TreeNode::printAST()
{
    this->printNodeInfo();
    if (this->child != nullptr)
        this->child->printAST();
    if (this->sibling != nullptr)
        this->sibling->printAST();
}

// You can *output more info...
void TreeNode::printSpecialInfo()
{
    switch (this->nodeType)
    {
    case NODE_CONST:
        *out << cType2String();
        break;
    case NODE_VAR:
        *out << varName2String();
        *out << " " << getfield();
        break;
    case NODE_EXPR:
        *out << opType2String();
        break;
    case NODE_STMT:
        *out << sType2String();
        break;
    case NODE_TYPE: //类型
        *out << tType2String();
        break;
    default:
        break;
    }
}

string TreeNode::getfield()
{
    string a = "field: ";
    if (scope != nullptr)
        return a + this->scope->attribute;
    else
        return a;
}

string TreeNode::sType2String()
{
    switch (this->stype)
    {
    case STMT_SKIP:
        return "stmt: empty";
    case STMT_DEFINE:
        return "stmt: define";
    case STMT_ASSIGN:
        return "stmt: assign";
    case STMT_DECL: //变量的声明或者定义
        return "stmt: declaration";
    case STMT_WHILE:
        return "stmt: while";
    case STMT_FOR:
        return "stmt: for";
    case STMT_RET:
        return "stmt: return";
    case STMT_PRT:
        return "stmt: printf";
    case STMT_SCF:
        return "stmt: scanf";
    case STMT_BREAK:
        return "stmt: break";
    case STMT_CONTINUE:
        return "stmt: continue";
    case STMT_DECLATION:
        return "stmt: stmt_declartion";
    case STMT_DECL_ARRAY:
        return "stmt: decl_array";

    case STMT_FUNC_DECL:
        return "stmt: func decalaration";
    case STMT_FUNC_DEF:
        return "stmt: func define";
    case STMT_SELF_INC_R:
        return "stmt: ++ R";
    case STMT_SELF_INC_L:
        return "stmt: ++ L";
    case STMT_SELF_DEC_R:
        return "stmt: -- R";
    case STMT_SELF_DEC_L:
        return "stmt: -- L";
    case STMT_IF_ELSE:
        return "stmt: if_else";
    case STMT_IF:
        return "stmt: if";
    case STMT_ELSE:
        return "stmt: else";
    case STMT_BLOCK:
        return "stmt: block";
    default:
        return "unknown stmt";
    }
}

string TreeNode::nodeType2String()
{
    switch (this->nodeType)
    {
    case NODE_CONST: //常量
        return "const";
    case NODE_VAR: //变量
        return "variable";
    case NODE_EXPR: //表达式
        return "expression";
    case NODE_TYPE: //类型
        return "type";
    case NODE_CON:
        return "const type";
    case NODE_STMT: //语句
        return "statement";
    case NODE_PROG: //程序
        return "program";
    case NODE_PARAM:
        return "parameter";
    case NODE_ARRAY:
        return "node_array";
    case NODE_ARRAY_EXPR:
        return "node_array_expr";

    default:
        return "unknown nodetype";
    }
    return "unknown__";
}

string TreeNode::tType2String()
{
    string a = "type: ";
    return a + type->getTypeInfo();
}

string TreeNode::cType2String()
{
    string a = "const: ";
    switch (this->contype)
    {
    case CON_INT:
        return a + "int " + this->int_val_;
    case CON_CHAR:
    {
        return a + "char '" + this->ch_val + "'";
    }
    case CON_STRING:
        return a + "string \"" + this->str_val + "\"";
    default:
        return "unknown const type";
    }
}

string TreeNode ::checktype2string()
{
    string a = "type：";
    switch (this->checktype)
    {
    case Integer:
        a += "Integer";
        break;
    case Boolean:
        a += "Boolean";
        break;
    case Char:
        a += "Char";
        break;
    case String:
        a += "String";
        break;
    default:
        a += "";
        break;
    }
    a += "\tis_const:";
    switch (this->is_const)
    {
    case 0:
        return a + "False";
    case 1:
        return a + "True";
        break;
    }
}

string TreeNode::opType2String()
{
    string a = "op: ";
    switch (this->optype)
    {
    case OP_FUNC_USE:
        return a + "function";
    case OP_EQUAL:
        return a + "==";
    case OP_GREAT:
        return a + ">";
    case OP_LESS:
        return a + "<";
    case OP_GREAT_EQ:
        return a + ">=";
    case OP_LESS_EQ:
        return a + "<=";
    case OP_NOT_EQ:
        return a + "!=";
    case OP_ADD:
        return a + "+";
    case OP_SUB:
        return a + "-（减）";
    case OP_MUL:
        return a + "*";
    case OP_DIV:
        return a + "/";
    case OP_MOD:
        return a + "%";
    case OP_NEG:
        return a + "- (负号)";
    case OP_POS:
        return a + "+ (正号)";
    case OP_AND:
        return a + "&&";
    case OP_OR:
        return a + "||";
    case OP_NOT:
        return a + "!";
    case OP_ADDR:
        return a + "&";
    case OP_VAL:
        return a + "*";
    default:
        return "unknown op";
    }
}
string TreeNode::varName2String()
{
    string a = "var_name: ";
    return a + this->var_name;
}
int insertID_(TreeNode *node, table *scope) //向当前作用域插入ID,
{
    node->scope = scope;
    node->workfield = scope->attribute;

    scope->item[scope->size].offset = node->offset;
    scope->item[scope->size].name = node->var_name;
    scope->item[scope->size].int_val = node->int_val;
    scope->item[scope->size].char_val = node->ch_val;
    scope->item[scope->size].str_val = node->str_val;

    scope->item[scope->size].label = node->label;
    scope->item[scope->size].type = node->checktype;
    scope->item[scope->size].var_func = node->var_func;
    scope->item[scope->size].set = node->set;
    scope->size++;
    int t = scope->size - 1;

    return t;
}
int insertID(TreeNode *node, table *scope) //向当前作用域插入ID,
{
    node->scope = scope;
    node->workfield = scope->attribute;

    if (node->var_name != "")
        if (checkID(node->var_name, scope) != -1)
        {
            *out << "第" << node->lineno << "行变量" << node->var_name << "已占用" << endl;
            node->printNodeInfo();
            return -1;
        }
    scope->item[scope->size].offset = node->offset;
    scope->item[scope->size].name = node->var_name;
    scope->item[scope->size].int_val = node->int_val;
    scope->item[scope->size].char_val = node->ch_val;
    scope->item[scope->size].str_val = node->str_val;
    scope->item[scope->size].is_const = node->is_const;

    scope->item[scope->size].label = node->label;
    scope->item[scope->size].type = node->checktype;
    scope->item[scope->size].var_func = node->var_func;
    scope->item[scope->size].set = node->set;
    scope->item[scope->size].param = node->param;
    scope->item[scope->size].is_array = node->is_array;

    if (node->nodeType == NODE_ARRAY)
    {
        //将数组的维度的size和maxsize赋给引用的节点
        int sum = 1;
        for (int i = 0; i < node->dimen_size.size(); i++)
        {
            scope->item[scope->size].dimen_size.push_back(node->dimen_size[i]);
            sum *= node->dimen_size[i];
        }

        scope->size += sum;
    }
    else
    {
        scope->size++;
    }
    int t = scope->size - 1;

    return t;
}

void updateSize(table *scope)
{
    if (scope == nullptr)
    {
        return;
    }
    table *tmp = scope->child;
    while (tmp != nullptr)
    {
        scope->size += tmp->size;
        tmp = tmp->sibling;
    }
    tmp = scope->child;
    while (tmp != nullptr)
    {
        updateSize(tmp);
        tmp = tmp->sibling;
    }
}
void getBlock(TreeNode *root, table *scope)
{
    if (root == nullptr)
        return;
    TreeNode *t = root->child;
    while (t != nullptr)
    {
        if (t->stype == STMT_BLOCK || t->stype == STMT_FOR || t->stype == STMT_FUNC_DEF || t->stype == STMT_FUNC_DECL)
        {
            table *newscope = new table(scope);
            t->workfield = newscope->attribute;
            t->scope = newscope;
            getBlock(t, newscope);
        }
        else
        {
            getBlock(t, scope);
        }
        t = t->sibling;
    }
}

int getVarField(TreeNode *root, table *scope)
{
    int flag = 1;
    root->genoffset();

    if (root == nullptr)
        return 1;

    TreeNode *t = root->child;
    if (root->scope != nullptr)
    {
        scope = root->scope;
    }
    while (t != nullptr)
    {
        if (t->scope == nullptr)
        {
            if (t->nodeType == NODE_VAR)
            {
                if (root->nodeType == NODE_PARAM || root->stype == STMT_DECL)
                {                //变量定义或者函数形参
                                 //直接加入作用域
                    t->set = -1; //未设置初值

                    if (insertID(t, scope) == -1)
                    {
                        flag = 0;
                    }
                }
                else if (root->stype == STMT_FUNC_DECL)
                {
                    if (checkID(t->var_name, scope->father) == -1)
                    {
                        int index = insertID(t, scope->father);
                        if (index == -1)
                        {
                            flag = 0;
                        }
                        if (t->sibling != nullptr)
                        {
                            TreeNode *m = t->sibling->child;
                            if (m != nullptr)
                            {
                                scope->father->item[index].head = new checktypelink;
                                checktypelink *link = scope->father->item[index].head;
                                for (; m != nullptr; m = m->sibling)
                                {
                                    if (m->nodeType == NODE_TYPE)
                                    {
                                        link->param = m->checktype;
                                        link->next = new checktypelink;
                                    }
                                }
                            }
                        }
                    }
                }
                else if (root->stype == STMT_FUNC_DEF)
                {
                    if (checkID(t->var_name, scope->father) == -1)
                    {
                        int index = insertID(t, scope->father);
                        if (index == -1)
                        {
                            flag = 0;
                        }
                        if (t->sibling != nullptr)
                        {
                            TreeNode *m = t->sibling->child;
                            if (m != nullptr)
                            {
                                scope->father->item[index].head = new checktypelink;
                                checktypelink *link = scope->father->item[index].head;
                                for (; m != nullptr; m = m->sibling)
                                {
                                    if (m->nodeType == NODE_TYPE)
                                    {
                                        link->param = m->checktype;
                                        link->next = new checktypelink;
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
                    table *wfield = nullptr;
                    wfield = search(t->var_name, scope);
                    if (wfield == nullptr)
                    {
                        *out << "第" << t->lineno << "行"
                             << "变量" << t->var_name << "未声明" << endl;
                        {
                            flag = 0;
                        }
                        t->scope = nullptr;
                        t->workfield = "";
                        t->checktype = Notype;
                    }
                    else
                    {
                        t->scope = wfield;
                        t->workfield = scope->attribute;
                        t->var_func = 0;

                        int index = checkID(t->var_name, wfield);
                        t->checktype = wfield->item[index].type;
                        t->offset = wfield->item[index].offset;
                        t->is_const = wfield->item[index].is_const;
                        t->is_array = wfield->item[index].is_array;
                        t->dimen_size = wfield->item[index].dimen_size;
                        t->param = wfield->item[index].param;
                        switch (t->checktype)
                        {
                        case Integer:
                            t->int_val = wfield->item[index].int_val;
                            break;
                        case Char:
                            t->ch_val = wfield->item[index].char_val;
                            break;
                        case String:
                            t->str_val = wfield->item[index].str_val;
                            break;
                        default:
                            cerr << endl;
                        }
                        if (root->stype == STMT_ASSIGN && t->is_const)
                        {
                            *out << "第" << t->lineno << "行"
                                 << "该变量名" << t->var_name << " const" << endl;
                            flag = 0;
                        }
                    }
                }
            }
            else if (t->stype == STMT_ASSIGN && root->stype == STMT_DEFINE)
            { //变量定义
                TreeNode *m = t->child;
                if (m->nodeType != NODE_VAR)
                    m = m->sibling;
                if (checkID(m->var_name, scope) == -1)
                {
                    if (scope->attribute == "0")
                    {
                        switch (m->checktype)
                        {
                        case Integer:
                            m->int_val = m->sibling->int_val;
                            break;
                        case Char:
                            m->ch_val = m->sibling->ch_val;
                            break;
                        case String:
                            m->str_val = m->sibling->str_val;
                            break;
                        default:
                            cerr << endl;
                        }
                    }
                    if (insertID(m, scope) == -1)
                    {
                        flag = 0;
                    }
                }
                else
                {
                    *out << "第" << m->lineno << "行"
                         << "该变量名" << m->var_name << "已经占用" << endl;
                    {
                        flag = 0;
                    }
                    // for(;t!=nullptr;t=t->sibling)
                    // {
                    //     TreeNode* m=t->child;
                    // 	if(m->nodeType==NODE_VAR)
                    // 	{
                    // 		//加入作用域
                    //         if(checkID(m->var_name,scope)==-1)
                    //             insertID(m,scope);
                    //         else{
                    //             *out<<"第"<<m->lineno<<"行"<<"该变量名"<<m->var_name<<"已经占用"<<endl;
                    // 	}
                    // }
                }
            }
            else if (t->nodeType == NODE_ARRAY)
            {
                if (root->stype == STMT_DECL_ARRAY || root->stype == STMT_DEFINE_ARRAY)
                {
                    if (checkID(t->var_name, scope) == -1)
                    {
                        int index = insertID(t, scope);
                        if (index == -1)
                        {
                            flag = 0;
                        }
                    }
                }
                else if (root->nodeType == NODE_PARAM || root->stype == STMT_DECL)
                {
                    t->set = -1;     //未设置初值
                    t->param = true; //表示该数组是参数
                    if (insertID(t, scope) == -1)
                    {
                        flag = 0;
                    }
                }
                else
                {
                    table *wfield = nullptr;
                    wfield = search(t->var_name, scope);
                    if (wfield == nullptr)
                    {
                        *out << "第" << t->lineno << "行"
                             << "变量" << t->var_name << "未声明" << endl;
                        flag = 0;
                        t->scope = nullptr;
                        t->workfield = "";
                        t->checktype = Notype;
                    }
                    else
                    {
                        t->scope = wfield;
                        t->workfield = scope->attribute;
                        t->var_func = 0;
                        int index = checkID(t->var_name, wfield);
                        if (index == -1)
                        {
                            cerr << "未找到标识符" << endl;
                            return 0;
                        }
                        t->checktype = wfield->item[index].type;
                        t->offset = wfield->item[index].offset;
                        t->dimen_size = wfield->item[index].dimen_size;
                        t->param = wfield->item[index].param;
                        t->is_array = wfield->item[index].is_array;

                        flag = 1;
                    }
                }
            }
        }
        if (t->contype)
        {
            if (t->contype == CON_STRING)
            {
                //添加至全局常量表
                t->var_func = 2;
                insertID_(t, table::conststringtable);
            }
        }

        if (getVarField(t, scope) == 0)
        {
            flag = 0;
        }
        t = t->sibling;
    }
    return flag;
}

bool TreeNode::type_check()
{
    //表达式+ - * / 关系表达式 assign，左右类型相同或者为char和int
    TreeNode *t = this->child;
    if (this->child == nullptr)
        return true;
    if (this->nodeType == NODE_EXPR)
    {
        switch (this->optype)
        {
        case OP_AND:
        case OP_OR:
        {
            if (t->checktype == Boolean && t->sibling->checktype == Boolean)
            {
                this->checktype = Boolean;
                return true;
            }
            return false;
        }
        case OP_NOT:
        {
            if (t->checktype == Boolean)
            {
                this->checktype = Boolean;
                return true;
            }
            return false;
        }
        case OP_NEG:
        case OP_POS:
        {
            if (t->checktype == Integer)
            {
                this->checktype = Integer;
                return true;
            }
            return false;
        }
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_MOD:
        {
            if (t->nodeType == NODE_ARRAY_EXPR)
            {
                if (t->sibling->nodeType == NODE_ARRAY_EXPR)
                {
                    if (t->child->checktype == Integer && t->sibling->child->checktype == Integer)
                    {
                        this->checktype = Integer;
                        return true;
                    }
                }
                else
                {
                    if (t->child->checktype == Integer && t->sibling->checktype == Integer)
                    {
                        this->checktype = Integer;
                        return true;
                    }
                }
            }
            else
            {
                if (t->checktype == Integer && t->sibling->checktype == Integer)
                {
                    this->checktype = Integer;
                    return true;
                }
            }
            return false;
        }

        case OP_GREAT:
        case OP_LESS:
        case OP_GREAT_EQ:
        case OP_LESS_EQ:
        {
            if (t->nodeType == NODE_ARRAY_EXPR)
            {
                if (t->sibling->nodeType == NODE_ARRAY_EXPR)
                {
                    if ((t->child->checktype == Integer || t->child->checktype == Char) && (t->sibling->child->checktype == Integer || t->sibling->checktype == Char))
                    {
                        this->checktype = Boolean;
                        return true;
                    }
                }
                else
                {
                    if ((t->child->checktype == Integer || t->child->checktype == Char) && (t->sibling->checktype == Integer || t->sibling->checktype == Char))
                    {
                        this->checktype = Boolean;
                        return true;
                    }
                }
            }
            else
            {
                if ((t->checktype == Integer || t->checktype == Char) && (t->sibling->checktype == Integer || t->sibling->checktype == Char))
                {
                    this->checktype = Boolean;
                    return true;
                }
            }
            return false;
        }
        case OP_EQUAL: //==
        case OP_NOT_EQ:
        {
            if (t->nodeType == NODE_ARRAY_EXPR)
            {
                if (t->sibling->nodeType == NODE_ARRAY_EXPR)
                {
                    if (t->child->checktype == t->sibling->child->checktype)
                    {
                        this->checktype = Boolean;
                        return true;
                    }
                }
                else
                {
                    if (t->child->checktype == t->sibling->checktype)
                    {
                        this->checktype = Boolean;
                        return true;
                    }
                }
            }
            else
            {
                if (t->sibling->nodeType == NODE_ARRAY_EXPR)
                {
                    if (t->checktype == t->sibling->child->checktype)
                    {
                        this->checktype = Boolean;
                        return true;
                    }
                }
                else
                {
                    if (t->checktype == t->sibling->checktype)
                    {
                        this->checktype = Boolean;
                        return true;
                    }
                }
            }
            return false;
        }
        case OP_FUNC_USE:
        {
            this->checktype = t->checktype;
            return true;
        }
        default:
            return true;
        }
    }

    //if for while bool表达式
    else if (this->nodeType == NODE_STMT)
    {
        switch (this->stype)
        {
        case STMT_ASSIGN:
        {
            if (t->nodeType == NODE_ARRAY_EXPR)
            {
                if (t->sibling->nodeType == NODE_ARRAY_EXPR)
                {
                    if (t->child->checktype == t->sibling->child->checktype)
                    {
                        this->checktype = t->checktype;
                        return true;
                    }
                }
                else
                {
                    if (t->child->checktype == t->sibling->checktype)
                    {
                        this->checktype = t->checktype;
                        return true;
                    }
                }
            }
            else
            {
                if (t->sibling->nodeType == NODE_ARRAY_EXPR)
                {
                    if (t->checktype == t->sibling->child->checktype)
                    {
                        this->checktype = t->checktype;
                        return true;
                    }
                }
                if (t->checktype == t->sibling->checktype)
                {
                    this->checktype = t->checktype;
                    return true;
                }
            }
            return false;
        }
        case STMT_SELF_INC_R:
        case STMT_SELF_INC_L:
        case STMT_SELF_DEC_R:
        case STMT_SELF_DEC_L:
        {
            if (t->checktype == Integer || t->checktype == Char)
            {
                this->checktype = t->checktype;
                return true;
            }
            return false;
        }
        default:
            return true;
        }
    }
    return true;
}

void TreeNode::recursivegenlabel()
{

    TreeNode *t = this;
    if (t != nullptr)
    {
        if (t->nodeType == NODE_EXPR)
        {
            t->genlabelexpr();
        }
        else if (t->nodeType == NODE_STMT)
        {
            t->genlabelstmt();
        }
        else
        {
            t->child->recursivegenlabel();
        }
    }
}

void TreeNode::genlabelexpr()
{
    if (this->child == nullptr || this->nodeType != NODE_EXPR)
        return;
    switch (this->optype)
    {
    case OP_AND:
    {
        TreeNode *left = this->child;
        TreeNode *right = left->sibling;

        if (this->controllabel.begin_label == 0)
        {
            this->controllabel.begin_label = current_label++;
        }
        left->controllabel.begin_label = this->controllabel.begin_label;
        right->controllabel.begin_label = current_label++;

        left->controllabel.true_label = right->controllabel.begin_label;
        left->controllabel.false_label = this->controllabel.false_label;

        right->controllabel.true_label = this->controllabel.true_label;
        right->controllabel.false_label = this->controllabel.false_label;

        left->controllabel.next_label = right->controllabel.begin_label;
        right->controllabel.next_label = this->controllabel.next_label;

        left->recursivegenlabel();
        right->recursivegenlabel();
        break;
    }
    case OP_OR:
    {
        TreeNode *left = this->child;
        TreeNode *right = left->sibling;

        if (this->controllabel.begin_label == 0)
        {
            this->controllabel.begin_label = current_label++;
        }
        left->controllabel.begin_label = this->controllabel.begin_label;
        right->controllabel.begin_label = current_label++;

        left->controllabel.true_label = this->controllabel.true_label;
        left->controllabel.false_label = right->controllabel.begin_label;
        right->controllabel.true_label = this->controllabel.true_label;
        right->controllabel.false_label = this->controllabel.false_label;

        left->controllabel.next_label = right->controllabel.begin_label;
        right->controllabel.next_label = this->controllabel.next_label;

        left->recursivegenlabel();
        right->recursivegenlabel();
        break;
    }
    case OP_NOT:
    {
        TreeNode *left = this->child;
        if (this->controllabel.begin_label == 0)
        {
            this->controllabel.begin_label = current_label++;
        }
        left->controllabel.begin_label = this->controllabel.begin_label;
        left->controllabel.true_label = this->controllabel.false_label;
        left->controllabel.false_label = this->controllabel.true_label;

        if (this->sibling != nullptr)
        {
            left->controllabel.next_label = this->controllabel.true_label;
        }
        else
        {
            left->controllabel.next_label = this->controllabel.next_label;
        }
        left->recursivegenlabel();
        break;
    }
    default:
        break;
    }
}

void TreeNode::genlabeljump(int begin, int next)
{
    TreeNode *node = this;
    while (node)
    {
        if (node->nodeType == NODE_STMT)
        {
            switch (node->stype)
            {
            case STMT_IF_ELSE:
                node->child->sibling->child->child->genlabeljump(begin, next);
                if (node->child->sibling->sibling->child)
                {
                    node->child->sibling->sibling->child->child->genlabeljump(begin, next);
                }
                break;
            case STMT_BREAK:
                node->controllabel.next_label = next;
                break;
            case STMT_CONTINUE:
                node->controllabel.next_label = begin;
                break;
            default:
                break;
            }
        }
        node = node->sibling;
    }
}

void TreeNode::genlabelstmt()
{
    if (this->nodeType == NODE_STMT)
    {
        switch (this->stype)
        {
        case STMT_WHILE:
        {
            TreeNode *boolchild = this->child->child;
            TreeNode *boolchildf = this->child;

            TreeNode *left = boolchildf->sibling;

            if (this->controllabel.begin_label == 0)
            {
                this->controllabel.begin_label = current_label++;
            }
            boolchild->controllabel.begin_label = this->controllabel.begin_label;
            boolchildf->controllabel.begin_label = this->controllabel.begin_label;

            boolchild->controllabel.true_label = current_label;
            boolchildf->controllabel.true_label = current_label++;
            left->controllabel.begin_label = boolchild->controllabel.true_label;

            if (this->controllabel.next_label == 0)
            {
                if (this->sibling != nullptr)
                {
                    if (this->sibling->controllabel.begin_label == 0)
                    {
                        this->controllabel.next_label = current_label++;
                        this->sibling->controllabel.begin_label = this->controllabel.next_label;
                    }
                    else
                    {
                        this->controllabel.next_label = this->sibling->controllabel.begin_label;
                    }
                }
                else
                {
                    this->controllabel.next_label = current_label++;
                }
            }

            boolchild->controllabel.false_label = this->controllabel.next_label;
            boolchildf->controllabel.false_label = this->controllabel.next_label;

            boolchild->controllabel.next_label = left->controllabel.begin_label;
            boolchildf->controllabel.next_label = left->controllabel.begin_label;
            left->controllabel.next_label = this->controllabel.begin_label;

            left->child->genlabeljump(this->controllabel.begin_label, this->controllabel.next_label);
            boolchild->recursivegenlabel();
            left->recursivegenlabel();
            break;
        }
        case STMT_FOR:
        {
            TreeNode *c1 = this->child;
            TreeNode *boolchildf = this->child->sibling;
            TreeNode *boolchild = boolchildf->child;

            TreeNode *c3 = boolchildf->sibling;
            TreeNode *left = boolchildf->sibling->sibling;

            if (this->controllabel.begin_label == 0)
            {
                this->controllabel.begin_label = current_label++;
            }
            c1->controllabel.begin_label = this->controllabel.begin_label;

            boolchild->controllabel.begin_label = current_label;
            boolchildf->controllabel.begin_label = boolchild->controllabel.begin_label;
            if (c3->controllabel.begin_label == 0)
            {
                c3->controllabel.begin_label = current_label++;
            }
            if (left->controllabel.begin_label == 0)
            {
                left->controllabel.begin_label = current_label++;
            }
            boolchild->controllabel.true_label = left->controllabel.begin_label;
            boolchildf->controllabel.true_label = left->controllabel.begin_label;

            if (this->controllabel.next_label == 0)
            {
                if (this->sibling != nullptr)
                {
                    if (this->sibling->controllabel.begin_label == 0)
                    {
                        this->controllabel.next_label = current_label++;
                        this->sibling->controllabel.begin_label = this->controllabel.next_label;
                    }
                    else
                    {
                        this->controllabel.next_label = this->sibling->controllabel.begin_label;
                    }
                }
                else
                    this->controllabel.next_label = current_label++;
            }

            boolchild->controllabel.false_label = this->controllabel.next_label;
            boolchildf->controllabel.false_label = this->controllabel.next_label;

            boolchild->controllabel.next_label = left->controllabel.begin_label;
            boolchildf->controllabel.next_label = left->controllabel.begin_label;

            left->controllabel.next_label = c3->controllabel.begin_label;
            c3->controllabel.next_label = boolchild->controllabel.begin_label;
            c1->controllabel.next_label = boolchild->controllabel.begin_label;

            boolchild->recursivegenlabel();
            left->recursivegenlabel();
            c1->recursivegenlabel();
            c3->recursivegenlabel();
            break;
        }

        case STMT_IF_ELSE:
        { //都是block的编号
            TreeNode *boolchild = this->child->child;
            TreeNode *boolchildf = this->child;
            TreeNode *left = this->child->sibling;
            TreeNode *right = left->sibling;

            if (this->controllabel.begin_label == 0)
            {
                this->controllabel.begin_label = current_label++;
            }
            boolchild->controllabel.begin_label = this->controllabel.begin_label;
            boolchildf->controllabel.begin_label = this->controllabel.begin_label;

            boolchild->controllabel.true_label = current_label++;
            boolchildf->controllabel.true_label = boolchild->controllabel.true_label;

            left->controllabel.begin_label = boolchild->controllabel.true_label;

            boolchild->controllabel.false_label = current_label++;
            boolchildf->controllabel.false_label = boolchild->controllabel.false_label;

            right->controllabel.begin_label = boolchild->controllabel.false_label;

            if (this->controllabel.next_label == 0)
            {
                if (this->sibling != nullptr)
                {
                    if (this->sibling->controllabel.begin_label == 0)
                    {
                        this->controllabel.next_label = current_label++;
                        this->sibling->controllabel.begin_label = this->controllabel.next_label;
                    }
                    else
                    {
                        this->controllabel.next_label = this->sibling->controllabel.begin_label;
                    }
                }
                else
                    this->controllabel.next_label = current_label++;
            }
            boolchild->controllabel.next_label = left->controllabel.begin_label;
            boolchildf->controllabel.next_label = left->controllabel.begin_label;

            left->controllabel.next_label = this->controllabel.next_label;
            right->controllabel.next_label = this->controllabel.next_label;

            boolchild->recursivegenlabel();
            left->recursivegenlabel();
            right->recursivegenlabel();
            break;
        }

        default:
        {
            //继续遍历下面的结点
            TreeNode *t = this->child;
            while (t != nullptr)
            {
                t->recursivegenlabel();
                t = t->sibling;
            }
            break;
        }
        }
    }
}

int TreeNode::typechecking()
{
    int flag = 1;
    TreeNode *t = this->child;
    while (t != nullptr)
    {
        if (t->typechecking() == 0) //失败
        {
            *out << "第" << t->lineno << "行结点" << this->nodeID << "类型1检查失败" << endl;
            flag = 0;
        }
        t = t->sibling;
    }
    if (this->type_check() == 0)
    {
        *out << "第" << this->lineno << "行结点" << this->nodeID << "类型检查失败" << endl;
        flag = 0;
    }
    return flag;
}

void TreeNode::asmout()
{
    asmconst();
    asmstatic();
    TreeNode *t = this->child;
    for (; t != nullptr; t = t->sibling)
    {
        if (t->stype == STMT_FUNC_DEF)
        {
            t->asmfunc();
        }
    }
    #ifdef ARM
    if(static_vars.size()!=0)
    {
        *out << ".L0:" << endl;
        for (map<string, int>::iterator it = static_vars.begin(); it != static_vars.end(); it++)
        {
            *out << "\t.word\t" << it->first << endl;
        }
    }
#endif
}

void TreeNode::asmfunc()
{
#ifdef X86
    *out << "\t.text" << endl;
    *out << "\t.globl\t" << this->child->sibling->var_name << "" << endl;
    *out << "\t.type\t" << this->child->sibling->var_name << ",@function" << endl;
    *out << "" << this->child->sibling->var_name << ":" << endl;
#endif

#ifdef ARM
    *out << "\t.text" << endl;
    *out << "\t.globl\t" << this->child->sibling->var_name << "" << endl;
    *out << "\t.type\t" << this->child->sibling->var_name << ", %function" << endl;
    *out << "" << this->child->sibling->var_name << ":" << endl;
#endif

    if (this->child->sibling->var_name == "main")
    { //主函数
#ifdef X86
        *out << "\tleal\t4(%esp), %ecx" << endl;
        *out << "\tandl\t$-16, %esp" << endl;
        *out << "\tpushl\t-4(%ecx)" << endl;
        *out << "\tpushl\t%ebp" << endl;
        *out << "\tmovl\t%esp, %ebp" << endl;
        *out << "\tpushl\t%ecx" << endl;
        *out << "\tsubl\t$" << (this->scope->size + 1) * 4 + 100 << ", %esp" << endl;
        *out << "" << endl;

        this->child->sibling->sibling->asmstmt();
        *out << endl;
        *out << "\tmovl\t$0, %eax" << endl;
        *out << "\tmovl\t-4(%ebp), %ecx" << endl;
        *out << "\tleave" << endl;
        *out << "\tleal\t-4(%ecx), %esp" << endl;
        *out << "\tret" << endl;
        *out << "\t.section\t.note.GNU-stack,\"\",@progbits" << endl;
#endif

#ifdef ARM
        *out << "\tpush\t{fp,lr}" << endl;
        *out << "\tadd\tfp, sp, #4" << endl;
        *out << "\tsub\tsp, sp, #" << (this->scope->size + 1) * 4 + 100 << endl;
        *out << "" << endl;

        this->child->sibling->sibling->asmstmt();
        *out << endl;
        *out << "\tsub\tsp, fp, #4" << endl;
        *out << "\tpop\t{fp, pc}" << endl;
#endif
    }
    else
    {
#ifdef X86
        *out << "\tpushl\t%ebp" << endl;
        *out << "\tmovl\t%esp, %ebp" << endl;
        *out << "\tsubl\t$" << (this->scope->size + 1) * 4 + 100 << ", %esp" << endl;
        TreeNode *m = this->child;
        while (m->sibling != nullptr)
        {
            m = m->sibling;
        }
        m->asmstmt();
        *out << endl;
        *out << "\tleave" << endl;
        *out << "\tret" << endl;
#endif

#ifdef ARM
        *out << "\tstr\t%fp, [sp, #-4]!" << endl;
        *out << "\tadd\tfp, sp, #0" << endl;
        *out << "\tsub\t"
             << "sp, sp, #" << (this->scope->size + 1) * 4 + 100 << endl;
        TreeNode *m = this->child;
        while (m->sibling != nullptr)
        {
            m = m->sibling;
        }
        m->asmstmt();
        *out << endl;
        *out << "\tadd\t"
             << "sp, fp, #0" << endl;
        *out << "\tldr\t"
             << "fp, [sp], #4" << endl;
        *out << "\tbx\tlr" << endl;
#endif
    }
}

void TreeNode::asmstmt()
{
    //带控制流的语句需要先处理，暂停深度遍历
    switch (this->stype)
    {
    case STMT_BOOL:
    {
        *out << "s_" << this->controllabel.begin_label << ":" << endl;
        break;
    }
    case STMT_WHILE:
    {
        break;
    }
    case STMT_IF_ELSE:
    {
        break;
    }
    case STMT_IF:
    {
        break;
    }
    case STMT_ELSE:
    {
        *out << "\tjmp\ts_" << this->controllabel.next_label << endl;
        *out << "s_" << this->controllabel.begin_label << ":" << endl;
        break;
    }
    case STMT_CONTINUE:
        *out << "\tjmp\ts_" << this->controllabel.next_label << endl;
        break;
    case STMT_BREAK:
        *out << "\tjmp\ts_" << this->controllabel.next_label << endl;
        break;
    default:
        break;
    }
    TreeNode *t = this->child;
    while (t != nullptr)
    {
        t->asmstmt();
        t = t->sibling;
    }
    if (nodeType == NODE_CONST)
    {
        // *out<<"\tmovl\t";
        // if(this->sibling->nodeType==contype)
        // {
        //     *out<<"$"<<this->setval();
        // }
        // else *out<<this->asmnode();
        // *out<<", %eax"<<endl;
    }
    switch (this->stype)
    {

    case STMT_PRT:
    {
        this->asmprintf();
        break;
    }
    case STMT_SCF:
    {
        this->asmscanf();
        break;
    }
    case STMT_RET:
    {
        this->asmret();
        break;
    }
    case STMT_DEFINE_ARRAY: //表示对数组元素初始化的节点
    {
        //对数组进行循环赋值, 首先全部赋值为0
        if (this->child->sibling->scope != nullptr && this->child->sibling->scope->attribute == "0")
        {
            *out << "\tleal\t" << this->child->sibling->var_name << ", %eax" << endl;
        }
        else if (this->child->sibling->scope != nullptr && this->child->sibling->scope->attribute != "0")
        {
            *out << "\tmovl\t%ebp, %eax" << endl;
            *out << "\taddl\t$" << this->child->sibling->offset << ", %eax" << endl;
        }

        int sum = 1;
        for (int i = 0; i < this->child->sibling->dimen_size.size(); i++)
        {
            sum *= this->child->sibling->dimen_size[i];
        }

        *out << "\tmovl\t$1, %ebx" << endl;
        *out << "\tmovl\t$-" << sum << ", %edx" << endl;
        *out << "label" << labelNumber << ":" << endl;
        labelNumber++;
        *out << "\tsubl\t$1, %ebx" << endl;
        *out << "\tmovl\t$0, (%eax, %ebx, 4)" << endl;
        *out << "\tcmpl\t%ebx, %edx" << endl;
        *out << "\tjl\tlabel" << labelNumber - 1 << endl;
        *out << "\tjge\tlabel" << labelNumber << endl;
        *out << "label" << labelNumber << ":" << endl;
        labelNumber++;

        TreeNode *tmp = this->child;
        //找到第一个初始化的元素的值
        while (tmp->stype != STMT_ARRAY_INIT)
        {
            tmp = tmp->sibling;
        }
        //然后，对有初始化的元素赋值
        while (tmp != nullptr)
        {
            if (tmp->nodeType == NODE_CONST)
            {
                *out << "\tmovl\t$" << tmp->int_val << ", %ecx" << endl;
            }
            else if (tmp->nodeType == NODE_EXPR || tmp->nodeType == NODE_VAR)
            {
                *out << "\tmovl\t" << tmp->setval() << ", %ecx" << endl;
            }
            else if (tmp->nodeType == NODE_ARRAY_EXPR)
            {
                *out << "\tpushl\t%eax" << endl;
                *out << "\tpushl\t%ebx" << endl;
                tmp->child->setval();
                if (tmp->child->scope->attribute != "0")
                {
                    *out << "\timull\t$-1, %ebx" << endl;
                }
                *out << "\tmovl\t(%eax, %ebx, 4), %ecx" << endl;
                *out << "\tpopl\t%ebx" << endl;
                *out << "\tpopl\t%eax" << endl;
            }
            *out << "\tmovl\t$" << tmp->row * this->child->sibling->dimen_size.back() + tmp->col << ", %ebx" << endl;
            if (this->child->sibling->scope != nullptr && this->child->sibling->scope->attribute != "0")
            {
                *out << "\timull\t$-1, %ebx" << endl;
            }
            *out << "\tmovl\t%ecx, (%eax, %ebx, 4)" << endl;
            tmp = tmp->sibling;
        }
        break;
    }
    //case STMT_DEFINE:
    case STMT_ASSIGN:
    {
        if (this->child->nodeType == NODE_ARRAY_EXPR)
        {
            this->child->child->setval();
            if (this->child->sibling->nodeType == NODE_ARRAY_EXPR)
            {
                *out << "\tpushl\t%eax" << endl;
                *out << "\tpushl\t%ebx" << endl;
                this->child->sibling->child->setval();

                if (this->child->sibling->child->scope->attribute != "0")
                {
                    *out << "\timull\t$-1, %ebx" << endl;
                }
                *out << "\tmovl\t(%eax, %ebx, 4), %ecx" << endl;
                *out << "\tpopl\t%ebx" << endl;
                *out << "\tpopl\t%eax" << endl;
            }
            else
            {
                *out << "\tmovl\t" << this->child->sibling->setval() << ", %ecx" << endl;
            }
            if (this->child->child->scope->attribute != "0")
            {
                *out << "\timull\t$-1, %ebx" << endl;
            }
            *out << "\tmovl\t%ecx, (%eax, %ebx, 4)" << endl;
            break;
        }
        else
        {
#ifdef X86
            *out << "\tmovl\t" << this->child->sibling->setval() << ", %eax" << endl;
            if (this->child->scope->attribute != "0")
            {
                //非局部变量
                this->child->asmsetvalue();
            }
            else
            {
                *out << "\tmovl\t%eax, " << this->child->var_name << endl;
            }
#endif

#ifdef ARM
            if (this->child->sibling->nodeType == NODE_CONST)
            {
                *out << "\tmov\tr3, ";
                *out << this->child->sibling->setval();
                *out << endl;
            }
            else if (this->child->sibling->scope != nullptr && this->child->sibling->scope->attribute == "0")
            {
                assert(0);
            }
            else if (this->child->sibling->nodeType == NODE_EXPR || this->child->sibling->nodeType == NODE_VAR)
            {
                *out << "\tldr\tr3, ";
                *out << this->child->sibling->setval();
                *out << endl;
            }
            else if (this->child->sibling->nodeType == NODE_ARRAY)
            {
                assert(0);
            }
            if (this->child->scope->attribute != "0")
            {
                //非局部变量
                //this->rightparam();
                this->child->asmsetvalue();
            }
            else
            {
                *out << "\tmovl\t%eax, " << this->child->var_name << endl;
            }
#endif
            break;
        }
    }
    case STMT_ELSE:
    {
        *out << "s_" << this->controllabel.next_label << ":" << endl;
        break;
    }
    case STMT_WHILE:
    {
        *out << "\tjmp\ts_" << this->controllabel.begin_label << endl;
        *out << "s_" << this->controllabel.next_label << ":" << endl;
        break;
    }
    case STMT_FOR:
    {
        *out << "\tjmp\ts_" << this->child->sibling->controllabel.begin_label << endl;
        *out << "s_" << this->controllabel.next_label << ":" << endl;
        break;
    }

    default:
        break;
    }
    switch (this->optype)
    {
    case OP_ADD:
    case OP_SUB:
    case OP_MUL:
    case OP_DIV:
    case OP_MOD:
    case OP_NEG:
    case OP_POS:
    {
        this->asmopnum();
        break;
    }
    case OP_EQUAL:
    case OP_GREAT:
    case OP_LESS:
    case OP_GREAT_EQ:
    case OP_LESS_EQ:
    case OP_NOT_EQ:
    {
        this->asmoprel();
        break;
    }
    case OP_ADDR:
    {
        this->asmopaddr();
        break;
    }
    case OP_FUNC_USE:
    {
        this->asmfuncuse();
    }
    default:
        break;
    }
}

void TreeNode::asmconst()
{
    *out << "	.section	.rodata" << endl; //
    for (int i = 0; i < table::conststringtable->size; i++)
    {
        *out << table::conststringtable->item[i].label << ":" << endl;
#ifdef X86
        *out << "\t.string \"" << table::conststringtable->item[i].str_val << "\"" << endl;
#endif
#ifdef ARM
        *out << "\t.ascii \"" << table::conststringtable->item[i].str_val << "\"" << endl;
        *out << "\t.align \t2" << endl;
#endif
    }
}

string TreeNode::setval()
{
    string a = "";
    val = 0;
    if (this->nodeType == NODE_CONST)
    {
        if (int_val)
        {
            val = int_val;
        }
        else if (ch_val)
        {
            val = (int)ch_val;
        }
        else if (b_val)
        {
            val = (int)b_val;
        }
        char tmp[20];
#ifdef X86
        sprintf(tmp, "$%d", val);
#endif
#ifdef ARM
        sprintf(tmp, "#%d", val);
#endif
        a = tmp;
    }
    else if (this->scope != nullptr && this->scope->attribute == "0")
    {
        a = this->var_name;
    }
    else if (this->nodeType == NODE_EXPR || this->nodeType == NODE_VAR)
    {
        char tmp[20];
#ifdef X86
        sprintf(tmp, "%d", offset);
        a = tmp;
        a += "(%ebp)";
#endif
#ifdef ARM
        sprintf(tmp, "[fp, #%d]", offset);
        a = tmp;
#endif
    }
    else if (this->nodeType == NODE_ARRAY) //对数组需要生成额外的汇编代码
    {
#ifdef X86
        if (this->scope != nullptr && this->scope->attribute == "0")
        {
            *out << "\tleal\t" << this->var_name << ", %eax" << endl;
            TreeNode *tmp = this->sibling;
            int i = 0;
            *out << "\tmovl\t$0, %ebx" << endl;
            while (tmp->sibling != nullptr && i < this->dimen_size.size())
            {
                if (tmp->nodeType == NODE_CONST)
                {
                    *out << "\tmovl\t$" << tmp->int_val << ", %ecx" << endl;
                    int sum = 1;
                    for (unsigned int j = this->dimen_size.size() - 1; j > i; j--)
                    {
                        sum *= this->dimen_size[j];
                    }
                    *out << "\timull\t$" << sum << ", %ecx" << endl;
                    *out << "\taddl\t%ecx, %ebx" << endl;
                }
                else if (tmp->nodeType == NODE_EXPR || tmp->nodeType == NODE_VAR)
                {
                    *out << "\tmovl\t" << tmp->setval() << ", %ecx" << endl;
                    int sum = 1;
                    for (unsigned int j = this->dimen_size.size() - 1; j > i; j--)
                    {
                        sum *= this->dimen_size[j];
                    }
                    *out << "\timull\t$" << sum << ", %ecx" << endl;
                    *out << "\taddl\t%ecx, %ebx" << endl;
                }
                tmp = tmp->sibling;
                i++;
            }
            if (tmp->nodeType == NODE_CONST)
            {
                *out << "\taddl\t$" << tmp->int_val << ", %ebx" << endl;
            }
            else if (tmp->nodeType == NODE_EXPR || tmp->nodeType == NODE_VAR)
            {
                *out << "\tmovl\t" << tmp->setval() << ", %ecx" << endl;
                *out << "\taddl\t%ecx, %ebx" << endl;
            }
        }
        else if (this->scope != nullptr && this->scope->attribute != "0")
        {
            *out << "\tmovl\t%ebp, %eax" << endl;
            *out << "\taddl\t$" << this->offset << ", %eax" << endl;
            if (this->param == true)
            {
                *out << "\tmovl\t(%eax), %eax" << endl;
            }
            TreeNode *tmp = this->sibling;
            int i = 0; //用于遍历存储好的数组的维度的size
            *out << "\tmovl\t$0, %ebx" << endl;
            while (tmp->sibling != nullptr && i < this->dimen_size.size())
            {
                if (tmp->nodeType == NODE_CONST)
                {
                    *out << "\tmovl\t$" << tmp->int_val << ", %ecx" << endl;
                    int sum = 1;
                    for (unsigned int j = this->dimen_size.size() - 1; j > i; j--)
                    {
                        sum *= this->dimen_size[j];
                    }
                    *out << "\timull\t$" << sum << ", %ecx" << endl;
                    *out << "\taddl\t%ecx, %ebx" << endl;
                }
                else if (tmp->nodeType == NODE_VAR || tmp->nodeType == NODE_EXPR)
                {
                    *out << "\tmovl\t" << tmp->setval() << ", %ecx" << endl;
                    int sum = 1;
                    for (unsigned int j = this->dimen_size.size() - 1; j > i; j--)
                    {
                        sum *= this->dimen_size[j];
                    }
                    *out << "\timull\t$" << sum << ", %ecx" << endl;
                    *out << "\taddl\t%ecx, %ebx" << endl;
                }
                tmp = tmp->sibling;
                i++;
            }
            if (tmp->nodeType == NODE_CONST)
            {
                *out << "\tmovl\t$" << tmp->int_val << ", %ecx" << endl;
                *out << "\taddl\t%ecx, %ebx" << endl;
            }
            else if (tmp->nodeType == NODE_EXPR || tmp->nodeType == NODE_VAR)
            {
                *out << "\tmovl\t" << tmp->setval() << ", %ecx" << endl;
                *out << "\taddl\t%ecx, %ebx" << endl;
            }
        }
#endif
        if (this->scope != nullptr && this->scope->attribute == "0")
        {
        }
        else if (this->scope != nullptr && this->scope->attribute != "0")
        {
        }
#ifdef ARM

#endif
    }
    return a;
}

TreeNode *TreeNode::leftsibling()
{
    TreeNode *t = this;
    for (; t->sibling != this; t = t->sibling)
    {
        if (t == nullptr)
            return nullptr;
    }
    return t;
}

int TreeNode::pushparam() //将参数从右至左压栈
{

    TreeNode *t = this->sibling;
    int count = 0;
    if (t != nullptr)
    {
        count += t->pushparam();
    }
    count++;

    if (this->nodeType == NODE_ARRAY_EXPR)
    {
        this->child->setval();
        if (this->child->scope->attribute != "0")
        {
            *out << "\timull\t$-1, %ebx" << endl;
        }
        *out << "\tmovl\t(%eax, %ebx, 4), %eax" << endl;
    }
    else if (this->is_array == true) //只用来判断参数是不是数组，这里面和printf的参数穿的不一样
    {
        if (this->scope != nullptr && this->scope->attribute != "0")
        {
            //将数组的维度放到全局变量中
            arrays_size.clear();
            arrays_size.push_back(this->dimen_size);
            //只需要将首地址传进去即可
            *out << "\tmovl\t%ebp, %eax" << endl;
            *out << "\taddl\t$" << this->offset << ", %eax" << endl;
            if (this->param == true)
            {
                *out << "\tmovl\t(%eax), %eax" << endl;
            }
        }
        else if (this->scope != nullptr && this->scope->attribute == "0")
        {
            *out << "\tleal\t" << this->var_name << ", %eax" << endl;
        }
    }
    else
    {
#ifdef X86
        *out << "\tmovl\t" << this->setval() << ", %eax" << endl;
#endif

#ifdef ARM
        *out << "\tldr\tr0, " << this->setval() << endl;
        *out << "\tpush\t{r0}" << endl;
#endif
    }

#ifdef X86
    *out << "\tpushl\t%eax" << endl;
#endif

    return count;
}

void TreeNode::asmfuncuse()
{
#ifdef X86
    *out << "\tsubl\t$12, %esp" << endl;
#endif

    int count = 0;
    if (this->child->sibling != nullptr)
    {
        int count = this->child->sibling->pushparam();
    }
    count *= 4;

#ifdef X86
    *out << "\tcall\t" << this->child->var_name << "\n\taddl\t$" << 12 + count + 4 << ", %esp" << endl;
    *out << "\tmovl\t%eax, " << this->offset << "(%ebp)" << endl;
#endif

#ifdef ARM
    *out << "\tbl\t" << this->child->var_name << endl;
    *out << "\tadd\tsp, sp, #" << 12+count+4 << endl;
    *out << "\tstr\tr0, " << this->setval() << endl;
#endif
}

void TreeNode::asmprintf()
{
#ifdef X86
    *out << "\tsubl\t$12, %esp" << endl;
    int count = 0;
    if (this->child->sibling != nullptr)
    {
        count = this->child->sibling->pushparam();
    }
    count *= 4;
    *out << "\tpushl\t$" << this->child->label << "" << endl;
    *out << "\tcall\tprintf\n\taddl\t$" << 12 + count + 4 << ", %esp" << endl;
#endif
#ifdef ARM
    if (this->child->sibling != nullptr)
        this->child->sibling->pushparam();
    *out << "\tadd\tr3, pc, r3" << endl;
    *out << "\tmov\tr0, r3" << endl;
    *out << "\tbl\tprintf(PLT)" << endl;
#endif
}

void TreeNode::asmscanf()
{
#ifdef X86
    *out << "\tsubl\t$12, %esp" << endl;
    int count = 0;
    cerr << this->child->sibling->nodeType << endl;
    if (this->child->sibling != nullptr)
    {
        int count = this->child->sibling->pushparam();
    }
    count *= 4;
    *out << "\tpushl\t$" << this->child->label << "" << endl;
    *out << "\tcall\t__isoc99_scanf\n\taddl\t$" << 12 + count + 4 << ", %esp" << endl;
#endif
#ifdef ARM
    if (this->child->sibling != nullptr)
        this->child->sibling->pushparam();
    *out << "\tadd\tr3, pc, r3" << endl;
    *out << "\tmov\tr0, r3" << endl;
    *out << "\tbl\tscanf(PLT)" << endl;
#endif
}

string TreeNode::asmnode()
{
    return setval();
}

void TreeNode::asmret()
{
#ifdef X86
    *out << "\tmovl\t";
    if (this->child->nodeType == NODE_CONST)
    {
        *out<<"\tmovl\t";
        *out<<"$";
        switch (this->child->contype)
        {
        case CON_INT:
        {
            *out << this->child->int_val;
            break;
        }
        case CON_CHAR:
        {
            *out << this->child->ch_val;
            break;
        }
        case CON_STRING:
        {
            *out << this->child->str_val;
            break;
        }
        default:
        {
            break;
        }
        }
        *out<<", %eax"<<endl;
    }
    else
    {
if (this->child->nodeType == NODE_ARRAY_EXPR)
        {
            this->child->child->setval();
            if (this->child->child->scope->attribute != "0")
            {
                *out << "\timull\t$-1, %ebx" << endl;
            }
            *out << "\tmovl\t(%eax, %ebx, 4), %eax" << endl;
        }
        else
        {
            *out<<"\tmovl\t";
            *out<<this->child->offset<<"(%ebp), %eax" << endl;
        }
    }
    *out << ", %eax" << endl;
    *out << "\tleave\n\tret\n";
#endif
#ifdef ARM
    if (this->child->nodeType == NODE_CONST)
    {
        *out << "\tmov\tr0, #";
        switch (this->child->contype)
        {
        case CON_INT:
        {
            *out << this->child->int_val;
            break;
        }
        case CON_CHAR:
        {
            *out << this->child->ch_val;
            break;
        }
        case CON_STRING:
        {
            *out << this->child->str_val;
            break;
        }
        default:
        {
            break;
        }
        }
    }
    else
    {
        char tmp[20];
        sprintf(tmp, "[fp, #%d]", offset);
        *out << "\tstr\tr0, " << tmp;
    }
    *out << endl;
#endif
}

void TreeNode::genoffset()
{
    if (this->nodeType == NODE_EXPR)
    {
        current_offset -= 4;
        this->offset += current_offset;
    }
    else if (this->nodeType == NODE_STMT)
    {
        if (this->stype == STMT_DEFINE)
        {
            TreeNode *t = this->child;
            for (; t != nullptr; t = t->sibling)
            {
                if (t->child != nullptr)
                {
                    current_offset -= 4;
                    t->child->offset += current_offset;
                }
            }
        }
        else if (this->stype == STMT_DECL)
        {
            TreeNode *t = this->child;
            for (; t != nullptr; t = t->sibling)
            {
                current_offset -= 4;
                t->offset = current_offset;
            }
        }
        else if (this->stype == STMT_FUNC_DEF)
        {
            //下面针对的都是参数
            //8是因为在调整新的ebp之前，先压入参数，然后是返回地址，然后是原函数的ebp
            //最后才调整了新的ebp和esp，所以，加8刚好就是参数
            current_offset = 8;
            TreeNode *t = this->child->sibling->sibling;
            while (t->sibling != nullptr)
            {
                //即便是数组也没有关系，因为这里面数组只需要存储个地址就可以
                t->child->sibling->offset = current_offset;
                current_offset += 4;
                t = t->sibling;
            }
            current_offset = -4;
        }
        else if (this->stype == STMT_DECL_ARRAY || this->stype == STMT_DEFINE_ARRAY) //虽然数组可以并列声明，但是stmt类型值对应一个数组的变量
        {
            //数组的首地址应该是最小的，
            //所以，因该先算出整个数组的大小，让数组的最后一个元素是current_offset

            if (this->child->sibling->nodeType == NODE_ARRAY)
            {
                //调整current_offset
                int size = 1;
                current_offset -= 4;
                this->child->sibling->offset = current_offset;

                for (int i = 0; i < this->child->sibling->dimen_size.size(); i++)
                {
                    size *= this->child->sibling->dimen_size[i]; //计算出数组的大小
                }
                current_offset += 4;
                current_offset -= size * 4;

                //调整插入节点的offset，该节点是正在初始化的节点
            }
            else
            {
                //调整current_offset
                int size = 1;
                for (int i = 0; i < this->child->sibling->dimen_size.size(); i++)
                {
                    size *= this->child->sibling->dimen_size[i]; //计算出数组的大小
                }
                current_offset -= size * 4;

                //调整插入节点的offset，该节点是正在初始化的节点
                this->child->sibling->offset = current_offset;
            }
        }
    }
}

void TreeNode::asmstatic() //打印全局变量
{
#ifdef X86
    *out << "\t.text" << endl;
    *out << "\t.data" << endl;
#endif

    TreeNode *c = this->child;

    while (c->stype == STMT_DECLATION || c->stype == STMT_DECL_ARRAY || c->stype == STMT_DEFINE_ARRAY)
    {
        TreeNode *t = c->child->sibling;
        while (t != nullptr)
        {
            if (t->stype == STMT_DEFINE)
            {
                TreeNode *m = t->child;
#ifdef X86
                *out << "\t.globl\t" << m->child->setval() << endl;
#endif

#ifdef ARM
                *out << "\t.global\t" << m->child->setval() << endl;
                *out << "\t.data" << endl;
                *out << "\t.align\t2" << endl;
                *out << "\t.type\t" << m->child->var_name << ", %object" << endl;
                *out << "\t.size\t" << m->child->var_name << ", 4" << endl;
                *out << m->child->var_name << ":" << endl;
                static_vars[m->child->var_name] = static_counts;
                static_counts++;
#endif

                switch (t->child->child->checktype)
                {
                case Integer:
                {
#ifdef X86
                    *out << "\t.align 4" << endl;
                    *out << "\t.type\t" << m->child->setval() << ", @object" << endl;
                    *out << m->child->setval() << ":" << endl;
                    *out << "\t.long " << m->child->sibling->int_val << endl;

#endif

#ifdef ARM
                    *out << "\t.word\t" << m->child->sibling->int_val << endl;
#endif

                    break;
                }
                case Char:
                {
                    *out << "\t.type\t" << m->child->setval() << ", @object" << endl;
                    *out << m->child->setval() << ":" << endl;
                    *out << "\t.byte " << (int)m->child->sibling->ch_val << endl;
                    break;
                }
                case String:
                {
                }
                default:
                    break;
                }
                m = m->sibling;
            }
            else if (t->stype == STMT_DECL)
            {
                
                TreeNode *m = t->child;
                static_vars[m->var_name] = static_counts;
                static_counts++;

                *out << "\t.comm\t" << m->setval();
                switch (t->child->checktype)
                {
                case Integer:
                {
                    *out << ", 4,4" << endl;
                    break;
                }
                case Char:
                {
                    *out << ", 1,1" << endl;
                    break;
                }
                case String:
                {
                    break;
                }
                default:
                    break;
                }
                m = m->sibling;
            }
            else if (t->nodeType == NODE_ARRAY)
            {
                int sum = 1;
                for (int i = 0; i < t->dimen_size.size(); i++)
                {
                    sum *= t->dimen_size[i];
                }
                *out << "\t.comm\t" << t->var_name << "," << sum * 4 << ",32" << endl;
            }
            t = t->sibling;
        }
        if (c->stype == STMT_DEFINE_ARRAY)
        {
            *out << "\t.text" << endl;
            *out << "\t.global\tstatic" << endl;
            *out << "\t.type\tstatic,@function" << endl;
            *out << "static:" << endl;

            //对数组进行循环赋值, 首先全部赋值为0
            //需要静态初始化的一定是全局变量，这里是单独列出来的，在函数中进行初始化的也进行了处理
            *out << "\tleal\t" << c->child->sibling->var_name << ", %eax" << endl;

            int sum = 1;
            for (int i = 0; i < c->child->sibling->dimen_size.size(); i++)
            {
                sum *= c->child->sibling->dimen_size[i];
            }

            *out << "\tmovl\t$-1, %ebx" << endl;
            *out << "\tmovl\t$" << sum << ", %edx" << endl;
            *out << "label" << labelNumber << ":" << endl;
            *out << "\taddl\t$1, %ebx" << endl;
            *out << "\tmovl\t$0, (%eax, %ebx, 4)" << endl;
            *out << "\tcmpl\t%edx, %ebx" << endl;
            *out << "\tjl\tlabel" << labelNumber << endl;
            labelNumber++;
            *out << "\tjge\tlabel" << labelNumber << endl;
            *out << "label" << labelNumber << ":" << endl;
            *out << "\tleal\t" << c->child->sibling->var_name << ", %eax" << endl;
            labelNumber++;

            TreeNode *tmp = c->child;
            //找到第一个初始化的元素的值
            while (tmp != nullptr && tmp->stype != STMT_ARRAY_INIT)
            {
                tmp = tmp->sibling;
            }
            //然后，对有初始化的元素赋值
            while (tmp != nullptr)
            {
                if (tmp->nodeType == NODE_CONST)
                {
                    *out << "\tmovl\t$" << tmp->int_val << ", %ecx" << endl;
                }
                else if (tmp->nodeType == NODE_EXPR || tmp->nodeType == NODE_VAR)
                {
                    *out << "\tmovl\t" << tmp->setval() << ", %ecx" << endl;
                }
                else if (tmp->nodeType == NODE_ARRAY_EXPR)
                {
                    *out << "\tpushl\t%eax" << endl;
                    *out << "\tpushl\t%ebx" << endl;
                    tmp->child->setval();
                    if (tmp->child->scope->attribute != "0")
                    {
                        *out << "\timull\t$-1, %ebx" << endl;
                    }
                    *out << "\tmovl\t(%eax, %ebx, 4), %ecx" << endl;
                    *out << "\tpopl\t%ebx" << endl;
                    *out << "\tpopl\t%eax" << endl;
                }
                *out << "\tmovl\t$" << tmp->row * c->child->sibling->dimen_size.back() + tmp->col << ", %ebx" << endl;
                *out << "\tmovl\t%ecx, (%eax, %ebx, 4)" << endl;
                tmp = tmp->sibling;
            }
        }
        c = c->sibling;
    }
}

void TreeNode::asmsetvalue()
{
#ifdef X86
    *out << "\tmovl\t%eax, " << this->asmnode();
    *out << endl;
#endif

#ifdef ARM
    *out << "\tstr\tr3, " << this->asmnode();
    *out << endl;
#endif
}

void TreeNode::leftparam()
{
    if (this->child != nullptr && this->child->nodeType == NODE_ARRAY_EXPR)
    {
#ifdef X86
        this->child->child->setval();
        if (this->child->child->scope->attribute != "0")
        {
            *out << "\timull\t$-1, %ebx" << endl;
        }
        *out << "\tmovl\t(%eax, %ebx, 4), %eax" << endl;
#endif

#ifdef ARM

#endif
    }
    else
    {
#ifdef X86
        *out << "\tmovl\t";
        *out << this->child->setval();
        *out << ", %eax" << endl;
#endif
#ifdef ARM
        if (this->child->nodeType == NODE_CONST)
        {
            *out << "\tmov\tr3, ";
            *out << this->child->setval();
            *out << endl;
        }
        else if (this->child->scope != nullptr && this->child->scope->attribute == "0")
        {
            // assert(0);
        }
        else if (this->child->nodeType == NODE_EXPR || this->child->nodeType == NODE_VAR)
        {
            *out << "\tldr\tr3, ";
            *out << this->child->setval();
            *out << endl;
        }
        else if (this->child->nodeType == NODE_ARRAY)
        {
            assert(0);
        }
#endif
    }
}

void TreeNode::rightparam()
{
    if (this->child->sibling != nullptr && this->child->sibling->nodeType == NODE_ARRAY_EXPR)
    {
        this->child->sibling->child->setval();
        if (this->child->sibling->child->scope->attribute != "0")
        {
            *out << "\timull\t$-1, %ebx" << endl;
        }
        *out << "\tmovl\t(%eax, %ebx, 4), %edx" << endl;
    }
    else
    {
#ifdef X86
        *out << "\tmovl\t";
        *out << this->child->setval();
        *out << ", %edx" << endl;
#endif
#ifdef ARM
        if (this->child->sibling->nodeType == NODE_CONST)
        {
            *out << "\tmov\tr2, ";
            *out << this->child->sibling->setval();
            *out << endl;
        }
        else if (this->child->sibling->scope != nullptr && this->child->sibling->scope->attribute == "0")
        {
            assert(0);
        }
        else if (this->child->sibling->nodeType == NODE_EXPR || this->child->sibling->nodeType == NODE_VAR)
        {
            *out << "\tldr\tr2, ";
            *out << this->child->setval();
            *out << endl;
        }
        else if (this->child->sibling->nodeType == NODE_ARRAY)
        {
            assert(0);
        }
#endif
    }
}

void TreeNode::asmopnum()
{
    //当前结点是操作符
    switch (this->optype)
    {
    case OP_ADD:
    {
        this->leftparam();
        if (this->child->sibling->nodeType == NODE_ARRAY_EXPR)
        {
            *out << "\tpushl\t%eax" << endl;
        }
        this->rightparam();
        if (this->child->sibling->nodeType == NODE_ARRAY_EXPR)
        {
            *out << "\tpop\t%eax" << endl;
        }
#ifdef X86
        *out << "\taddl\t%edx, %eax" << endl;
        *out << "\tmovl\t%eax, " << this->asmnode() << endl;
#endif

#ifdef ARM
        *out << "\tadd\tr3, r3, r2" << endl;
        *out << "\tstr\tr3, " << this->asmnode() << endl;
#endif
        break;
    }
    case OP_SUB:
    {
        this->leftparam();
        if (this->child->sibling->nodeType == NODE_ARRAY_EXPR)
        {
            *out << "\tpushl\t%eax" << endl;
        }
        this->rightparam();
        if (this->child->sibling->nodeType == NODE_ARRAY_EXPR)
        {
            *out << "\tpopl\t%eax" << endl;
        }
#ifdef X86
        *out << "\tsubl\t%edx, %eax" << endl;
        *out << "\tmovl\t%eax, " << this->asmnode() << endl;
#endif

#ifdef ARM
        *out << "\tsub\tr3, r3, r2" << endl;
        *out << "\tstr\tr3, " << this->setval() << endl;
#endif
        break;
    }
    case OP_MUL:
    {
        this->leftparam();
        if (this->child->sibling->nodeType == NODE_ARRAY_EXPR)
        {
            *out << "\tpushl\t%eax" << endl;
        }
        this->rightparam();
        if (this->child->sibling->nodeType == NODE_ARRAY_EXPR)
        {
            *out << "\tpopl\t%eax" << endl;
        }
#ifdef X86
        *out << "\timull\t%edx, %eax" << endl;
        *out << "\tmovl\t%eax, " << this->asmnode() << endl;
#endif

#ifdef ARM
        *out << "\tmul\tr3, r3, r2" << endl;
        *out << "\tstr\tr3, " << this->asmnode() << endl;
#endif
        break;
    }
    case OP_DIV:
    {

#ifdef X86
        this->leftparam();
        *out << "\tcltd" << endl;
        if (this->child->sibling->nodeType != NODE_CONST)
        {
            *out << "\tidivl\t" << this->child->sibling->setval() << endl;
        }
        else
        {
            *out << "\tmovl\t" << this->child->sibling->setval() << ", " << this->setval() << endl;
            *out << "\tidivl\t" << this->setval() << endl;
        }
        *out << "\tmovl\t%eax, " << this->asmnode() << endl;
#endif

#ifdef ARM
        *out << "\tldr\tr1, " << this->child->sibling->setval() << endl;
        *out << "\tldr\tr0, " << this->child->setval() << endl;
        *out << "\tbl\t__aeabi_idiv" << endl;
#endif
        break;
    }
    case OP_MOD:
    {
#ifdef X86
        this->leftparam();
        *out << "\tmovl\t" << this->child->sibling->setval() << ", %ecx" << endl;
        *out << "\tcltd" << endl;
        *out << "\tidivl\t%ecx" << endl;
        *out << "\tmovl\t%edx, %eax" << endl;
        *out << "\tmovl\t%eax, " << this->asmnode() << endl;
#endif

#ifdef ARM

#endif
        *out << "\tldr\tr3, " << this->child->sibling->setval() << endl;
        *out << "\tldr\tr1, " << this->child->setval() << endl;
        *out << "\tmov\tr0, r3" << endl;
        *out << "\tbl\t__aeabi_idivmod" << endl;
        *out << "\tmov\tr0, r3" << endl;
        *out << "\tstr\tr3, " << this->setval() << endl;
        break;
    }
    case OP_NEG:
    {
        this->leftparam();
#ifdef X86
        *out << "\tnegl\t%eax" << endl;
        *out << "\tmovl\t%eax, " << this->setval() << endl;
#endif
#ifdef ARM
        *out << "\tsub\tr3, #0, r3" << endl;
        *out << "\tstr\tr3, " << this->setval() << endl;
#endif
        break;
    }
    default:
        break;
    }
}

void TreeNode::asmoprel()
{
    this->leftparam();
    if (this->child->sibling->nodeType == NODE_ARRAY_EXPR)
    {
#ifdef X86
        *out << "\tpushl\t%eax" << endl;
#endif

#ifdef ARM
        *out << "\tPUSH\tR0" << endl;
#endif
    }
    this->rightparam();
    if (this->child->sibling->nodeType == NODE_ARRAY_EXPR)
    {
#ifdef X86
        *out << "\tpopl\t%eax" << endl;
#endif

#ifdef ARM
        *out << "\tpop\tr0" << endl;
#endif
    }
#ifdef X86
    *out << "\tcmpl\t%edx, %eax" << endl;
#endif

#ifdef ARM
    *out << "\tcmp\tr4, r0" << endl;
#endif
    switch (this->optype)
    {
    case OP_EQUAL:
    {
#ifdef X86
        *out << "\tjne\t"
             << "s_" << this->controllabel.false_label << endl;
        *out << "\tje\t"
             << "s_" << this->controllabel.true_label << endl;
#endif

#ifdef ARM
        *out << "\tbne\ts_" << this->controllabel.false_label << endl;
        *out << "\tbeq\ts_" << this->controllabel.true_label << endl;
#endif
        if (this->sibling != nullptr)
        {
            *out << "s_" << this->sibling->controllabel.begin_label << ":" << endl;
        }
        else
            *out << "s_" << this->controllabel.next_label << ":" << endl;
        break;
    }
    case OP_GREAT:
    {
#ifdef X86
        *out << "\tjle\t"
             << "s_" << this->controllabel.false_label << endl;
        *out << "\tjg\t"
             << "s_" << this->controllabel.true_label << endl;
#endif

#ifdef ARM
        *out << "\tble\ts_" << this->controllabel.false_label << endl;
        *out << "\tbgt\ts_" << this->controllabel.true_label << endl;
#endif
        if (this->sibling != nullptr)
        {
            *out << "s_" << this->sibling->controllabel.begin_label << ":" << endl;
        }
        else
        {
            *out << "s_" << this->controllabel.next_label << ":" << endl;
        }
        break;
    }
    case OP_LESS:
    {
#ifdef X86
        *out << "\tjge\t"
             << "s_" << this->controllabel.false_label << endl;
        *out << "\tjl\t"
             << "s_" << this->controllabel.true_label << endl;
#endif

#ifdef ARM
        *out << "\tbge\ts_" << this->controllabel.false_label << endl;
        *out << "\tblt\ts_" << this->controllabel.true_label << endl;
#endif
        if (this->sibling != nullptr)
        {
            *out << "s_" << this->sibling->controllabel.begin_label << ":" << endl;
        }
        else
            *out << "s_" << this->controllabel.next_label << ":" << endl;

        break;
    }
    case OP_GREAT_EQ:
    {
#ifdef X86
        *out << "\tjl\t"
             << "s_" << this->controllabel.false_label << endl;
        *out << "\tjge\t"
             << "s_" << this->controllabel.true_label << endl;
#endif

#ifdef ARM
        *out << "\tblt\ts_" << this->controllabel.false_label << endl;
        *out << "\tbge\ts_" << this->controllabel.true_label << endl;
#endif
        if (this->sibling != nullptr)
        {
            *out << "s_" << this->sibling->controllabel.begin_label << ":" << endl;
        }
        else
            *out << "s_" << this->controllabel.next_label << ":" << endl;
        break;
    }
    case OP_LESS_EQ:
    {
#ifdef X86
        *out << "\tjg\t"
             << "s_" << this->controllabel.false_label << endl;
        *out << "\tjle\t"
             << "s_" << this->controllabel.true_label << endl;
#endif

#ifdef ARM
        *out << "\tbgt\ts_" << this->controllabel.false_label << endl;
        *out << "\tble\ts_" << this->controllabel.true_label << endl;
#endif

        if (this->sibling != nullptr)
        {
            *out << "s_" << this->sibling->controllabel.begin_label << ":" << endl;
        }
        else
            *out << "s_" << this->controllabel.next_label << ":" << endl;
        break;
    }
    case OP_NOT_EQ:
    {
#ifdef X86
        *out << "\tje\t"
             << "s_" << this->controllabel.false_label << endl;
        *out << "\tjne\t"
             << "s_" << this->controllabel.true_label << endl;
#endif

#ifdef ARM
        *out << "\tbeq\ts_" << this->controllabel.false_label << endl;
        *out << "\tbne\ts_" << this->controllabel.true_label << endl;
#endif

        if (this->sibling != nullptr)
        {
            *out << "s_" << this->sibling->controllabel.begin_label << ":" << endl;
        }
        else
            *out << "s_" << this->controllabel.next_label << ":" << endl;
        break;
    }
    default:
        break;
    }
}

void TreeNode::asmopaddr()
{
    switch (this->optype)
    {
    case OP_ADDR:
    {
#ifdef X86
        *out << "\tleal\t" << this->child->setval() << ", %eax" << endl;
        *out << "\tmovl\t%eax, ";
        *out << this->asmnode() << endl;
#endif
#ifdef ARM
        *out << "\tleal\t" << this->child->setval() << ", %eax" << endl;
        *out << "\tmovl\t%eax, ";
        *out << this->asmnode() << endl;
#endif
        break;
    }
    default:
        break;
    }
}