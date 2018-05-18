#!/usr/bin/env bash

for file in ./Tests/*.ss
do
	#./P2.out $file >> results.out
	./P3.out $file
done
