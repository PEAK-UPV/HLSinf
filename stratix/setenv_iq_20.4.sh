# 
# 
# 
# To use different versions of IntelQuartus software and/or BoardSupportPackage files
#  set the appropriate values to
#    IQ_VER
#    BSP_VER
# To specify a different intallation path 
#  set the appropriate value to
#    IQ_BASE_DIR

RED='\033[0;31m'
GREEN='\033[0;32m'
BRNORG='\033[0;33m'
BLUE='\033[0;34m'
WHITE='\033[1;37m'
NC='\033[0m'

# clean previously set env vars
unset INTELFPGAOCLSDKROOT
unset INTELFPGAHLSROOT
unset QSYS_ROOTDIR

# set current toolset version and install path
export IQ_VER="20.4"
export IQ_BASE_DIR="/opt/intelFPGA_pro/${IQ_VER}"
export BSP_VER="20.4"

#
echo ""
echo -e "${GREEN}Setting Quartus ${IQ_VER} environment variables"
echo -e "${GREEN}Setting BSP ${BSP_VER} environment variables"
echo ""
echo -e "${NC}  setting INTELFPGAHLSROOT"
export INTELFPGAHLSROOT="$IQ_BASE_DIR/hls"
echo "   INTELFPGAHLSROOT=$INTELFPGAHLSROOT"
echo ""
echo -e "${NC}  setting INTELFPGAOCLSDKROOT"
export INTELFPGAOCLSDKROOT="$IQ_BASE_DIR/hld"
echo "   INTELFPGAOCLSDKROOT=$INTELFPGAOCLSDKROOT"

echo "  setting QSYS_ROOTDIR"
export QSYS_ROOTDIR="$IQ_BASE_DIR/qsys/bin"
echo "   QSYS_ROOTDIR=$QSYS_ROOTDIR"

echo "  setting QUARTUS_ROOTDIR"
export QUARTUS_ROOTDIR="$IQ_BASE_DIR/quartus"
echo "   QUARTUS_ROOTDIR = $QUARTUS_ROOTDIR"

#since we are using same version of quartus and bsp files, both point to the same version
echo "  setting QUARTUS_ROOTDIR_OVERRIDE for backwards compatibility with stratix bsp $BSP_VER"
export QUARTUS_ROOTDIR_OVERRIDE="/opt/intelFPGA_pro/$BSP_VER/quartus"
echo "    QUARTUS_ROOTDIR_OVERRIDE=$QUARTUS_ROOTDIR_OVERRIDE"

echo "  updating PATH"
export PATH=$PATH:$QUARTUS_ROOTDIR/bin:$QSYS_ROOTDIR:$INTELFPGAOCLSDKROOT/bin:$INTELFPGAHLSROOT/bin
echo "   PATH=$PATH"

#If you use the Mentor Graphics ModelSim software provided
#with Intel Quartus Prime, add the path to ModelSim to your PATH environment variable
echo "  setting ModelSim variables and updating path"
export MODELSIM_PATH="$IQ_BASE_DIR/modelsim_ase"
export MODELSIM_BIN="$MODELSIM_PATH/bin"

echo "  updating PATH"
export PATH=$PATH:$MODELSIM_BIN

# setting environment to compile with gcc as described in "Intel High Level Synthesis Compiler Pro Edition: Reference Manual"
echo ""
echo "  calling init_hls script"
source $INTELFPGAHLSROOT/init_hls.sh
echo -e ""

# Call init opencl script
echo ""
echo "  calling init_opencl script"
source $INTELFPGAOCLSDKROOT/init_opencl.sh

#
echo ""
echo -e "${GREEN}Updating LIBRARY_PATH environment variable"
export LIBRARY_PATH=/usr/lib/x86_64-linux-gnu:$LIBRARY_PATH

export LD_LIBRARY_PATH=$INTELFPGAHLSROOT/host/linux64/lib:$LD_LIBRARY_PATH

#
## repeat some exports that seem to be overwritten or unset....
export QSYS_ROOTDIR="$IQ_BASE_DIR/qsys/bin"
export QUARTUS_ROOTDIR="$IQ_BASE_DIR/quartus"
export QUARTUS_ROOTDIR_OVERRIDE="/opt/intelFPGA_pro/$BSP_VER/quartus"
export MODELSIM_PATH="$IQ_BASE_DIR/modelsim_ase"
export MODELSIM_BIN="$MODELSIM_PATH/bin"

#
echo ""
echo "All done"
echo ""
