In this folder you will find the parser for ONNX models. This parser will read ONNX input
files and will generate an HLSinf model ready to be run by HLSinf. The process is divided in four steps.

FIRST STEP: Extract the model from ONNX
---------------------------------------

In a first step, the parser will read the input ONNX file and will generate an ASCII text file which
will include a description of every node of the model and the initializers of the model.

Assuming you have a onnx file (file.onnx) then you perform this step by running;

python3.8 onnx2txt.py file.onnx > file.txt

SECOND STEP: Extract model initializers
---------------------------------------

The initializers (weights and bias) are extracted by running:

python3.8 get_initializes file.onnx

This program generates a text file for each initializer.

THIRD STEP: Parse the model and generate a new model for the HLSinf platform
----------------------------------------------------------------------------

In this step the txt generated file is read and a new txt file (model.hlsinf) is generated, which contains the new
model for the HLSinf platform.

To run this step you need to launch:

./txt2hlsinf file.txt 1 0

The last two arguments specify the target HLSinf version (1) and subversion (0). Different HLSinf versions have different capabilities.

The HLSinf model is found in the model.hlsinf file. It is an ASCII text file. In addition, two fig files are 
generated (original_model.fig and generated_model.fig) which show the model before and after being adapted.

Initializer data (one in a separate file) are also adapted in this step.

FOUR STEP: Run the model
------------------------

The model is ready to be run on FPGA. To do this you can use the following command:

./hlsinf_run xclbin_file model.hlsinf

This program will read the model, the initializers and will run the complete model for an inference input data.

