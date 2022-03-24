#!/bin/bash

###############################################################################
## nice colors for terminal messages
RED='\033[0;31m'
GREEN='\033[0;32m'
BRNORG='\033[0;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
WHITE='\033[1;37m'
NC='\033[0m'

###############################################################################

BASE_PATH="$PWD"

TARGET_FPGA_BOARD="s10mx_hbm"


HOST_FILE="host"

INPUT_FILE="input.data"

KERNEL_NAME="k_conv2D"
KERNEL_FILE="${KERNEL_NAME}.aocx"
PROG_COMMAND="./${HOST_FILE} ${INPUT_FILE} ${KERNEL_FILE}"

AOCL_COMPILE_OPTS=""
####AOCL_COMPILE_EXTRA="-fp-relaxed -fpc -board=s10mx_hbm"
AOCL_COMPILE_EXTRA="-fpc -board=${TARGET_FPGA_BOARD}"


EMULATION_MODE="no"
EMULATION_MACRO_MAKE=(CXXFLAGS+="  ")

###############################################################################
## abort script execution function
abort()
{
    echo >&2 '
***************
*** ABORTED ***
***************
'
    now=$(date +"%d-%m-%y  %a  %T")
    echo -e "${BRNORG}$now${NC} An error occurred" >&2
    echo -e "${RED} $1 ${NC}"
    echo "" >&2
    echo "Exiting..." >&2  
    exit 1
}

###############################################################################
## check input optinons
while getopts "e" opt
do
  case "${opt}" in
    e) 
      EMULATION_MODE="yes"
      EMULATION_MACRO_MAKE=(CXXFLAGS+="-D EMULATION_CONFIG_ENABLED=TRUE")
      AOCL_COMPILE_OPTS=" -D EMULATION_CONFIG_ENABLED=TRUE -march=emulator"
      PROG_COMMAND="${PROG_COMMAND} emulation"
      echo "EMULATION_MACRO_MAKE=${EMULATION_MACRO_MAKE}"
      ;;
  esac
done
 
###############################################################################
now=$(date +"%d-%m-%y  %a  %T")
echo ""
echo "---------------------------------------------------------"
echo "---------------------------------------------------------"
echo "                                                         "
echo -e "${BRNORG}$now${NC}                                    "
echo -e "compile conv2D kernel file                            "
echo -e "compile host executable binary                        "
echo "                                                         "
echo "---------------------------------------------------------"
echo "---------------------------------------------------------"
echo ""
echo -e " target fpga board  = ${TARGET_FPGA_BOARD} "
echo -e " using INTELFPGAOCLSDKROOT = ${INTELFPGAOCLSDKROOT}"
echo -e " emulation_mode = ${EMULATION_MODE} "
echo -e " BASE_PATH set to ${BASE_PATH}"
echo -e "   Set current dir to ${BASE_PATH}"
echo ""

cd ${BASE_PATH}


###############################################################################
## clean previous run files
echo -e "${CYAN} Clean: Remove (if any) previous run files ${NC}"
rm -f bin/${HOST_FILE}
rm -f bin/${KERNEL_FILE}
rm -f bin/core
rm -f bin/profile.mon
rm -f bin/reprogram_temp.sof 
rm -f ${KERNEL_FILE}
rm -f ${KERNEL_NAME}.aocr
rm -f ${KERNEL_NAME}.aoco
rm -rf {$KERNEL_NAME}
rm -rf ${KERNEL_NAME}.*.temp
rm -rf .emu_models

###############################################################################
# compile aocl kernel
echo ""
now=$(date +"%d-%m-%y  %a  %T")
echo -e "${BRNORG}$now${NC}"
echo -e "${CYAN}Compile opencl kernel ${NC}"
aoc -g ${AOCL_COMPILE_OPTS} -I $INTELFPGAOCLSDKROOT/include/kernel_headers -v -g -high-effort device/src/k_conv2D.cl -o ${KERNEL_FILE} ${AOCL_COMPILE_EXTRA}

# check opencl kernel file
echo ""
now=$(date +"%d-%m-%y  %a  %T")
echo -e "${BRNORG}$now${NC}"
echo -e "${CYAN}Check OpenCL kernel file${NC}"
FILE=${KERNEL_FILE}
if [ -f "$FILE" ]; then
    echo -e "${GREEN}file \"$FILE\" exists.${NC}"
    echo "continue..."
else 
    echo ""
    abort "file \"$FILE\" does not exist."
fi
 
echo "move kernel file: ${KERNEL_FILE} to executable folder"
mv ${KERNEL_FILE} bin/
echo "done"

###############################################################################
# compile host executable
echo ""
now=$(date +"%d-%m-%y  %a  %T")
echo -e "${BRNORG}$now${NC}"
echo -e "${CYAN}Compile sources ${NC}"
#make CXXFLAGS+="-D EMULATION_CONFIG_ENABLED=TRUE"
make "${EMULATION_MACRO_MAKE[@]}"

# check host executable file exists
echo ""
now=$(date +"%d-%m-%y  %a  %T")
echo -e "${BRNORG}$now${NC}"
echo -e "${CYAN}Check host executable file${NC}"
FILE="bin/${HOST_FILE}"
if [ -f "$FILE" ]; then
    echo -e "${GREEN}file \"$FILE\" exists.${NC}"
    echo "continue..."
else 
    echo ""
    abort "file \"$FILE\" does not exist."
fi
echo "done"

###############################################################################
# all files seem to be in place, run test
echo ""
echo -e "${CYAN}Moving to executable folder: bin ${NC}"
cd bin
echo "  current path is: $PWD"

echo "\n\n\n"
now=$(date +"%d-%m-%y  %a  %T")
echo "##############################################################"
echo ""
echo -e "${BRNORG}$now${NC}"
echo -e "${CYAN} Launch host prog ${NC}"
echo -e "  ${GREEN} $PROG_COMMAND ${NC}"
echo ""
echo "##############################################################"
echo ""
$PROG_COMMAND

###############################################################################
# all done (secceeded or not), returning
echo ""
echo "#####################"
echo ""
echo "  program finished"
echo ""
echo "#####################"
echo ""
echo -e "${CYAN}Returning to ${BASE_PATH} ${NC}"
cd ${BASE_PATH}

now=$(date +"%d-%m-%y  %a  %T")
echo ""
echo -e "${BRNORG}${now} ${NC}"
echo -e "${CYAN}      ...all done ${NC}"
echo ""

###############################################################################
## end of file
