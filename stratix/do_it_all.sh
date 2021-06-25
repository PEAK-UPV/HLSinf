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


echo ""
echo "-----------------------------------------------------"
echo "          compile files to run test_conv2D"
echo "-----------------------------------------------------"
echo ""
echo -e "${CYAN} Set current dir to ${BASE_PATH} ${NC}"
cd ${BASE_PATH}

echo -e "${CYAN} Clean: Remove (if any) previous run files ${NC}"
rm -f bin/${HOST_FILE}
rm -f bin/${KERNEL_FILE}
rm -f bin/core
rm -f ${KERNEL_FILE}
rm -f ${KERNEL_NAME}.aocr
rm -rf {$KERNEL_NAME}
rm -rf ${KERNEL_NAME}.*.temp
rm -rf .emu_models
rm -rf *-lib
rm -f device/src/lib/${KERNEL_LIB}.aoclib
rm -f device/src/${KERNEL_LIB}.aoclib
rm -rf device/src/lib/${KERNEL_LIB}.*.temp

echo ""
echo -e "${CYAN} Compile ${KERNEL_NAME} library ${NC}"
cd device/src/lib
fpga_crossgen -v -g -cl-opt-disable ${KERNEL_LIB}.cpp --target ocl -o ${KERNEL_LIB}.aoco
fpga_libtool -v --target ocl --create ${KERNEL_LIB}.aoclib ${KERNEL_LIB}.aoco
cp ${KERNEL_LIB}.aoclib ..
cd ../../.. 

# check library file was generated
echo ""
echo "Check HLS library file"
FILE="device/src/${KERNEL_LIB}.aoclib"
if [ -f "$FILE" ]; then
    echo -e "${GREEN} file \"$FILE\" exists. ${NC}"
    echo " continue..."
else
    abort "file \"$FILE\" does not exist."
fi


echo ""
echo -e "${CYAN} Compile opencl kernel - EMULATION MODE ${NC}"
aoc -cl-opt-disable -g -march=emulator -v -g device/src/k_conv2D.cl -o ${KERNEL_FILE} -I device/src/lib -L device/src -l ${KERNEL_LIB}.aoclib
cp k_conv2D.aocx bin

# check opencl kernel file
echo ""
echo "Check OpenCL kernel file"
FILE=${KERNEL_FILE}
if [ -f "$FILE" ]; then
    echo -e "${GREEN}file \"$FILE\" exists.${NC}"
    echo "continue..."
else 
    echo ""
    abort "file \"$FILE\" does not exist."
fi
 


echo ""
echo -e "${CYAN} Compile sources ${NC}"
make

# check host executable file
echo ""
echo "Check host executable file"
FILE="bin/${HOST_FILE}"
if [ -f "$FILE" ]; then
    echo -e "${GREEN}file \"$FILE\" exists.${NC}"
    echo "continue..."
else 
    echo ""
    abort "file \"$FILE\" does not exist."
fi


echo ""
echo -e "${CYAN} Moving to executable folder: bin ${NC}"
cd bin

echo -e "${CYAN} Launch host prog ${NC}"
echo -e "${GREEN} $PROG_COMMAND ${NC}"
$PROG_COMMAND

echo ""
echo -e "${CYAN} Returning to ${BASE_PATH} ${NC}"
cd ${BASE_PATH}

echo ""
echo -e "${CYAN} ...all done ${NC}"
echo ""

