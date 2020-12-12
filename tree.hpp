#pragma once

#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cctype>
#include <cstring>

//--------------------------------------------------------------------------------------------------\\

typedef enum node_types_en
{
    NUM      = 1, // == double
    VAR      = 2, // == int
    OPERATOR = 3  // == int
} Node_type;

typedef union
{
    int    ival;
    double dval;
} Node_value;

//--------------------------------------------------------------------------------------------------\\

struct Node
{
    Node_type type;
    Node_value value;
    Node *left;
    Node *right;
};

//--------------------------------------------------------------------------------------------------\\

struct Tree
{
    static const int alphabet = 26;
    Node *root;
    bool var[alphabet]; // 'e' - isn't a variable 
};

//--------------------------------------------------------------------------------------------------\\

Node_value Make_val_int(int val);

Node_value Make_val_double(double val);

void Tree_construct(Tree *tree);

void Tree_destruct(Tree *Tree);

Node *Make_node(Node_type type, Node_value value, Node *left_ptr, Node *right_ptr);

Node *Make_copy(const Node *source_node);

void Node_destruct(Node *node);

void Dump(Tree *tree, const char *file_name);

void Node_dump(FILE *dump_file, Node *node);

//=====================================================================================================\\
//=====================================================================================================//

Node_value Make_val_int(int val)
{
    Node_value tmp = {};
    tmp.ival = val;
    return tmp;
}

Node_value Make_val_double(double val)
{
    Node_value tmp = {};
    tmp.dval = val;
    return tmp;
}

//--------------------------------------------------------------------------------------------------\\

void Tree_construct(Tree *tree)
{
    assert(tree != nullptr);
    
    tree->root = nullptr;
    for (int i = 0; i < tree->alphabet; i++)
        tree->var[i] = false;
}

void Tree_destruct(Tree *tree)
{
    assert(tree != nullptr);
    
    Node_destruct(tree->root);
    tree->root = nullptr;
    
    for (int i = 0; i < tree->alphabet; i++)
        tree->var[i] = false;
}

//--------------------------------------------------------------------------------------------------\\

Node *Make_node(Node_type type, Node_value value, Node *left_ptr, Node *right_ptr)
{
    Node *new_node = (Node *)calloc(1, sizeof(Node));
    
    new_node->type  = type;
    new_node->value = value;
    new_node->left  = left_ptr;
    new_node->right = right_ptr;
    
    return new_node;
}

//--------------------------------------------------------------------------------------------------\\

Node *Make_copy(const Node *source_node)
{
    if (source_node == nullptr)
        return nullptr;
    
    Node *new_node = (Node *)calloc(1, sizeof(Node));
    
    new_node->type  = source_node->type;
    new_node->value = source_node->value;
    new_node->left  = Make_copy(source_node->left);
    new_node->right = Make_copy(source_node->right);
    
    return new_node;
}

//--------------------------------------------------------------------------------------------------\\

void Node_destruct(Node *node)
{
    if (node == nullptr)
        return;
    
    Node_destruct(node->left);
    Node_destruct(node->right);
    free(node);
}

//--------------------------------------------------------------------------------------------------\\

void Dump(Tree *tree, const char *file_name)
{
    assert(tree != nullptr);
    assert(file_name != nullptr);
    
    FILE *dump_file = fopen("Dump.gv", "wb");
    fprintf(dump_file, "digraph structs {\n");
        
    Node_dump(dump_file, tree->root);
    
    fprintf(dump_file, "}\n");
    fclose(dump_file);
    
    char syst[100] = "dot -Tpdf Dump.gv -o "; 
    
    strcat(syst, file_name);
    strcat(syst, " && rm Dump.gv");
    system(syst);
}

//--------------------------------------------------------------------------------------------------\\

void Node_dump(FILE *dump_file, Node *node)
{
    if (node == nullptr)
        return;
    
    char arg[4] = {0};
    
    #define DEF_FUN(enum, num, str, len, deriv, latex_code, count) \
        case num:                                                  \
            strcpy(arg, #str);                                     \
            break;                                                 \
    
    if (node->type == OPERATOR)
    {
        switch (node->value.ival)
        {
            
        #include "functions.dsl"
        #undef DEF_FUN
        
        default:
            strcpy(arg, ":(");
            break;
        }        
    }
    else if (node->type == VAR)
        arg[0] = node->value.ival;
    
    if (node->type == NUM)
        fprintf(dump_file, "struct_%p [style = filled, fillcolor = \"#8ddfff\", label = \" %lg \"];\n", node, node->value.dval);
    else
        fprintf(dump_file, "struct_%p [style = filled, fillcolor = \"#8ddfff\", label = \" %s \"];\n", node, arg);
    
    if (node->left != nullptr)
    {
        Node_dump(dump_file, node->left);
        fprintf(dump_file, "struct_%p->struct_%p [style = bold];\n", node, node->left);
    }
    if (node->right != nullptr)
    {
        Node_dump(dump_file, node->right);
        fprintf(dump_file, "struct_%p->struct_%p [style = bold];\n", node, node->right);
    }
    return;
}

