#!/bin/bash

test="AbC AbC XyZ AbC XyZ AbC"
replacewith="DeF"
replacethis="AbC"

xx=${test//"Ab"/"XX"}
echo ${xx}

xx=${test//${replacethis}/"X"}
echo ${xx}

xx=${test//${replacethis}/${replacewith}}
echo ${xx}

xx=${test//${replacethis}/${replacewith,}}
echo ${xx}

xx=${test//${replacethis}/${replacewith^^}}
echo ${xx}

xx=${test/"Ab"/"XX"}
echo ${xx}

xx=${test/${replacethis}/"X"}
echo ${xx}

xx=${test/${replacethis}/${replacewith}}
echo ${xx}

xx=${test/${replacethis}/${replacewith,}}
echo ${xx}

xx=${test/${replacethis}/${replacewith^^}}
echo ${xx}



