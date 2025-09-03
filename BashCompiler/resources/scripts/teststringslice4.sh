#!/bin/bash

test="000 abc def xyz 123 456 xyz abc 123 xyz 999"

xx=${test##?*abc}
echo ${xx}

xx=${test#?*abc}
echo ${xx}

glob="abc"
xx=${test#?*${glob}}
echo ${xx}

glob="abc"
xx=${test##?*${glob}}
echo ${xx}

xx=${test%abc?*}
echo ${xx}

xx=${test%%abc?*}
echo ${xx}

xx=${test%${glob}?*}
echo ${xx}

xx=${test%%${glob}?*}
echo ${xx}

