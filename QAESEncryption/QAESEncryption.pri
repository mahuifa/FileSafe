HEADERS += \
    $$PWD/aesni/aesni-enc-cbc.h \
    $$PWD/aesni/aesni-enc-ecb.h \
    $$PWD/aesni/aesni-key-exp.h \
    $$PWD/qaesencryption.h

SOURCES += \
    $$PWD/qaesencryption.cpp

#DEFINES += USE_INTEL_AES_IF_AVAILABLE         # 不建议开启，否则性能较低的电脑会直接异常退出
#QMAKE_CXXFLAGS += -maes
