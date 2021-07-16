# Differentiator
### Differentiation of any expressions and subsequent analysis.

## Installing
Project supported on Linux only.  
Differentiator requires pdfTeX.  
To start clone repository and make project: ``` > make main```  

## Features
* Supporting of many variables.
* Supporting exponential function(**^**), **sin**, **cos**, **ln** and **tg**. Also supported **Euler's number**.  
* Differentiator creates article (*Derivate.pdf*) in scientific-like style.
* Plots of the original function and derivate function (if expression contains one variable).
* Simplification of the expressions.
* All the steps of differentiation and simplification are reflected in the article.

## Usage
### Grammar of expression
{} - optional  
[] - obligatory  
{}\*, []\* - repeat ({}\* could be 0 times)  
* *Variable*: set of latin letters, except of **e**(Reserved - Euler's number).
* *Number*: any rational number
* *Function*: sin(*Expression*) | cos(*Expression*) | ln(*Expression*) | tg(*Expression*)
* *Priority*: (*Expression*) | *Number* | *Variable* | *Function*
* *Unary*: {+ | -} *Priority*
* *Power*: *Unary*{^ *Unary*}\*
* *Term*: *Power*{[\* | \\] *Power*}\*
* *Expression*: *Term* {[+ | -] *Term*}\*
* *Global*: *Expression* '\0'
### Program launch
1. Create expression.txt file and put there math expression.
2. Do ```./differentiator ```
3. *Derivate.pdf* is done, you can enjoy
