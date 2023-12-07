from math import isnan
import numpy as np
import onnxruntime as backend
import onnx.numpy_helper as oh
from onnx import numpy_helper
import onnx
import sys

def shape2tuple(shape):
    return tuple(getattr(d, 'dim_value', 0) for d in shape.dim)

if __name__ == '__main__':

    # path to ONNX model file
    old_model = sys.argv[1]
    # sym, arg_params, aux_params = import_model(old_model)
    onnxModel = onnx.load(old_model)
#    print("LOAD")
    onnx.checker.check_model(onnxModel)
    nodes = onnxModel.graph.node
    initializers = onnxModel.graph.initializer
    inputs = onnxModel.graph.input
    outputs = onnxModel.graph.output


    # input buffers
    for obj in inputs:
        name = obj.name
        # deberiamos sacar de una forma mas uniforme las dimensiones
        num_dims=len(obj.type.tensor_type.shape.dim)-1
        print(name,",inputs,",num_dims,",",obj.type.tensor_type.shape.dim[1].dim_value,",",obj.type.tensor_type.shape.dim[2].dim_value,",",obj.type.tensor_type.shape.dim[3].dim_value,",float32")

    # output buffers 
    for obj in outputs:
        name = obj.name
        print(name,",outputs")

    # nodes
    for node in onnxModel.graph.node:
        name = node.name
        type = node.op_type
        num_inputs=len(node.input)
        num_outputs=len(node.output)
        str_in = ""
        for i in node.input:
            str_in = str_in + i + ","
        str_out = ""
        for i in node.output:
            str_out = str_out + i + ","
            
        kernel_shape=""                      
        pads=""                              
        strides=""                           
        group=""
        epsilon=""
        dilations=""
        momentum=""
        if type=="Conv":
            for a in node.attribute:
                if a.name == "dilations":
                    for i in a.ints:
                        dilations = dilations + " " + str(i) + ","
                elif a.name == "group":
                    group=str(a.i)
                elif a.name == "kernel_shape":
                    for i in a.ints:
                        kernel_shape = kernel_shape + " " + str(i) + ","
                elif a.name == "pads":
                    for i in a.ints:
                        pads = pads + " " + str(i) + ","
                elif a.name == "strides":
                    for i in a.ints:
                        strides = strides + " " + str(i) + ","
        if type=="BatchNormalization":
            for a in node.attribute:
                if a.name == "epsilon":
                    epsilon=str(a.f) + ","
                elif a.name == "momentum":
                    momentum=str(a.f) + ","
        if type=="MaxPool":
            for a in node.attribute:
                if a.name == "kernel_shape":
                    for i in a.ints:
                        kernel_shape = kernel_shape + " " + str(i) + ","
                elif a.name == "pads":
                    for i in a.ints:
                        pads = pads + " " + str(i) + ","
                elif a.name == "strides":
                    for i in a.ints:
                        strides = strides + " " + str(i) + ","

        print(name,",node,",type,",",num_inputs,",",str_in,num_outputs,",",str_out,dilations,kernel_shape,pads,strides,group,epsilon,momentum)
        
        

    # initializers
    for obj in onnxModel.graph.initializer:
        name=obj.name
        a=onnx.numpy_helper.to_array(obj).copy()
        dtype=a.dtype
        num_dims = len(obj.dims)
        str_dims=""
        for i in obj.dims:
            str_dims = str_dims + str(i) + ","
        dims=obj.dims
        print(name,",initializer,",num_dims, ",", str_dims, dtype)