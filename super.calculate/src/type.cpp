#include "type.h"

Type::Type(ValueType valueType)
{
    this->type = valueType;
}

string Type::getTypeInfo()
{
    switch (this->type)
    {
    case VALUE_BOOL:
        return "bool";
    case VALUE_INT:
        return "int";
    case VALUE_CHAR:
        return "char";
    case VALUE_VOID:
        return "void";
    case VALUE_STRING:
        return "string";
    case VALUE_INT_POINTER:
        return "int_pointer";
    case VALUE_CHAR_POINTER:
        return "char_pointer";
    default:
        cerr << "shouldn't reach here, typeinfo";
        assert(0);
    }
    return "?";
}