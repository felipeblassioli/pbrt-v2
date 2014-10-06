#!/bin/bash

for i in *pbrt
do
	../../src/bin/pbrt "$i"
done
