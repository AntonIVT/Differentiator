#pragma once

#include "tree.hpp"
#include "func_enum.h"
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cctype>
#include <cstring>
 
// G := E'\0'
// E := T{[+, -] T}*
// T := S{[*, /] S}*
// S := U{^ U}*
// U := [- +]? P
// P := '('E')' | N | V | F 
// F := 'sin('E')' | 'cos('E')' | 'ln('E')'
// N := ['0' - '9']+ ? '.'['0' - '9']+]
// V := ['a' - 'z']

#define DOUBLE_(double) Make_val_double(double)
#define INT_(int) Make_val_int(int)

int FileSize(FILE *file);

void Skip(const char *&ptr);

void Tree_filling(Tree *tree, const char *expression_file);

void Get_vars(Tree *tree);

void Get_vars_node(Node *node, bool *vars);

//=====================================================================================================\\
//=====================================================================================================//

struct Parser
{
private:

    const char *ptr;
    bool error;
    
    void skip()
    {
        while (isspace(*ptr)) ptr++;
    }
    
    void syntax_error()
    {
        if (ptr != nullptr && *ptr != '\0')
            printf("Wrong syntax : <%s>\n", ptr);
        else
            printf("Wrong syntax\n");
    }
    
    bool require(const char symbol)
    {
        if (*ptr == symbol)
        {
            if (*ptr != '\0')
                ptr++;
            return true;
        }
        else
        {
            error = true;
            return false;
        }
    }
    
    // Variable
    Node *getV()
    {
        skip();        
        if ('a' <= *ptr && *ptr <= 'z')
        {
            Node *new_node = Make_node(VAR, INT_(*ptr), nullptr, nullptr);
            ptr++;
            return new_node;
        }
        else
            return nullptr;
    }
    
    // Number
    Node *getN()
    {
        char *tmp = nullptr;
                
        double dval = strtod(ptr, &tmp);
        
        if (tmp == ptr)
            return nullptr;
            
        ptr = tmp;
                
        Node *new_node = Make_node(NUM, DOUBLE_(dval), nullptr, nullptr);
        return new_node;
    }
    
    // Function
    Node *getF()
    {
        skip();
            
        const char *find = nullptr;
        Node* new_node = nullptr;
        
        #define DEF_FUN(enum, num, str, len, deriv, latex_code, count)                   \
            else if (find = strstr(ptr, #str"("), find == ptr)                           \
            {                                                                            \
                new_node = Make_node(OPERATOR, INT_(num), nullptr, nullptr);             \
                ptr += len + 1;                                                          \
                new_node->left = getE();                                                 \
            }                                                                            \
        
        if (0) {}
        #include "functions.dsl"
        #undef DEF_FUN
        else
            return nullptr;
        
        skip();
        if (!require(')'))
        {
            Node_destruct(new_node);
            return nullptr;
        }
        return new_node;
    }
    
    // Priority
    Node *getP()
    {
        skip();
        Node *new_node = nullptr;        
        
        if (*ptr == '(')
        {
            ptr++;
            new_node = getE();
            
            if (new_node == nullptr) return nullptr;
            
            skip();
            if (!require(')'))
            {
                Node_destruct(new_node);
                return nullptr;
            }
            return new_node;
        }
        else if (new_node = getN(), new_node != nullptr)
            return new_node;
            
        else if (new_node = getF(), new_node != nullptr)
            return new_node;
            
        else if (new_node = getV(), new_node != nullptr)
            return new_node;
        
        else
            return nullptr;
    }
    
    //Unary
    Node* getU()
    {
        skip();
        
        Node * maybe_node = nullptr;
        
        if (*ptr == '+')
            ptr++;
        else if (*ptr == '-')
        {
            ptr++;
            maybe_node = Make_node(OPERATOR, INT_(MUL), nullptr, nullptr);
            maybe_node->left = Make_node(NUM, DOUBLE_(-1), nullptr, nullptr);
            maybe_node->right = getP();
            return maybe_node;
        }
        
        return getP();
    }
    
    // Stepen' ak Strength
    Node* getS()
    {
        skip();
        Node *new_node_left = getU();
        skip();
        
        if (new_node_left == nullptr) return nullptr;
        
        if (*ptr == '^')                
        {                                                           
            Node *new_node = Make_node(OPERATOR, INT_(POW), new_node_left, nullptr);  
            
            ptr ++;                                                         
            new_node->right = getS();                                                 
            return new_node;                                                          
        }                                                                             
        else
            return new_node_left;   
    }
    
    // Term
    Node* getT()
    {
        skip();
        Node *new_node_left = getS();
        skip(); 
        
        if (new_node_left == nullptr) return nullptr;
        
        if (*ptr == '*' || *ptr == '/')  
        {
            int tmp = (*ptr == '*' ? MUL : DIV);                      
            Node *new_node = Make_node(OPERATOR, INT_(tmp), new_node_left, nullptr);    
            
            ptr ++;                                          
            new_node->right = getT();                                                   
            return new_node;                                                           
        }                                                                               
        else
            return new_node_left;   
    }
    
    // Expression
    Node* getE()
    {
        skip();
        Node *new_node_left = getT();
        skip();
        
        if (new_node_left == nullptr) return nullptr;
        
        if (*ptr == '+' || *ptr == '-')  
        {
            int tmp = (*ptr == '+' ? ADD : SUB);                     
            Node *new_node = Make_node(OPERATOR, INT_(tmp), new_node_left, nullptr);    
            
            ptr++;                                  
            new_node->right = getE();                                                   
            return new_node;                                                            
        }         
        else
            return new_node_left;   
    }
    
public:

    void construct()
    {
        error = false;
        ptr = nullptr;
    }
    
    Node *getG(const char *line)
    {
        assert(line != nullptr);
        
        ptr = line;
        Node *root = getE();
        
        skip();
        require('\0');
        if (root == nullptr || error)
        {
            syntax_error();
            Node_destruct(root);
            return nullptr;
        }
        
        return root; 
    }
};

//--------------------------------------------------------------------------------------------------\\

int FileSize(FILE *file)
{
    assert(file != NULL);

	int _file_size = -1;

	fseek(file, 0, SEEK_END);
	_file_size = ftell(file);

	return _file_size;
}

//--------------------------------------------------------------------------------------------------\\

void Tree_filling(Tree *tree, const char *expression_file)
{
    assert(tree != nullptr);
    assert(expression_file != nullptr);
    
    FILE *expression = fopen(expression_file, "rb");
    
    assert(expression != nullptr);
    
    int size = FileSize(expression);
    fseek(expression, 0, SEEK_SET);
    
    char * buffer = (char *)calloc(size + 1, sizeof(char));
    fread(buffer, sizeof(char), size, expression);
    fclose(expression);
    
    Parser parser = {};
    
    parser.construct();
    
    tree->root = parser.getG(buffer);
    
    free(buffer);
    
    Get_vars(tree);
}

//--------------------------------------------------------------------------------------------------\\

void Get_vars(Tree *tree)
{
    assert(tree != nullptr);
    
    Get_vars_node(tree->root, tree->var);
}

//--------------------------------------------------------------------------------------------------\\

void Get_vars_node(Node *node, bool *vars)
{
    assert(vars != nullptr);
    
    if (node == nullptr)
        return;
    
    Get_vars_node(node->left, vars);
    Get_vars_node(node->right, vars);
    
    if (node->type == VAR)
    {
        if (node->value.ival != 'e')
            vars[node->value.ival - 'a'] = true;
    }
}


