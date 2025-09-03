#!/bin/bash

numbers="1234567890"
data="abc 123 xyz"
from=2
len=4

xx=${data:${from}:${len}}
echo "$data"
echo "$xx"
echo "${data:${from}}"

slice=${data:5:1}
echo "$slice"
zz=${data:${from}:${slice}}
echo "$zz"

zz1=${data:${from}:${data:6:1}}
echo "$zz1"

zz2=${data:${data:6:1}:${data:6:1}}
echo "${zz2}"


echo ${data:${data:6:1}}
echo "${data:${data:${numbers:3:1}:1}}"

echo "$0 ${1}"
echo ${#0}
echo ${#1}
echo "num len = ${#numbers}"
echo ${numbers:2}
echo ${numbers:${from}}

