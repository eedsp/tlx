#!/bin/bash

export DYLD_LIBRARY_PATH=$APP/lib

EXEC_PATH=../bin

mkdir -p ../db
mkdir -p ../temp

vFILE="garbage.txt emoticon.txt html.txt phrase.txt pset.txt nset.txt token.txt"
CONFIG_FILE=./db.phrase.json
echo "${EXEC_PATH}/nltk_tools -c $CONFIG_FILE $vOPT $vFILE"
${EXEC_PATH}/db_tools -c $CONFIG_FILE $vOPT $vFILE

vFILE="token.sgmt.txt"
CONFIG_FILE=./db.token.sgmt.json
echo "$EXEC_PATH/db_tools -c $CONFIG_FILE $vOPT $vFILE"
${EXEC_PATH}/db_tools -c $CONFIG_FILE $vOPT $vFILE


