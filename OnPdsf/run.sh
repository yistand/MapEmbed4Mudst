#!/bin/bash -l

for i in /global/u1/l/liyi/pp200embed/lists/*;
	do echo $i;
	root  -l -b -q EventIdList.C"(\"$i\")"
done;
