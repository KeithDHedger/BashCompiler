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
````console
./autogen.sh --prefix=/usr
make
sudo make install ( optional ).

````  

#### Simple Usage:
````console
bashcompiler -V /PATH/TO//BASHSCRIPT >/tmp/x.cpp
g++ -Wall $(pkg-config --cflags --libs Qt5Core ) -fPIC  -Ofast /tmp/x.cpp -o APPNAME
````

The bashcompiler executable can be installed or run directly from the BashCompiler/app folder, or just copy it somewhere convienient.  
There are a number of examples in BashCompiler/resources/scripts.  

######
### Caveates:  
BASH is weakly typed so you **MUST** use "=, ==, <, or >" for string comparisons.  
You MUST use "-gt, -ge ... etc for numeric comparisons.  
All numerical expression **MUST** be enclosed by $(())  
######
'eval' is NOT supported and almost certainly will never be, not because of any supposed "eval=evil" but simply it would need the compiler available at runtime and probably a JIT compiler.  
When using redirect to a file or pipe with echo  you **MUST** precede the '>' or '|' with a space.  
When using -e/-n with echo you **MUST** use individual switches eg -n -e AND NOT -ne.  
When using loops ( eg for ((x=0;x<n;x=x++)) ) the control loop is **READ ONLY**, you shouldn't really programatically alter the control loop anyway, if you do need to you should use a 'while' loop.  
  
When using ${foo^^} be aware that BASH mistakenly uppercases any embedded '\n' to '\N'.  

Positional paramaters should be assigned to a variable BEFORE string slicing eg: 
```` console
file=${1}
echo ${file^^}
AND NOT
echo ${1^^}
````
######
All examples in BashCompiler/resources/scripts will compile and run.
######
### USAGE:
````console
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

cd
case.  
simple 'here' doc.
env variables.
Command redirection.  
Variable assignment.  
export.  
When using ${string#substring} ( #/##/%/%% ) globs must be literal strings ( for now ), ie	echo ${avar#*.abc} NOT echo ${avar#${PATTERN}}  
for ((x=0;x<n;x=x+n))  
echo, assumes quotes see example 3.  
external commands, see example 2.  
Process substitution, see example 5.  
if/then/else/fi, see 'To do', see example 6.  
while/do/done, see 'To do', see example 7.  
$? see example 7.  
While read;do ... done see example whileifread.
pushd/popd, No switches allowed ie no manipulating the stack ( yet! ) also does not op dir changes ( same as eg pushd /usr >/dev/null ).
printf, optional '-v VARNAME' FORMATSTR STR1 ... STRN, format string is ignored ( except if it ends with '\n' ) but MUST be present ( for now! ).
######

### Ongoing in no particular order. 
for loops.  
String slicing. see example stringslice.  
Add options to bashcompiler.  
Functions.
###

### To do ( loads! ) in no particular order.  

C style assign/for etc.  
read.  
arrays.  
regex.  
Backgrounding ( '&' ).  
###
All special '$'s  
Logical ops.  


