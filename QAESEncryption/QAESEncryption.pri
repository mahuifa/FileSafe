HEADERS += \
    $$PWD/aesni/aesni-enc-cbc.h \
    $$PWD/aesni/aesni-enc-ecb.h \
    $$PWD/aesni/aesni-key-exp.h \
    $$PWD/qaesencryption.h

SOURCES += \
    $$PWD/qaesencryption.cpp

#DEFINES += USE_INTEL_AES_IF_AVAILABLE         # 开启后出现过有些电脑异常退出情况
#QMAKE_CXXFLAGS += -maes
