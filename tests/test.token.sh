
EXEC_PATH=../bin

CONFIG_FILE=./token.json
TEST_FILE=./txt/sample_01.txt
#TEST_FILE=./txt/sample_02.txt

echo "${EXEC_PATH}/text_sgmt -c ${CONFIG_FILE} ${TEST_FILE}"
${EXEC_PATH}/text_sgmt -c ${CONFIG_FILE} ${TEST_FILE}


