RED='\033[0;31m'
GREEN='\033[0;32m'
BRNORG='\033[0;33m'
BLUE='\033[0;34m'
WHITE='\033[1;37m'
NC='\033[0m'


BASE_PATH=/home/jomarm10/repos/HLSinf/stratix
HOST_FILE="host"
INPUT_FILE="input.data"
KERNEL_FILE="k_conv2D.aocx"
PROG_COMMAND="./${HOST_FILE} ${INPUT_FILE} ${KERNEL_FILE}"

abort()
{
    echo >&2 '
***************
*** ABORTED ***
***************
'
    echo "An error occurred. Exiting..." >&2
    echo -e "${RED} $1 ${NC}"
    exit 1
}


echo "Set current dir to ${BASE_PATH}"
cd ${BASE_PATH}

echo "Remove (if any) previous executable"
rm -f bin/host

echo "Compile sources"
make

echo "Moving to executable folder: bin"
cd bin

echo "Launch host prog"
$PROG_COMMAND

echo "Returning to ${BASE_PATH}"
cd ${BASE_PATH}
