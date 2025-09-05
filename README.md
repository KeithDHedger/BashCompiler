# Bash Compiler
######
Just started so lots to do.  
Please read the Caveates section first.

######
Doesn't use AI, or obfuscation, outputs compileable C++ code.  
You should check the script first by running it in bash.  
No syntax checking ( that's BASH's job ).  
Main use is for simpler but time consuming scripts, eg recursing through a music collection.  
######
### Compile with:
````console
./autogen.sh --prefix=/usr
Or to compile with Qt5 instead of the default Qt6
USEQTVERSION=5 ./autogen.sh --prefix=/usr

make
sudo make install ( optional ).

````  

#### Simple Usage:
````console
bashcompiler -V /PATH/TO/BASHSCRIPT >/tmp/x.cpp
g++ -Wall $(pkg-config --cflags --libs Qt5Core ) -fPIC  -Ofast /tmp/x.cpp -o APPNAME

Or:
bashcompiler -V /PATH/TO/BASHSCRIPT -c /PATH/TO/FINAL/APP

All intermediate folders are created if need when using the -c switch.

Options can also be put in a config file at ~/.config/bashcompiler.rc eg:

verbose-ccode true
use-qt5 true
full-compile /tmp
lowercase-name true

Command line options will overide these.

````

The bashcompiler executable can be installed or run directly from the BashCompiler/app folder, or just copy it somewhere convienient.  
There are a number of examples in BashCompiler/resources/scripts.  

######
### Caveates:  
BASH is weakly typed so you **MUST** use "=, ==, <, or >" for string comparisons.  
You MUST use "-gt, -ge ... etc for numeric comparisons.  
All numerical expression **MUST** be enclosed by $(())  
When doing maths using the '$(( n + d ))' format **NOTE** that the opening and closing brackets **MUST** be seperated by at least one space.  

######
'eval' is NOT supported and almost certainly will never be, not because of any supposed "eval=evil" but simply it would need the compiler available at runtime and probably a JIT compiler.  
When using redirect to a file or pipe with echo  you **MUST** surround the '>', '>>' or '|' with at least one space/tab.  
When using -e/-n with echo you **MUST** use individual switches eg -n -e AND NOT -ne.  
When using loops ( eg for ((x=0;x<n;x=x++)) ) the control loop is **READ ONLY**, you shouldn't really programatically alter the control loop anyway, if you do need to you should use a 'while' loop.  
Variables and functions should **NOT** start with "\_BC\_" this is reserved for the compiler.  
  
When using ${foo^^} be aware that BASH mistakenly uppercases any embedded '\n' to '\N'.  

Positional paramaters should be assigned to a variable **BEFORE** string slicing eg: 
```` console
file=${1}
echo ${file^^}
AND NOT
echo ${1^^}

echo ${#1}
Is OK

````
read ( not read while ) will only use the LAST variable given the default REPLY is used if no variable given.  
Eg:
````
read -p "run this command ? [N/y] "
echo $REPLY

read -p "run this command ? [N/y] " ans1 ans2
echo $ans2
N.B. extra variables supplied ( ie ans1 ) will 'swallow' input.

Only stdout is redirectable to a file for now.
Pipelines of commands may ONLY redirect the LAST command to a file, if you need to do fancy redirects or redirect stderr then wrap in a shell command eg:
/bin/sh -c "ls /tmpl 2>/tmp/log|ls / &>>/tmp/log"

Creating menus with 'select' is BASH only and will ( for now ) run a seperate shell to run the command at runtime.
See the testpipesndselect example.

printf, optional '-v VARNAME' FORMATSTR STR1 ... STRN, format string is ignored ( except if it ends with '\n' ) but MUST be present ( for now! ).
For full printf functionality use the external printf ( probably /usr/bin/printf ).

Defaults MUST be in quotes so:
WRITEBYTES=${WRITEBYTES:-"128M"}
And NOT
WRITEBYTES=${WRITEBYTES:-128M}
The same goes for string slicing eg
echo ${FOO//"bar"/"baz"}
And NOT
echo ${FOO//bar/baz}

Be careful of quotes eg use this:
echo "The login/password is \"${RESULT}\""
Not this:
echo "The login/password is \""$RESULT"\""

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

Or to compile a final app:
bashcompiler -V /PATH/TO/SCRIPT -c /PATH/TO/OUTPUT/saferdd
Which will compile the final application 'saferdd' in /PATH/TO/OUTPUT, intermediate folders will be created if needed.

````
######
### Done ( mostly! )

select
read
cd
case.  
simple 'here' doc.
env _BC_variables.
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
printf   
select  
read  
for loops.  
String slicing. see example stringslice.  
Add options to bashcompiler.  
Functions.
###

### To do ( loads! ) in no particular order.  

C style assign/for etc.  
arrays.  
regex.  
Backgrounding ( '&' ).  
###
All special '$'s  
Logical ops.  


