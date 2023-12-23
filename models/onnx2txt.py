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
    new_model = sys.argv[2]

    onnxModel = onnx.load(old_model)
    onnx.checker.check_model(onnxModel)
    nodes = onnxModel.graph.node
    initializers = onnxModel.graph.initializer
    inputs = onnxModel.graph.input
    outputs = onnxModel.graph.output

    f = open(new_model, "w")

    # input buffers
    for obj in inputs:
        name = obj.name
        # number of dimensions
        num_dims=len(obj.type.tensor_type.shape.dim)
        # dimensions
        dims = []
        for i in obj.type.tensor_type.shape.dim:
            dims.append(i.dim_value)
        # write info
        f.write(name)
        f.write(",inputs,")
        f.write(str(num_dims))
        f.write(",")
        for i in dims:
            f.write(str(i))
            f.write(",")
        f.write("float32\n")    # TODO

    # output buffers 
    for obj in outputs:
        name = obj.name
        f.write(name)
        f.write(",outputs\n")

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

        if (type=="BatchNormalization"):
            # epsilon
            epsilon = 0
            for a in node.attribute:
                if a.name == "epsilon":
                    epsilon = a.f
            # momentum
            momentum = 0
            for a in node.attribute:
                if a.name == "momentum":
                    momentum = a.f
            # write info
            f.write(name)               # node name
            f.write(",node,")           # "node" label
            f.write(type)               # node type
            f.write(",")
            f.write(str(num_inputs))    # number of inputs
            f.write(",")
            f.write(str_in)             # inputs (includes final comma)
            f.write(str(num_outputs))   # number of outputs
            f.write(",")
            f.write(str_out)            # outputs (includes final comma)
            f.write(str(epsilon))       # epsilon
            f.write(",")
            f.write(str(momentum))      # momentum
            f.write("\n")

        elif (type=="MaxPool") | (type=="AveragePool"):
            # kernel shape
            kh=-1
            kw=-1
            for a in node.attribute:
                if a.name == "kernel_shape":
                    kh=a.ints[0]
                    kw=a.ints[1]
            # strides (if not present defaults to 1)
            sh=1
            sw=1
            for a in node.attribute:
                if a.name == "strides":
                    sh=a.ints[0]
                    sw=a.ints[1]
            #pads
            pt=0
            pb=0
            pl=0
            pr=0
            for a in node.attribute:
                if a.name == "pads":
                    pt=a.ints[0]
                    pl=a.ints[1]
                    pb=a.ints[2]
                    pr=a.ints[3]
            for a in node.attribute:
                if a.name == "auto_pad":
                    if ((a.s == "SAME_UPPER") | (a.s == "VALID")):
                        if sh==1:
                            pt=1
                            pb=1
                        if sh==2:
                            pt=0
                            pb=0
                    elif ((a.s == "SAME_LOWER") | (a.s == "VALID")):
                        if sw==1:
                            pl=1
                            pr=1
                        if sw==2:
                            pl=0
                            pr=0
            # write info
            f.write(name)               # node name
            f.write(",node,")           # "node" label
            f.write(type)               # node type
            f.write(",")
            f.write(str(num_inputs))    # number of inputs
            f.write(",")
            f.write(str_in)             # inputs (includes final comma)
            f.write(str(num_outputs))   # number of outputs
            f.write(",")
            f.write(str_out)            # outputs (includes final comma)
            f.write(str(kh))            # kernel_shape
            f.write(",")
            f.write(str(kw))
            f.write(",")
            f.write(str(pt))            # padings
            f.write(",")
            f.write(str(pb))
            f.write(",")
            f.write(str(pl))
            f.write(",")
            f.write(str(pr))
            f.write(",")
            f.write(str(sh))             # stride
            f.write(",")
            f.write(str(sw))
            f.write("\n")

        elif (type=="Conv"):
            # kernel shape (if not present should be inferred from input W)
            kh=-1 
            kw=-1
            for a in node.attribute:
                if a.name == "kernel_shape":
                    kh=a.ints[0]
                    kw=a.ints[1]
            # strides (if not present defaults to 1)
            sh=1
            sw=1
            for a in node.attribute:
                if a.name == "strides":
                    sh=a.ints[0]
                    sw=a.ints[1]
            # dilations (if not present defaults to 1)
            dh=1
            dw=1
            for a in node.attribute:
                if a.name == "dilations":
                    dh=a.ints[0]
                    dw=a.ints[1]
            #group (number of groups input channels and output channels are divided into)
            g=-1
            for a in node.attribute:
                if a.name == "group":
                    g=a.i
            #pads (defaults to zero)
            pt=0
            pb=0
            pl=0
            pr=0
            for a in node.attribute:
                if a.name == "pads":
                    pt=a.ints[0]
                    pl=a.ints[1]
                    pb=a.ints[2]
                    pr=a.ints[3]
            for a in node.attribute:
                if a.name == "auto_pad":
                    if ((a.s == "SAME_UPPER") | (a.s == "VALID")):
                        if sh==1:
                            pt=1
                            pb=1
                        if sh==2:
                            pt=0
                            pb=0
                    elif ((a.s == "SAME_LOWER") | (a.s == "VALID")):
                        if sw==1:
                            pl=1
                            pr=1
                        if sw==2:
                            pl=0
                            pr=0
            # write info
            f.write(name)               # node name
            f.write(",node,")           # "node" label
            f.write(type)               # node type
            f.write(",") 
            f.write(str(num_inputs))    # number of inputs
            f.write(",")
            f.write(str_in)             # inputs (includes final comma)
            f.write(str(num_outputs))   # number of outputs
            f.write(",")
            f.write(str_out)            # outputs (includes final comma)
            f.write(str(dh))            # dilations
            f.write(",")
            f.write(str(dw))
            f.write(",")
            f.write(str(kh))            # kernel_shape
            f.write(",")
            f.write(str(kw))
            f.write(",")
            f.write(str(pt))            # padings
            f.write(",")
            f.write(str(pb))
            f.write(",")
            f.write(str(pl))
            f.write(",")
            f.write(str(pr))
            f.write(",")
            f.write(str(sh))             # strice
            f.write(",")
            f.write(str(sw))
            f.write(",")
            f.write(str(g))              # group
            f.write("\n")

        elif (type == "Transpose"):
            # num permutations
            num_perms=0
            for a in node.attribute:
                if a.name == "perm":
                    num_perms=len(a.ints)
            perm = []
            for a in node.attribute:
                if a.name == "perm":
                    for i in a.ints:
                        perm.append(i)
            # write info
            f.write(name)               # node name
            f.write(",node,")           # "node" label
            f.write(type)               # node type
            f.write(",")
            f.write(str(num_inputs))    # number of inputs
            f.write(",")
            f.write(str_in)             # inputs (includes final comma)
            f.write(str(num_outputs))   # number of outputs
            f.write(",")
            f.write(str_out)            # outputs (includes final comma)
            f.write(str(num_perms))     # number of permutations
            f.write(",")
            for a in perm:
                f.write(str(perm[a]))
                f.write(",")

            f.write("\n")

        else:
            # write info
            f.write(name)               # node name
            f.write(",node,")           # "node" label
            f.write(type)               # node type
            f.write(",")
            f.write(str(num_inputs))    # number of inputs
            f.write(",")
            f.write(str_in)             # inputs (includes final comma)
            f.write(str(num_outputs))   # number of outputs
            f.write(",")
            f.write(str_out)            # outputs (includes final comma)
            f.write("\n")

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
        f.write(name)
        f.write(",initializer,")
        f.write(str(num_dims))
        f.write(",")
        f.write(str_dims)
        f.write(str(dtype))
        for d in range(a.size):
           f.write(",")
           f.write(str(a.flat[d]))

        f.write("\n")

    f.close()

