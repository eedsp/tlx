#!/bin/bash

EXEC_PATH=../bin

for vDB in token.sgmt.db phrase.db
do
    echo "${EXEC_PATH}/db2shm -c import  -d ../db -f ${vDB}"
    ${EXEC_PATH}/db2shm -c import  -d ../db -f ${vDB}
done

ipcs -am

