#pragma once

#include "tree.hpp"
#include "func_enum.h"
#include "latex_dump.hpp"
#include "parser.hpp"

Node *Shrink(FILE *latex, Node *node, bool &isChanged);

Node *Fix_mul(FILE *latex, Node *node, bool &isChanged);

Node *Fix_add(FILE *latex, Node *node, bool &isChanged);

Node *Fix_sin(FILE *latex, Node *node, bool &isChanged);

Node *Fix_pow(FILE *latex, Node *node, bool &isChanged);

Node *Fix_ln(FILE *latex, Node *node, bool &isChanged);

Node *Function_optimize(FILE *latex, Node *node, bool &isChanged);

void Simplify(FILE *latex, Tree *tree);

//--------------------------------------------------------------------------------------------------\\

Node *Shrink(FILE *latex, Node *node, bool &isChanged)
{
    assert(node != nullptr);

    if (node->type != OPERATOR)
        return node;

    switch (node->value.ival)
    {
    case ADD:
    case SUB:
    case MUL:
    case DIV:
    case POW:
        if (node->left->type == NUM && node->right->type == OPERATOR)
        {
            if (node->value.ival == ADD && node->right->value.ival == ADD)
            {
                if ((node->right)->left->type == NUM)
                {
                    Node *tmp = (node->right)->right;
                    node->left->value.dval = node->left->value.dval + (node->right)->left->value.dval;
                    free((node->right)->left);
                    free(node->right);
                    node->right = tmp;
                }
            }
            if (node->value.ival == MUL && node->right->value.ival == MUL)
            {
                if ((node->right)->left->type == NUM)
                {
                    Node *tmp = (node->right)->right;
                    node->left->value.dval = node->left->value.dval * (node->right)->left->value.dval;
                    free((node->right)->left);
                    free(node->right);
                    node->right = tmp;
                }
            }
        }

        if (node->left->type == OPERATOR)
            node->left = Shrink(latex, node->left, isChanged);
        if (node->right->type == OPERATOR)
            node->right = Shrink(latex, node->right, isChanged);

        if (node->right->type == NUM && node->left->type == NUM)
        {
            Print_before(latex, node);

            switch (node->value.ival)
            {
            case ADD:
                node->value.dval = node->left->value.dval + node->right->value.dval;
                break;
            case SUB:
                node->value.dval = node->left->value.dval - node->right->value.dval;
                break;
            case MUL:
                node->value.dval = node->left->value.dval * node->right->value.dval;
                break;
            case DIV:
                node->value.dval = node->left->value.dval / node->right->value.dval;
                break;
            case POW:
                node->value.dval = pow(node->left->value.dval, node->right->value.dval);
            default:
                break;
            }

            node->type = NUM;
            free(node->left);
            free(node->right);
            node->right = nullptr;
            node->left = nullptr;
            isChanged = true;
            Print_after(latex, node);
            return node;
        }
        return node;
        break;
    default:
        node->left = Shrink(latex, node->left, isChanged);
        return node;
        break;
    }
}

//--------------------------------------------------------------------------------------------------\\

Node *Fix_mul(FILE *latex, Node *node, bool &isChanged)
{
    assert(node != nullptr);

    if ((node->left->type == NUM && node->left->value.dval == 0.) ||
        (node->right->type == NUM && node->right->value.dval == 0.))
    {
        Print_before(latex, node);
        node->type = NUM;
        node->value.dval = 0.;
        Node_destruct(node->left);
        Node_destruct(node->right);
        node->left = nullptr;
        node->right = nullptr;
        isChanged = true;
        Print_after(latex, node);
        return node;
    }
    if (node->left->type == NUM && node->left->value.dval == 1. && node->value.ival != DIV)
    {
        Print_before(latex, node);
        Node *tmp = node->right;
        free(node->left);
        free(node);
        isChanged = true;
        Print_after(latex, tmp);
        return tmp;
    }
    if (node->right->type == NUM && node->right->value.dval == 1.)
    {
        Print_before(latex, node);
        Node *tmp = node->left;
        free(node->right);
        free(node);
        isChanged = true;
        Print_after(latex, tmp);
        return tmp;
    }
    if (node->value.ival == MUL && node->left->type == VAR)
    {
        if (node->right->type == VAR && node->left->value.ival == node->right->value.ival)
        {
            node->value.ival = POW;
            free(node->right);
            node->right = Make_node(NUM, DOUBLE_(2), nullptr, nullptr);
            isChanged = true;
            return node;
        }
        if (node->right->type == OPERATOR && node->right->value.ival == POW && (node->right)->left->type == VAR &&
            (node->right)->right->type == NUM && node->left->value.ival == (node->right)->left->value.ival)
        {
            Node *tmp = node->right;
            free(node->left);
            free(node);
            tmp->right->value.dval += 1;
            isChanged = true;
            return tmp;
        }
        else
            return node;
    }
    else
        return node;
}

//--------------------------------------------------------------------------------------------------\\

Node *Fix_add(FILE *latex, Node *node, bool &isChanged)
{
    assert(node != nullptr);

    if (node->left->type == NUM && node->left->value.dval == 0.)
    {
        if (node->value.ival == SUB)
        {
            if (node->right->type == NUM)
            {
                Print_before(latex, node);
                Node *tmp = node->right;
                tmp->value.dval *= (-1);
                free(node->left);
                free(node);
                isChanged = true;
                Print_after(latex, tmp);
                return tmp;
            }
            else
            {
                node->value.ival = MUL;
                node->left->value.dval = -1.;
                return node;
            }
        }
        else
        {
            Print_before(latex, node);
            Node *tmp = node->right;
            free(node->left);
            free(node);
            isChanged = true;
            Print_after(latex, tmp);
            return tmp;
        }
    }
    if (node->right->type == NUM && node->right->value.dval == 0.)
    {
        Print_before(latex, node);
        Node *tmp = node->left;
        free(node->right);
        free(node);
        isChanged = true;
        Print_after(latex, tmp);
        return tmp;
    }
    if (node->right->type == NUM && node->right->value.dval < 0)
    {
        if (node->value.ival == SUB)
        {
            node->value.ival = ADD;
            node->right->value.dval *= (-1);
        }
        else
        {
            node->value.ival = SUB;
            node->right->value.dval *= (-1);
        }
        isChanged = true;
        return node;
    }
    if (node->value.ival == ADD && node->left->type == VAR)
    {
        if (node->right->type == VAR && node->left->value.ival == node->right->value.ival)
        {
            node->value.ival = MUL;
            free(node->left);
            node->left = Make_node(NUM, DOUBLE_(2), nullptr, nullptr);
            isChanged = true;
            return node;
        }
        if (node->right->type == OPERATOR && node->right->value.ival == MUL && (node->right)->left->type == NUM &&
            (node->right)->right->type == VAR && node->left->value.ival == (node->right)->right->value.ival)
        {
            Node *tmp = node->right;
            free(node->left);
            free(node);
            tmp->left->value.dval += 1;
            isChanged = true;
            return tmp;
        }
        else
            return node;
    }
    else
        return node;
}

//--------------------------------------------------------------------------------------------------\\

Node *Fix_sin(FILE *latex, Node *node, bool &isChanged)
{
    assert(node != nullptr);

    if (node->left->type == NUM && node->left->value.dval == 0.)
    {
        Print_before(latex, node);
        free(node->left);
        node->value.dval = 0.;
        node->type = NUM;
        isChanged = true;
        Print_after(latex, node);
        return node;
    }
    else
        return node;
}

//--------------------------------------------------------------------------------------------------\\

Node *Fix_pow(FILE *latex, Node *node, bool &isChanged)
{
    assert(node != nullptr);

    if (node->right->type == NUM)
    {
        if (node->right->value.dval == 0.)
        {
            Print_before(latex, node);
            Node_destruct(node->left);
            free(node->right);
            node->type = NUM;
            node->value.dval = 1.;
            isChanged = true;
            Print_after(latex, node);
            return node;
        }
        else if (node->right->value.dval == 1.)
        {
            Print_before(latex, node);
            Node *tmp = node->left;
            free(node->right);
            free(node);
            isChanged = true;
            Print_after(latex, tmp);
            return tmp;
        }
    }
    else if (node->left->type == NUM)
    {
        if (node->left->value.dval == 0.)
        {
            Print_before(latex, node);
            Node_destruct(node->right);
            free(node->left);
            node->type = NUM;
            node->value.dval = 0.;
            isChanged = true;
            Print_after(latex, node);
            return node;
        }
        else if (node->left->value.dval == 1.)
        {
            Print_before(latex, node);
            Node_destruct(node->right);
            free(node->left);
            node->type = NUM;
            node->value.dval = 1.;
            isChanged = true;
            Print_after(latex, node);
            return node;
        }
    }
    return node;
}

//--------------------------------------------------------------------------------------------------\\

Node *Fix_ln(FILE *latex, Node *node, bool &isChanged)
{
    assert(node != nullptr);

    if (node->left->type == NUM && node->left->value.dval == 1.)
    {
        Print_before(latex, node);
        free(node->left);
        node->type = NUM;
        node->value.dval = 0.;
        node->left = nullptr;
        Print_after(latex, node);
        isChanged = true;
        return node;
    }
    else if (node->left->type == VAR && node->left->value.ival == 'e')
    {
        Print_before(latex, node);
        free(node->left);
        node->type = NUM;
        node->value.dval = 1.;
        node->left = nullptr;
        Print_after(latex, node);
        isChanged = true;
        return node;
    }
    else
        return node;
}

//--------------------------------------------------------------------------------------------------\\

Node *Rotate_node(Node *node, bool &isChanged)
{
    assert(node != nullptr);

    if (node->right != nullptr)
    {
        if ((node->value.ival == ADD || node->value.ival == MUL) && node->right->type == NUM && node->left->type != NUM)
        {
            Node *tmp = node->left;
            node->left = node->right;
            node->right = tmp;
            isChanged = true;
        }
        else if (node->value.ival == ADD && node->right->type == OPERATOR && node->right->value.ival == ADD)
        {
            if ((node->right)->left->type == NUM && node->left->type != NUM)
            {
                Node *tmp = (node->right)->left;
                (node->right)->left = node->left;
                node->left = tmp;
                isChanged = true;
            }
        }
        else if (node->value.ival == MUL && node->right->type == OPERATOR && node->right->value.ival == MUL)
        {
            if ((node->right)->left->type == NUM && node->left->type != NUM)
            {
                Node *tmp = (node->right)->left;
                (node->right)->left = node->left;
                node->left = tmp;
                isChanged = true;
            }
        }
    }
    return node;
}

//--------------------------------------------------------------------------------------------------\\

Node *Function_optimize(FILE *latex, Node *node, bool &isChanged)
{
    assert(node != nullptr);

    if (node->type != OPERATOR)
        return node;

    if (node->left != nullptr)
        node->left = Function_optimize(latex, node->left, isChanged);
    if (node->right != nullptr)
        node->right = Function_optimize(latex, node->right, isChanged);

    if (node->value.ival == ADD || node->value.ival == SUB)
        node = Fix_add(latex, node, isChanged);
    if (node->type == OPERATOR && (node->value.ival == MUL || node->value.ival == DIV))
        node = Fix_mul(latex, node, isChanged);
    if (node->type == OPERATOR && node->value.ival == POW)
        node = Fix_pow(latex, node, isChanged);
    if (node->type == OPERATOR && node->value.ival == SIN)
        node = Fix_sin(latex, node, isChanged);
    if (node->type == OPERATOR && node->value.ival == LN)
        node = Fix_ln(latex, node, isChanged);
    if (node->type == OPERATOR)
        node = Rotate_node(node, isChanged);

    return node;
}

//--------------------------------------------------------------------------------------------------\\

void Simplify(FILE *latex, Tree *tree)
{
    assert(latex != nullptr);
    assert(tree != nullptr);

    bool isChanged = true;
    while (isChanged == true)
    {
        isChanged = false;
        tree->root = Shrink(latex, tree->root, isChanged);
        tree->root = Function_optimize(latex, tree->root, isChanged);
    }
}
