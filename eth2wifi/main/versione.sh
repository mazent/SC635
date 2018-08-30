#!/bin/bash

rev=`git rev-list HEAD --count`

echo "#define VER   $rev" > versione.h
