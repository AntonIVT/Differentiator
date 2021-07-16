#pragma once

#include "tree.hpp"
#include "latex_dump.hpp"

#define DOUBLE_(double) Make_val_double(double)
#define INT_(int) Make_val_int(int)

#define dL Derivate_node(latex, source_node->left, var)
#define cL Make_copy(source_node->left)
 
#define dR Derivate_node(latex, source_node->right, var)
#define cR Make_copy(source_node->right)

#define NUM_(double) Make_node(NUM, DOUBLE_(double), nullptr, nullptr)

void Derivate_tree(FILE *latex, Tree *diff_tree, const Tree *source_tree, char var);

Node *Derivate_node(FILE *latex, const Node *source_node, char var);

//=====================================================================================================\\
//=====================================================================================================//

void Derivate_tree(FILE *latex, Tree *diff_tree, const Tree *source_tree, char var)
{
    assert(diff_tree != nullptr);
    assert(source_tree != nullptr);
    
    diff_tree->root = Derivate_node(latex, source_tree->root, var);
}

//--------------------------------------------------------------------------------------------------\\

#define DEF_FUN(enum, num, str, len, deriv, latex_code, count)                                      \
Node * enum ## _ (Node *left_node = nullptr, Node *right_node = nullptr) {return Make_node(OPERATOR, INT_(num), left_node, right_node);}
#include "functions.dsl"
#undef DEF_FUN

//--------------------------------------------------------------------------------------------------\\

Node *Derivate_node(FILE *latex, const Node *source_node, char var)
{   
    if (source_node == nullptr)
        return nullptr;
    
    Node *tmp_node = nullptr;
    
    switch(source_node->type)
    {
        case NUM:
            Print_der(latex, source_node);
            tmp_node = NUM_(0);
            Print_after(latex, tmp_node);
            return tmp_node;
            break;
            
        case VAR:
            Print_der(latex, source_node);
            if (source_node->value.ival == var && source_node->value.ival != 'e')
            {
                tmp_node = NUM_(1);
                Print_after(latex, tmp_node);
                return tmp_node;
            }
            else
            {
                tmp_node = NUM_(0);
                Print_after(latex, tmp_node);
                return tmp_node;
            }
            break;  
                
        case OPERATOR:
        {
            #define DEF_FUN(enum, num, str, len, deriv, latex_code, count)  \
            case num:                                               \
                tmp_node = deriv;                                    \
                Print_der(latex, source_node);                      \
                Print_after(latex, tmp_node);                       \
                return tmp_node;                                    \
                break;
                  
            switch(source_node->value.ival)
            {
            #include "functions.dsl"
            #undef DEF_FUN
                
            default:
                return nullptr;
                break;
            }
        }
        default:
            return nullptr;
            break;   
    }
}
