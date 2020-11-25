#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cctype>

//--------------------------------------------------------------------------------------------------\\

enum node_types
{
    NUM = 1,
    VAR = 2,
    OPERATOR = 3
};

enum operators
{
    ADD = 1,
    SUB = 2,
    MUL = 3,
    DIV = 4,
};

//--------------------------------------------------------------------------------------------------\\

struct Node
{
    int type;
    double value;
    Node *left;
    Node *right;
};

struct Diff_tree
{
    Node *root;  
};

//--------------------------------------------------------------------------------------------------\\

int FileSize(FILE *file);

void skipper(char *&ptr);

void Tree_filling(Diff_tree *tree);

Node *fill_node(char *&ptr);

void dump(Diff_tree *tree);

void node_dump(FILE *dump_file, Node *node);

//=====================================================================================================\\
//======================================================================================================||
//=====================================================================================================//

int FileSize(FILE *file)
{
    assert(file != NULL);

	int _file_size = -1;

	fseek(file, 0, SEEK_END);
	_file_size = ftell(file);

	return _file_size;
}

//--------------------------------------------------------------------------------------------------\\

void skipper(char *&ptr)
{
    while (isspace(*ptr)) ptr++;
}

//--------------------------------------------------------------------------------------------------\\

void Tree_filling(Diff_tree *tree)
{
    assert(tree != nullptr);
    
    FILE * expr = fopen("expression.txt", "rb");
    
    int size = FileSize(expr);
    fseek(expr, 0, SEEK_SET);
    
    char * buffer = (char *)calloc(size + 1, sizeof(char));
    fread(buffer, sizeof(char), size, expr);
    fclose(expr);
    
    char *ptr = buffer;
    
    tree->root = fill_node(ptr);
    
    free(buffer);
}

//--------------------------------------------------------------------------------------------------\\

Node *fill_node(char *&ptr)
{
    assert(ptr != nullptr);
        
    skipper(ptr);
    
    Node *new_node = (Node *)calloc(1, sizeof(Node));
    
    if (*ptr == '(')
    {
        ptr++;
        
        new_node->left = fill_node(ptr);
        skipper(ptr);
        new_node->type = OPERATOR;
        
        switch (*ptr)
        {
        case '+':
            new_node->value = ADD;
            break;
        case '-':
            new_node->value = SUB;
            break;
        case '*':
            new_node->value = MUL;
            break;
        case '/':
            new_node->value = DIV;
            break;
        default:
            break;
        }
        
        ptr++;
        
        new_node->right = fill_node(ptr);
        
        skipper(ptr);
        if (*ptr == ')')
        {
            ptr++;
            return new_node;
        }
        else
        {
            printf("Sorry, wrong symbol in expression : <%c> | <%i>\n", *ptr, *ptr);
            return new_node;
        }
    }
    else if (isdigit(*ptr))
    {
        new_node->type = NUM;
        sscanf(ptr, "%lg", &new_node->value);
        while(isdigit(*ptr) || *ptr == '.') ptr++;
        
        return new_node;
    }
    else if (*ptr == 'x')
    {
        new_node->type = VAR;
        new_node->value = 1;
        ptr++;
        
        return new_node;
    }
    else
    {
        printf("Sorry, wrong symbol in expression : <%c> | <%i>\n", *ptr, *ptr);
        free(new_node);
        return nullptr;
    }
}

//--------------------------------------------------------------------------------------------------\\

void dump(Diff_tree *tree)
{
    assert(tree != nullptr);
    
    FILE *dump_file = fopen("Dump.gv", "wb");
    fprintf(dump_file, "digraph structs {\n");
        
    node_dump(dump_file, tree->root);
    
    fprintf(dump_file, "}\n");
    fclose(dump_file);
    
    system("dot -Tpdf Dump.gv -o Dump.pdf && rm Dump.gv");
}

void node_dump(FILE *dump_file, Node *node)
{
    if (node == nullptr)
        return;
    
    
    char arg = 0;
    if (node->type == OPERATOR)
    {
        switch ((int)node->value)
        {
        case ADD:
            arg = '+';
            break;
        case SUB:
            arg = '-';
            break;
        case MUL:
            arg = '*';
            break;
        case DIV:
            arg = '/';
            break;
        default:
            arg = 38;
            break;
        }
    }
    else if (node->type == VAR)
        arg = 'x';
    
    if (node->type == NUM)
        fprintf(dump_file, "struct_%p [style = filled, fillcolor = \"#8ddfff\", label = \" %lg \"];\n", node, node->value);
    else
        fprintf(dump_file, "struct_%p [style = filled, fillcolor = \"#8ddfff\", label = \" %c \"];\n", node, arg);
    
    if (node->left != nullptr)
    {
        node_dump(dump_file, node->left);
        fprintf(dump_file, "struct_%p->struct_%p [style = bold];\n", node, node->left);
    }
    if (node->right != nullptr)
    {
        node_dump(dump_file, node->right);
        fprintf(dump_file, "struct_%p->struct_%p [style = bold];\n", node, node->right);
    }
    return;
}