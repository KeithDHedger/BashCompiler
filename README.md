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
All numerical expression MUST be enclosed by $(())  
######
'eval' is NOT supported and almost certainly will never be, not because of any supposed "eval=evil" but simply it would need the compiler available at runtime and probably a JIT compiler.  
######
All examples in BashCompiler/resources/scripts will compile and run.
######
### USAGE:
````
bashcompiler /PATH/TO/SCRIPT > /tmp/x.cpp
Or nice output:
bashcompiler /PATH/TO/SCRIPT| astyle -A7 --indent=tab > /tmp/x.cpp
pushd /tmp
g++ -Wall $(pkg-config --cflags --libs Qt5Core ) -fPIC -Ofast x.cpp
./a.out
popd
````
######
### Done ( mostly! )

Variable assignment.  
When using ${string#substring} ( #/##/%/%% ) globs must be literal strings ( for now ), ie	echo ${avar#*.abc} NOT echo ${avar#${PATTERN}}  

echo, assumes quotes see example 3.  
external commands, see example 2.  
Process substitution, see example 5.  
if/then/else/fi, see 'To do', see example 6.  
while/do/done, see 'To do', see example 7.  
$? see example 7.  
While read;do ... done see example whileifread ( ONLY default $REPLY variable used for now ).
######

### Ongoing in no particular order. 
String slicing. see example stringslice.  
Add options to bashcompiler.  
###

### To do ( loads! ) in no particular order.  

env variables.
C style assign/for etc.  
read.  
case.  
for loops.  
Command redirection.  
export.  
arrays.  
regex.  
Backgrounding ( '&' ).  
###
All special '$'s  
Logical ops.  


