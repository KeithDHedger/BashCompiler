#!/bin/bash

test="AbC 1234 def XyZ"
test2="aBc 1234 def xYz"
len=4

xx=${xx:-${test}}
echo ${xx}

xx=
xx=${xx:-"this is a test"}
echo ${xx}

xx=
xx=${xx:-${test:2:5}}
echo ${xx}

xx=
xx=${xx:-${test:2:${len}}}
echo ${xx}

xx=
xx=${xx:-${test,}}
echo ${xx}

xx=
xx=${xx:-${test,,}}
echo ${xx}

xx=
xx=${xx:-${test2^}}
echo ${xx}

xx=
xx=${xx:-${test2^^}}
echo ${xx}
#TODO#
#echo $xx

foo=test2
echo ${!foo}
