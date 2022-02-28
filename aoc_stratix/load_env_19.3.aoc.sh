# Enable Quartus
# In a separate script
RED='\033[0;31m'
GREEN='\033[0;32m'
BRNORG='\033[0;33m'
BLUE='\033[0;34m'
WHITE='\033[1;37m'
NC='\033[0m'


unset INTELFPGAOCLSDKROOT
unset QSYS_ROOTDIR

export IQ_VER="19.3"
export IQ_BASE_DIR="/opt/intelFPGA_pro/${IQ_VER}"

export BSP_VER="19.3"
#BSP_PATH="/opt/intel_boards/s10mx_ref_${BSP_VER}"
BSP_PATH="/opt/intel_boards/s10mx_ref_original"

echo ""
echo -e "${GREEN}Setting Quartus ${IQ_VER} environment variables"
echo -e "${GREEN}Setting BSP ${BSP_VER} environment variables"
echo ""
echo -e "${NC}  setting INTELFPGAOCLSDKROOT"
export INTELFPGAOCLSDKROOT="$IQ_BASE_DIR/hld"
echo "   INTELFPGAOCLSDKROOT=$INTELFPGAOCLSDKROOT"
echo "  setting QSYS_ROOTDIR"
export QSYS_ROOTDIR="$IQ_BASE_DIR/qsys/bin"
echo "   QSYS_ROOTDIR=$QSYS_ROOTDIR"

#echo ""
#echo "WARNING to run Board Test System"
echo "  setting QUARTUS_ROOTDIR"
export QUARTUS_ROOTDIR="$IQ_BASE_DIR/quartus"
echo "   QUARTUS_ROOTDIR = $QUARTUS_ROOTDIR"


#since we are using same version of quartus and bsp files, both point to the same version
echo "  setting QUARTUS_ROOTDIR_OVERRIDE for backwards compatibility with stratix bsp $BSP_VER"
export QUARTUS_ROOTDIR_OVERRIDE="/opt/intelFPGA_pro/$BSP_VER/quartus"
echo "    QUARTUS_ROOTDIR_OVERRIDE=$QUARTUS_ROOTDIR_OVERRIDE"


echo "  updating PATH"
export PATH=$PATH:$QUARTUS_ROOTDIR/bin:$QSYS_ROOTDIR:$INTELFPGAOCLSDKROOT/bin
echo "   PATH=$PATH"

#If you use the Mentor Graphics ModelSim software provided
#with Intel Quartus Prime, add the path to ModelSim to your PATH environment variable
echo "  setting ModelSim variables and updating path"
export MODELSIM_PATH="$IQ_BASE_DIR/modelsim_ae"
export MODELSIM_BIN="$MODELSIM_PATH/bin"

export PATH=$PATH:$MODELSIM_BIN


## puede que sea necesario, verificar con RAFA y con el manual de instalación de quartus i config del devkit
echo ""
echo -e "${GREEN}Setting AOCL environment variables"
echo -e "${NC}  setting AOCL_BOARD_PACKAGE_ROOT to ${BSP_PATH}"
export AOCL_BOARD_PACKAGE_ROOT="${BSP_PATH}"

echo ""
echo "  calling init_opencl script"
source $INTELFPGAOCLSDKROOT/init_opencl.sh


## para recipe1 los settings son diferentes


#echo ""
#echo -e "${GREEN}Setting HLS environment variables"
#echo -e "${NC}  sourcing init_hls.sh script"
#source /opt/intelFPGA_pro/$IQ_VER/hls/init_hls.sh
#echo -e ""

echo ""
echo -e "${GREEN}Setting LIBRARY_PATH environment variable"
export LIBRARY_PATH=/usr/lib/x86_64-linux-gnu:$LIBRARY_PATH












## repeat some exports that seem to be overwritten or unset....
export QSYS_ROOTDIR="$IQ_BASE_DIR/qsys/bin"
export QUARTUS_ROOTDIR="$IQ_BASE_DIR/quartus"
export QUARTUS_ROOTDIR_OVERRIDE="/opt/intelFPGA_pro/$BSP_VER/quartus"
export MODELSIM_PATH="$IQ_BASE_DIR/modelsim_ase"
export MODELSIM_BIN="$MODELSIM_PATH/bin"




echo ""
echo "All done"
echo ""
