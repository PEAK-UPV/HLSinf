RED='\033[0;31m'
GREEN='\033[0;32m'
BRNORG='\033[0;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
WHITE='\033[1;37m'
NC='\033[0m'


BASE_PATH=/home/jomarm10/repos/HLSinf/stratix
HOST_FILE="host"
INPUT_FILE="input.data"
KERNEL_NAME="k_conv2D"
KERNEL_FILE="${KERNEL_NAME}.aocx"
KERNEL_LIB="lib_conv2D"
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


echo -e "${CYAN} Set current dir to ${BASE_PATH} ${NC}"
cd ${BASE_PATH}

echo -e "${CYAN} Remove (if any) previous executable and kernel ${NC}"
rm -f bin/${HOST_FILE}
rm -f bin/${KERNEL_FILE}
rm -f ${KERNEL_FILE}
rm -f ${KERNEL_NAME}.aocr
rm -rf ${KERNEL_NAME}.*.temp

echo -e "${CYAN} Compile ${KERNEL_NAME} library ${NC}"
cd device/src/lib
rm -f ${KERNEL_LIB}.aoclib
rm -rf ${KERNEL_LIB}.*.temp
fpga_crossgen ${KERNEL_LIB}.cpp --target ocl -o ${KERNEL_LIB}.aoco
fpga_libtool --target ocl --create ${KERNEL_LIB}.aoclib ${KERNEL_LIB}.aoco
cp ${KERNEL_LIB}.aoclib ..
cd ../../.. 


echo -e "${CYAN} Compile opencl kernel ${NC}"
aoc -march=emulator -v device/src/k_conv2D.cl -o ${KERNEL_FILE} -I device/src/lib -L device/src -l ${KERNEL_LIB}.aoclib
cp k_conv2D.aocx bin


echo -e "${CYAN} Compile sources ${NC}"
make

echo -e "${CYAN} Moving to executable folder: bin ${NC}"
cd bin

echo -e "${CYAN} Launch host prog ${NC}"
$PROG_COMMAND

echo -e "${CYAN} Returning to ${BASE_PATH} ${NC}"
cd ${BASE_PATH}
