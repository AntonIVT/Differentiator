// enum, int, str, length, derivate

DEF_FUN(ADD, 1, +, 1,
    ADD_(dL, dR);,
    {Lat_Node(Lnode);
    Lat_Op(" + ");
    Lat_Node(Rnode);},
    Lnode + Rnode;
    )

DEF_FUN(SUB, 2, -, 1, 
    SUB_(dL, dR);,
    {Lat_Node(Lnode);
    Lat_Op(" - ");
    Lat_Node(Rnode);},
    Lnode - Rnode;
    )

DEF_FUN(MUL, 3, *, 1, 
    ADD_(MUL_(dL, cR), MUL_(cL, dR));,
    {
    if ((Lnode->type == OPERATOR && (Lnode->value.ival == ADD || Lnode->value.ival == SUB)) || (Lnode->type == NUM && Lnode->value.dval < 0))
    {
        Lat_Op("\\left(");
        Lat_Node(Lnode);
        Lat_Op("\\right)");
    }
    else
        Lat_Node(Lnode);
        
    if (Lnode->type != VAR && Rnode->type != VAR)
        Lat_Op(" \\cdot ");
    
    if ((Rnode->type == OPERATOR && (Rnode->value.ival == ADD || Rnode->value.ival == SUB)) || (Rnode->type == NUM && Rnode->value.dval < 0))
    {
        Lat_Op("\\left(");
        Lat_Node(Rnode);
        Lat_Op("\\right)");
    }
    else
        Lat_Node(Rnode);},
    Lnode * Rnode;
    )

DEF_FUN(DIV, 4, /, 1, 
    DIV_(SUB_(MUL_(dL, cR), MUL_(cL, dR)), POW_(cR, NUM_(2)));,
    {Lat_Op("\\frac{");
    Lat_Node(Lnode);
    Lat_Op("}{");
    Lat_Node(Rnode);
    Lat_Op("}");},
    Lnode / Rnode;
    )

DEF_FUN(POW, 5, ^, 1, 
    MUL_(POW_(cL, SUB_(cR, NUM_(1))), ADD_(MUL_(LN_(cL), MUL_(cL, dR)), MUL_(cR, dL)));,
    {Lat_Op("{");
    if (Lnode->type != NUM && Lnode->type != VAR)
    {
        Lat_Op("\\left(");
        Lat_Node(Lnode);
        Lat_Op("\\right)");
    }
    else
        Lat_Node(Lnode);
    Lat_Op("}^{");
    if (Rnode->type != NUM && Rnode->type != VAR)
    {
        Lat_Op("\\left(");
        Lat_Node(Rnode);
        Lat_Op("\\right)");
    }
    else
        Lat_Node(Rnode);
    Lat_Op("}");},
    pow(Lnode, Rnode);
    )

DEF_FUN(SIN, 6, sin, 3, 
    MUL_(COS_(cL), dL);,
    {Lat_Op("\\sin\\left(");
    Lat_Node(Lnode);
    Lat_Op("\\right)");},
    sin(Lnode);
    )

DEF_FUN(COS, 7, cos, 3, 
    MUL_(MUL_(SIN_(cL), NUM_(-1)), dL);,
    {Lat_Op("\\cos\\left(");
    Lat_Node(Lnode);
    Lat_Op("\\right)");},
    cos(Lnode);
    )

DEF_FUN(LN,  8, ln, 2, 
    DIV_(dL, cL);,
    {Lat_Op("\\ln\\left(");
    Lat_Node(Lnode);
    Lat_Op("\\right)");},
    log(Lnode);
    )
DEF_FUN(TG,  9, tg, 2,
    MUL_(dL, DIV_(NUM_(1), POW_(COS_(cL), NUM_(2)))),
    {Lat_Op("\\tg\\left(");
    Lat_Node(Lnode);
    Lat_Op("\\right)");},
    tan(Lnode);
    )
