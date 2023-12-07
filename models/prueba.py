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

    for obj in onnxModel.graph.initializer:
        print(obj)

