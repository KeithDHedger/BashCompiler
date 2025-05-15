# Bash Compiler
######
Just started so lots to do.
######
Doesn't use AI, or obfuscation, outputs compileable C++ code.  
You should check the script first by running it in bash.  
No syntax checking ( that's BASH's job ).  
Main use is for simpler but time consuming scripts, eg recursing through a music collection.  
######
### Compile with:
````
./autogen.sh --prefix=/usr
make
sudo make install ( optional ).
````
The bashcompiler executable can be installed or run directly from the BashCompiler/app folder, or just copy it somewhere convienient.  
######
### Caveates:  
BASH is weakly typed so you MUST use "=, ==, <, or >" for string comparisons.  
You MUST use "-gt, -ge ... etc for numeric comparisons.  
the '!' operator is available in comparisons.  
######
'eval' is NOT supported and almost certainly will never be, not because of any supposed "eval=evil" but simply it would need the compiler available at runtime and probably a JIT compiler.  
######
All examples in BashCompiler/resources/scripts will compile and run.
######
### USAGE:
````
bashcompiler /PATH/TO/SCRIPT > /tmp/x.cpp
pushd /tmp
g++ x.cpp
./a.out
popd
````
######
### Done ( mostly! )

Variable assignment.  
echo, assumes quotes see example 3.  
external commands, see example 2.  
Process substitution, see example 5.  
if/then/else/fi, see 'To do', see example 6.  
######

### Ongoing in no particular order. 
Return value from command ( '$?' ), only gives exit code of external commands, see example 6.  
while loops  
###

### To do ( loads! ) in no particular order.  

Add options to bashcompiler.  

Multi line commands, at the moment the only command that supports ';' is 'if' eg:
````
if [ VAR -gt 100 ]];then
	echo 100
fi

````

C style assign/for etc.  
read.  
case.  
for loops.  
Command redirection.  
export.  
String slicing.  
arrays.  
regex.  
Backgrounding ( '&' ).  
###
All special '$'s  
Logical ops.  


