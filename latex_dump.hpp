#pragma once
 
#include "tree.hpp"
#include "func_enum.h"
#include <cstdio>
#include <cassert>
#include <cmath>
 
#define Lnode node->left
#define Rnode node->right
 
#define Lat_Node(node) Latex_node(latex, node)
#define Lat_Op(str)    fprintf(latex, str)

const char* const simp_words[] = {"Попробуем упростить это нереально сложное выражение:",
                                  "Далее необходимо перестроить следующее выражение:",
                                  "Следующее выражение можно упростить, что мы и сделаем:",
                                  "Видно, что это выражение легко упрощается:"};

const char* const success_words[] = {"Это было сложно, но вот, что получилось:",
                                     "Что-ж, некоторые преобразования привели выражение к такому виду:",
                                     "Машина думает...........итоговое выражение:",
                                     "Бип..боп..бип..преобразую ваше выражение...готово:"};
                                                      
const char* const deriv_words[] = {"Давайте продифференцируем эту нереально сложную функцию:",
                                   "Хм...надо взять производную этого выражения:",
                                   "Производная следующего выражения очевидна, так что мы предлагаем читателю самому найти её",
                                   "Дифференцирование вещь сложная, так что пусть это сделает компьютер, не так ли?"
                                   "Например, пусть продифференцирует это выражение:"};

void Print_before(FILE *latex, const Node *node);

void Print_after(FILE* latex, const Node *node);

void Print_node(FILE *latex, const Node *node, const char *word);

void Latex_tree(FILE* latex, const Tree* tree);

void Latex_node(FILE* latex, const Node *node);

void Latex_node_op(FILE* latex, const Node *node);

void Make_graph_tree(Tree *tree, FILE *latex, const char *graphic_name);

void Print_coords(Tree *tree, FILE *latex, double vars[], int main_var);

double Count(Tree *tree, double alphabet[]);

double Count_node(Node *node, double *alphabet);

//=====================================================================================================\\
//=====================================================================================================//

void Print_before(FILE *latex, const Node *node)
{
    if (latex == nullptr)
        return;
    
    int rand_num = rand() % (sizeof(simp_words) / sizeof(char *));
    Print_node(latex, node, simp_words[rand_num]);
}

//--------------------------------------------------------------------------------------------------\\

void Print_after(FILE* latex, const Node *node)
{
    if (latex == nullptr)
        return;
    
    int rand_num = rand() % (sizeof(success_words) / sizeof(char *));
    Print_node(latex, node, success_words[rand_num]);
}

//--------------------------------------------------------------------------------------------------\\

void Print_der(FILE* latex, const Node *node)
{
    if (latex == nullptr)
        return;
    
    int rand_num = rand() % (sizeof(deriv_words) / sizeof(char *));
    Print_node(latex, node, deriv_words[rand_num]);
}

//--------------------------------------------------------------------------------------------------\\

void Print_node(FILE *latex, const Node *node, const char *const word)
{
    if (latex == nullptr)
        return;
    
    assert(node != nullptr);
    assert(word != nullptr);
    
    fprintf(latex, "%s\n"
                   "$$ ", word);
                   
    Latex_node(latex, node);
    fprintf(latex, " $$\n");
}
//--------------------------------------------------------------------------------------------------\\

void Latex_tree(FILE* latex, const Tree* tree)
{
    assert(tree != nullptr);
    
    fprintf(latex, "$$");
    Latex_node(latex, tree->root);
    
    fprintf(latex, " $$\n");
}

//--------------------------------------------------------------------------------------------------\\

void Latex_node(FILE* latex, const Node *node)
{
    assert(latex != nullptr);
    assert(node != nullptr);
    
    switch (node->type)
    {
        case OPERATOR:
            Latex_node_op(latex, node);
            break;
        case VAR:
            fprintf(latex, "%c", node->value.ival);
            break;
        case NUM:
            fprintf(latex, "%lg", node->value.dval);
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------------------------------------------\\

void Latex_node_op(FILE* latex, const Node *node)
{   
    #define DEF_FUN(enum, num, str, len, deriv, latex_code, count) \
    case num:                                                      \
    {                                                              \
        latex_code;                                                \
        break;                                                     \
    }      
    
    switch (node->value.ival)
    {
    #include "functions.dsl"
    #undef DEF_FUN
    
    default:
        break;
    }
}

//--------------------------------------------------------------------------------------------------\\

void Make_graph_tree(Tree *tree, FILE *latex, const char *graphic_name)
{
    assert(tree  != nullptr);
    assert(latex != nullptr);
    
    double vars[tree->alphabet] = {0.};
    vars['e' - 'a'] = 2.718281828459045;
    
    for (int main_var = 0; main_var < tree->alphabet; main_var++)
    {
        if (tree->var[main_var])
        {
            fprintf(latex, "{\\large \\textbf{График функции относительно переменной %c:}}\\newline \n", 'a' + main_var);
            for (int sub_var = 0; sub_var < tree->alphabet; sub_var++)
            {
                if (tree->var[sub_var] && sub_var != main_var)
                {
                    printf("Введите переменную %c для \"%s\" относительно переменной %c \n", 'a' + sub_var, graphic_name, 'a' + main_var);
                    scanf("%lg", vars + sub_var);
                    fprintf(latex, "\\textbf{%c = %lg}\\newline \n", 'a' + sub_var, vars[sub_var]);
                }
            }
            
            fprintf(latex, "\\begin{tikzpicture}\n"
                           "\\begin{axis}[\n"
                           "title = %s,\n"
                           "xlabel = {$%c$},\n"
                           "ylabel = {$f(%c)$},\n]\n"
                           "\\addplot[graph] coordinates {\n", graphic_name,'a' + main_var, 'a' + main_var);
            
            Print_coords(tree, latex, vars, main_var);
                               
            fprintf(latex, "};\n"
                           "\\end{axis}\n"
                           "\\end{tikzpicture}\n"
                           "\\newline\n");
        }
    }
}

//--------------------------------------------------------------------------------------------------\\

void Print_coords(Tree *tree, FILE *latex, double vars[], int main_var)
{
    assert(tree  != nullptr);
    assert(latex != nullptr);
    assert(vars  != nullptr);
    
    double step  = 0.01,
           begin = -20,
           end   =  20;
    
    for (double x = begin; x <= end; x += step)
    {
        vars[main_var] = x;
        double y = Count(tree, vars);
        
        if (y <= 500 && y >= -500)
            fprintf(latex, "(%lg, %lg) \n", x, y);
    }
    fprintf(latex, "\n");
}

//--------------------------------------------------------------------------------------------------\\

double Count(Tree *tree, double alphabet[])
{
    assert(tree     != nullptr);
    assert(alphabet != nullptr);
    
    return Count_node(tree->root, alphabet);
}

//--------------------------------------------------------------------------------------------------\\

#undef Lnode
#undef Rnode

#define Lnode Count_node(node->left,  alphabet)
#define Rnode Count_node(node->right, alphabet)

double Count_node(Node *node, double *alphabet)
{
    if (node == nullptr) return 0.;
    
    switch (node->type)
    {
    case OPERATOR:
        
        #define DEF_FUN(enum, num, str, len, deriv, latex_code, count) \
        case num:                                                      \
            return count;                                              \
            break;      
        
        switch (node->value.ival)
        {
            #include "functions.dsl"
            
            default:
                return 0.;
                break;
        }
        #undef DEF_FUN 
        break;
    case NUM:
        return node->value.dval;
        break;
    case VAR:
        return alphabet[node->value.ival - 'a'];
        break;
    default:
        return 0.;
        break;
    }
}

#undef Lnode
#undef Rnode
