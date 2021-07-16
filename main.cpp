#include "src/parser.hpp"
#include "src/tree.hpp"
#include "src/derivate.hpp"
#include "src/optimization.hpp"
#include "src/latex_dump.hpp"
#include <cstring>

const int MAX_LINE_LENGTH = 100;

Tree *Make_derivate(Tree *tree, FILE *latex)
{
    Tree *diff_tree = (Tree *)calloc(tree->alphabet, sizeof(Tree));

    for (int i = 0; i < tree->alphabet; i++)
        Tree_construct(diff_tree + i);

    for (int i = 0; i < tree->alphabet; i++)
    {
        if (tree->var[i] == true)
        {
            diff_tree[i].var[i] = true;
            fprintf(latex, "\\begin{center} {\\Large \\textbf{Продифференцируем по переменной %c:}} \\end{center}\n", 'a' + i);
            Derivate_tree(latex, diff_tree + i, tree, 'a' + i);
            fprintf(latex, "{\\large \\textbf{Итоговая производная по переменной %c:}}\n", 'a' + i);
            Latex_tree(latex, diff_tree + i);

            Simplify(latex, diff_tree + i);
            fprintf(latex, "{\\large \\textbf{Упрощенная версия производной:}}\n");
            Latex_tree(latex, diff_tree + i);

            Get_vars(diff_tree + i);

            char line[MAX_LINE_LENGTH] = "";
            sprintf(line, "График производной по переменной %c", 'a' + i);

            Make_graph_tree(diff_tree + i, latex, line);

            fprintf(latex, "\\vspace*{4em} \n");
        }
    }

    return diff_tree;
}

int main()
{
    Tree tree = {0};
    Tree_construct(&tree);

    Tree_filling(&tree, "expression.txt");

    FILE *latex = fopen("Derivate.tex", "wb");

    assert(latex != nullptr);

    fprintf(latex, "\\documentclass{article}\n"
                   "\\usepackage[T2A]{fontenc}\n"
                   "\\usepackage[utf8]{inputenc}\n"
                   "\\usepackage[english,russian]{babel}\n"
                   "\\usepackage[a4paper, total={6in, 10in}]{geometry}\n"
                   "\\usepackage{pgfplots}\n"
                   "\\usepackage{hyperref}\n"
                   "\\pgfplotsset{compat=1.9}\n"
                   "\\usepackage{xcolor}\n"
                   "\\definecolor{linkcolor}{HTML}{0000FF}\n"
                   "\\definecolor{urlcolor}{HTML}{0000FF}\n"
                   "\\hypersetup{pdfstartview=FitH,  linkcolor=linkcolor,urlcolor=urlcolor, colorlinks=true}\n"
                   "\\usepackage{fancyhdr}\n"
                   "\\pagestyle{fancy}\n"
                   "\\fancyhf{}\n"
                   "\\fancyfoot[R]{\\thepage}\n"
                   "\\renewcommand{\\headrulewidth}{0pt}\n"
                   "\\renewcommand{\\footrulewidth}{0pt}\n"
                   "\\begin{document}\n"

                   "\\begin{center}\n"
                   "\\vspace*{10em}\n"
                   "\\textbf{{\\huge ANTON-ALPHA}}\\\\ \n"
                   "\\vspace*{4em}\n"
                   "{\\Large Доклад-реферат-робот-дифференциатор{\\scriptsize-миллиардер-филантроп}}\\\\ \n"
                   "\\vspace*{2em}\n"
                   "{\\Large Создан, чтобы брать производные любых\\footnote{На самом деле только тех, которые поместятся в строку \\LaTeX{}'а} выражений!}\\\\ \n"
                   "\\end{center}\n"
                   "\\vspace*{20em}\n"
                   "\\begin{flushright} \\textcolor{blue}{Автор - Манакин Антон Б05-033{\\scriptsize (господи, горе ему...)}} \\end{flushright} \n"
                   "\\vspace*{1em}\n"
                   "{\\large \\qquad То, что вы видите на своем экране было сделано людьми разных культур и национальностей, авторы выступают резко против какого-либо "
                   "вида расизма и старались кого-то обидеть. Данная программа вычисляет производные функции по всем переменным, "
                   "и рисует графики. Также это нечто специально(а может и нет, лол) пишет наиболее подробные "
                   "выкладки при вычислении и упрощении производных.} \\newline \\noindent \n"
                   "\\null {\\large \\qquad Отче наш, сущий на небесах! Да святится имя Твоё; да приидет Царствие Твоё; да будет воля Твоя и на земле, как на небе; "
                   "хлеб наш насущный дай нам на сей день; и прости нам долги наши, как и мы прощаем должникам нашим; и не введи нас в искушение, но избавь "
                   "нас от лукавого. Ибо Твоё есть Царство и сила и слава во веки. Аминь.}\\\\ \n"
                   "\\newpage\n"
                   "\\pgfplotsset{graph/.style = {blue}}\n");

    fprintf(latex, "\\begin{center} {\\Large \\textbf{Изначальное выражение, которое нужно продифференцировать:}}\n");
    Latex_tree(latex, &tree);

    Simplify(latex, &tree);
    fprintf(latex, "{\\large \\textbf{Упрощенная версия исходного выражения:}}\n");
    Latex_tree(latex, &tree);

    Make_graph_tree(&tree, latex, "График изначальной функции");

    fprintf(latex, "\\vspace*{4em} \n");

    Tree *diff_trees = Make_derivate(&tree, latex);

    fprintf(latex, "\\newline {\\large \\textbf{Список литературы:}}\\newline \n"
                   "\\begin{itemize}\n"
                   "\\item GitHub \\href{https://github.com/amanakin/Differentiator}{https://github.com/amanakin/Differentiator}\n"
                   "\\item Дединский И.Р. Искусство программирования, 2020 г. - 228 с.\n"
                   "\\item Левий Матфей \\href{http://bibliya-online.ru/chitat-evangelie-ot-matfey-onlayn/}{Евангелие от Матфея}, 50 г. - 6:9 - 13\n"
                   "\\item Путин В.В \\href{https://www.kommersant.ru/doc/1866753}{Демократия и качество государства}, 2012 г.\n"
                   "\\item Прокопенко И.С. \\href{https://monster-book.com/reader/16959}{Страшное дело. Рептилоиды нашего времени}, 2018 г.\n"
                   "\\end{itemize}\n"
                   "\n\\end{center}\n"
                   "\\end{document} \n");
    fclose(latex);

    for (int i = 0; i < tree.alphabet; i++)
        Tree_destruct(diff_trees + i);

    free(diff_trees);

    Tree_destruct(&tree);

    system("pdflatex Derivate.tex > /dev/null  && rm Derivate.tex && rm Derivate.aux && rm Derivate.log");
}
