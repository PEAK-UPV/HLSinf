<project xmlns="com.autoesl.autopilot.project" name="HLSinf" top="k_conv2D">
    <includePaths/>
    <libraryPaths/>
    <Simulation argv="">
        <SimFlow name="csim" setup="true" optimizeCompile="true" ldflags="" mflags="" csimMode="0" lastCsimMode="0" clean="false"/>
    </Simulation>
    <files xmlns="">
        <file name="../../../src/test_print.cpp" sc="0" tb="1" cflags=" -DHLSINF_1_15 -Wno-unknown-pragmas" csimflags=" -Wno-unknown-pragmas" blackbox="false"/>
        <file name="../../../src/test_kernel.cpp" sc="0" tb="1" cflags=" -DHLSINF_1_15 -Wno-unknown-pragmas" csimflags=" -Wno-unknown-pragmas" blackbox="false"/>
        <file name="../../../src/test_file.cpp" sc="0" tb="1" cflags=" -DHLSINF_1_15 -Wno-unknown-pragmas" csimflags=" -Wno-unknown-pragmas" blackbox="false"/>
        <file name="../../../src/test_cpu.cpp" sc="0" tb="1" cflags=" -DHLSINF_1_15 -Wno-unknown-pragmas" csimflags=" -Wno-unknown-pragmas" blackbox="false"/>
        <file name="../../../src/test_conv2D.h" sc="0" tb="1" cflags=" -DHLSINF_1_15 -Wno-unknown-pragmas" csimflags=" -Wno-unknown-pragmas" blackbox="false"/>
        <file name="../../../src/test_conv2D.cpp" sc="0" tb="1" cflags=" -DHLSINF_1_15 -Wno-unknown-pragmas" csimflags=" -Wno-unknown-pragmas" blackbox="false"/>
        <file name="../../../src/test_check.cpp" sc="0" tb="1" cflags=" -DHLSINF_1_15 -Wno-unknown-pragmas" csimflags=" -Wno-unknown-pragmas" blackbox="false"/>
        <file name="../../../src/test_buffers.cpp" sc="0" tb="1" cflags=" -DHLSINF_1_15 -Wno-unknown-pragmas" csimflags=" -Wno-unknown-pragmas" blackbox="false"/>
        <file name="../../../src/test_arguments.cpp" sc="0" tb="1" cflags=" -DHLSINF_1_15 -Wno-unknown-pragmas" csimflags=" -Wno-unknown-pragmas" blackbox="false"/>
        <file name="../../../src/data_test.cpp" sc="0" tb="1" cflags=" -DHLSINF_1_15 -Wno-unknown-pragmas" csimflags=" -Wno-unknown-pragmas" blackbox="false"/>
        <file name="../src/batch_normalization.cpp" sc="0" tb="false" cflags="-DHLSINF_1_15" csimflags="" blackbox="false"/>
        <file name="../src/stm.cpp" sc="0" tb="false" cflags="-DHLSINF_1_15" csimflags="" blackbox="false"/>
        <file name="../src/write.cpp" sc="0" tb="false" cflags="-DHLSINF_1_15" csimflags="" blackbox="false"/>
        <file name="../src/winograd_convolution.cpp" sc="0" tb="false" cflags="-DHLSINF_1_15" csimflags="" blackbox="false"/>
        <file name="../src/serialization.cpp" sc="0" tb="false" cflags="-DHLSINF_1_15" csimflags="" blackbox="false"/>
        <file name="../src/relu.cpp" sc="0" tb="false" cflags="-DHLSINF_1_15" csimflags="" blackbox="false"/>
        <file name="../src/read.cpp" sc="0" tb="false" cflags="-DHLSINF_1_15" csimflags="" blackbox="false"/>
        <file name="../src/pooling.cpp" sc="0" tb="false" cflags="-DHLSINF_1_15" csimflags="" blackbox="false"/>
        <file name="../src/padding.cpp" sc="0" tb="false" cflags="-DHLSINF_1_15" csimflags="" blackbox="false"/>
        <file name="../src/mul.cpp" sc="0" tb="false" cflags="-DHLSINF_1_15" csimflags="" blackbox="false"/>
        <file name="../src/k_conv2D.cpp" sc="0" tb="false" cflags="-DHLSINF_1_15" csimflags="" blackbox="false"/>
        <file name="../src/join_split.cpp" sc="0" tb="false" cflags="-DHLSINF_1_15" csimflags="" blackbox="false"/>
        <file name="../src/dws_convolution.cpp" sc="0" tb="false" cflags="-DHLSINF_1_15" csimflags="" blackbox="false"/>
        <file name="../src/direct_convolution.cpp" sc="0" tb="false" cflags="-DHLSINF_1_15" csimflags="" blackbox="false"/>
        <file name="../src/cvt.cpp" sc="0" tb="false" cflags="-DHLSINF_1_15" csimflags="" blackbox="false"/>
        <file name="../src/conv2D.h" sc="0" tb="false" cflags="-DHLSINF_1_15" csimflags="" blackbox="false"/>
        <file name="../src/add_data.cpp" sc="0" tb="false" cflags="-DHLSINF_1_15" csimflags="" blackbox="false"/>
        <file name="../src/add.cpp" sc="0" tb="false" cflags="-DHLSINF_1_15" csimflags="" blackbox="false"/>
    </files>
    <solutions xmlns="">
        <solution name="AlveoU200" status="inactive"/>
        <solution name="CONF_ALVEO_U200_4x4_DIRECT_FP32" status="inactive"/>
        <solution name="CONF_ALVEO_U280_4x4_DIRECT_FP32" status="inactive"/>
        <solution name="CONF_ALVEO_U200_4x4_DIRECT_STM_FP32" status="inactive"/>
        <solution name="CONF_ALVEO_U200_8x8_DIRECT_APF8" status="inactive"/>
        <solution name="CONF_ALVEO_U200_4x4_DIRECT_MIXEDPRECISION_TEST" status="inactive"/>
        <solution name="CONF_ALVEO_U200_4x4_DIRECT_FP32_TEST" status="inactive"/>
        <solution name="CONF_ALVEO_U200_8x8_DIRECT_MIXEDPRECISION_TEST" status="inactive"/>
        <solution name="CONF_ALVEO_U200_16x16_DIRECT_MIXEDPRECISION_TEST" status="inactive"/>
        <solution name="HLSINF_1_1" status="inactive"/>
        <solution name="HLSINF_1_0" status="inactive"/>
        <solution name="HLSINF_TEST2" status="inactive"/>
        <solution name="HLSINF_1_3" status="inactive"/>
        <solution name="HLSINF_TEST" status="inactive"/>
        <solution name="HLSINF_TESTç" status="inactive"/>
        <solution name="CONF_1_0" status="inactive"/>
        <solution name="HLSINF_TEST3" status="inactive"/>
        <solution name="HLSINF_1_14" status="inactive"/>
        <solution name="HLSINF_1_11" status="inactive"/>
        <solution name="HLSINF_1_15" status="inactive"/>
        <solution name="NoConfig" status="active"/>
    </solutions>
</project>

