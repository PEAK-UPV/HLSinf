In this folder you will find the parser for ONNX models. This parser will read ONNX input
files and will generate an HLSinf model ready to be run by HLSinf. The parser is able also
to run the model on a system with HLSinf (even with no HLSinf support).

The whole process is divided in three steps. First, an ONNX model is parsed and data is extracted
and a text file is generated. With this generated file a new text file is generated with the needed
optimizations and adaptations to be used in a system with HLSinf. Different configurations and optimizations
will be possible In a third and final step the generated model can be used to run inference processes, providing
timing statistics.

FIRST STEP: Extract the model from ONNX
---------------------------------------

In a first step, the parser will read the input ONNX file and will generate an ASCII text file which
will include a description of every node of the model and the initializers of the model.

Assuming you have a onnx file (file.onnx) then you perform this step by running:

python3.8 onnx2txt.py file.onnx file.txt

This script will read the onnx file and will generate a text file with all needed information. Basically,
for every input, output, node, and initializer a text line will be written into the text file. The format is as follows:

  - input (all fields comma separated):
     - name of the input
     - "inputs" label
     - number of dimensions
     - dimensions
     - data type (e.g. float32)

  - output (all fields comma separated):
     - name of the output
     - "outputs" label

  - node:
     - name of the node
     - "node" label
     - type of the node (e.g Conv, Relu, BatchNormalization, ...)
     - number of inputs of the node
     - list of input names (they are either nodes or initializers)
     - number of outputs of the node
     - list of output names (they are nodes)
     - dilations (list of dilation numbers, if applicable to the node, the node is a Conv node)
     - kernel shape (list of kernel dimensions, if applicable to the node, the node is a Conv or a MaxPool node)
     - padings (list of padings, if applicable to the node, the node is a Conv or a MaxPool node)
     - strides (list of strides, if applicable to the node, the node is a Conv or a MaxPool node)
     - group (if applicable to the node, the node is a Conv node)
     - epsilon (epsilon value if the node is a BatchNormalization node)
     - momentum (momentum value if the node is a BatchNormalization node)

  - initializer:
     - name of the initializer
     - "initializer" label
     - number of dimensions
     - dimensions
     - data type (e.g. float32)
     - list of values (values of the initializer)

The generated text file can be of large size.

SECOND STEP: Parse the model and generate a new model for the HLSinf platform
-----------------------------------------------------------------------------

In this step the text generated file is read and a new text file is generated, which contains the new
model for the HLSinf platform. Different optimizations and configurations can be applied in this step. These
options can be listed by running:

./hlsinf_runner -help

The options are described as follows:

  -cpi <value>          Assume a specific CPI value for HLSinf (by default set to 4)
  -cpo <value>          Assume a specific CPO value for HLSinf (by default set to 4)
  -a1x1                 Adapt 1x1 Conv filters to equivalent 3x3 filters
  -k_c                  Replace Conv nodes by HLSinf nodes
  -k_cb                 Replace Conv + BatchNormalization nodes by HLSinf nodes
  -k_cbr                Replace Conv + BatchNormalization+Relu nodes by HLSinf nodes
  -k_cbar               Replace Conv + BatchNormalization+Add+Relu nodes by HLSinf nodes
  -xclbin <filename>    Set xclbin file including the HLSinf accelerator/s
  -f                    Generate fig files for original and generated models
  -c                    Run in convert mode (does not run the model, just converts the model)
  -r                    Run the input model into the HLSinf platform
  -v                    Convert/Run in verbose mode
  -i                    Set input file name to convert or to run
  -o                    Set output file name to write the generated model

Every HLSinf version can use a subset of options, since not all optimizations are included in all HLSinf versions. Here is a list
of options applicable to current HLSinf versions:

------------------------------------------------------------
| HLSinf  |     |     |      |     |      |       |        |
| version | cpi | cpo | a1x1 | k_c | k_cb | k_cbr | k_cbar |
------------------------------------------------------------
|   1.0   |  4  |  4  |  yes | yes | yes  | yes   | yes    |
------------------------------------------------------------

For instance, to run this step for HLSinf 1.0 you run:

./hlsinf_runner -i model.txt -o model_hlsinf_1_0.txt -a1x1 -k_c -k_cb -k_cbr -k_cbar -c

The new model file has a slightly different format. For each input, output, node, and initializer a text line is included, as follows:

  - node (all fields comma separated)
    - "node" label
    - runing order, is a number. A node with a order x can be run before a node with order y only if x <= y. 
      (two nodes with the same order can be run in parallel)
    - node type (e.g. Conv, Relu, Add, BatchNormalization, HLSinf)
    - node name
    - number of inputs
    - input list (each input listed by its name, inputs can be nodes and/or initializers)
    - number of outputs
    - output list (each output listed by its name)
    - data parameters (comma separated):
      - number of input channels (I)
      - height of input channel (HI)
      - width of input channel (WI)
      - number of output channels (O)
      - height of output channel (HO)
      - width of output channel (WO)
    - if the node is an HLSinf node, then the "keyword" is listed (a keyword represents the functionality of the HLSinf node: k_c, k_cb, ...)
    - if the node is an HLSinf node, then its parameters are listed (comma separated):
      - filter height and width (kh, kw)
      - stride (sh, sw)
      - paddings (pt, pb, pl, pr)
    - if the node is a Conv node, its parameters are listed (comma separated):
      - dilations (dh, dw)
      - filter height and width (kh, kw)
      - stride (sh, sw)
      - paddings (pt, pb, pl, pr)
      - groups
    - if the node is a MaxPool node, its parameters are listed (comma separated):
      - filter height and width (kh, kw)
      - stride (sh, sw)
      - paddings (pt, pb, pl, pr)
    - if the node is a BatchNormalization node, then its parameters are listed
      - epsilon
      - momentum

  - input (all fields comma separated)
    - "input" label
    - input name
    - number of dimensions
    - dimensions

  - outputs (all fields comma separated)
    - "output" label
    - output name

  - initializers (all fields comma separated)
    - "initializer" label
    - initializer name
    - initializer type (e.g. weight, bias, gamma, beta, running_mean, running_var)
    - number of dimensions
    - dimensions
    - data type (e.g. float32)
    - list of values (values of the initializer)

The generated text file can be of large size. 

THIRD STEP: Run the model
-------------------------

Now, the generated model from the previous step is ready to be used. To do this, we run the 
program again but use the -r option (instead of -c). As an example, we can run:

./hlsinf_runner -i model.txt -r -xclbin file.xclbin

We need to specify the xclbin file to be used for the FPGA to be programmed.

