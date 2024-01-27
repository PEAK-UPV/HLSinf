xvlog --work xil_defaultlib acc.v -m64 --relax
xvlog --work xil_defaultlib align.v -m64 --relax
xvlog --work xil_defaultlib distribute.v -m64 --relax
xvlog --work xil_defaultlib fifo.v -m64 --relax
xvlog --work xil_defaultlib mem.v -m64 --relax
xvlog --work xil_defaultlib mul.v -m64 --relax
xvlog --work xil_defaultlib read.v -m64 --relax
xvlog --work xil_defaultlib write.v -m64 --relax
xvlog --work xil_defaultlib RTLinf.v -m64 --relax
xvlog --work xil_defaultlib write.v -m64 --relax
xvlog --work xil_defaultlib tb_1.v -m64 --relax

xvlog --work xil_defaultlib glbl.v

# elaborate
xelab -wto 898f28d1485241a29ee1020fd14733e0 --incr --debug typical --relax --mt 8 -L xil_defaultlib -L unisims_ver -L unimacro_ver -L secureip --snapshot tb_1_behav xil_defaultlib.tb_1 xil_defaultlib.glbl -log elaborate.log

# simulate
xsim tb_1_behav -key {Behavioral:sim_1:Functional:tb_1} -tclbatch tb_1.tcl -log simulate.log

