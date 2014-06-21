if [ -z $CHISRC ]; then
        echo 'Please set CHISRC'
        exit 1
fi

CHIDST=.

mkdir -p $CHIDST/boards
mkdir -p $CHIDST/os/hal/platforms
mkdir -p $CHIDST/os/ports/GCC/ARMCMx
mkdir -p $CHIDST/os/ports/GCC/STM32F0xx
cp -r $CHISRC/boards/ST_STM32F0_DISCOVERY $CHIDST/boards/
cp -r $CHISRC/os/hal/platforms/STM32F0xx $CHIDST/os/hal/platforms/
cp -r $CHISRC/os/hal/platforms/STM32 $CHIDST/os/hal/platforms/
cp -r $CHISRC/os/hal/include $CHIDST/os/hal/
cp -r $CHISRC/os/hal/hal.mk $CHIDST/os/hal/
cp -r $CHISRC/os/hal/src $CHIDST/os/hal/
cp -r $CHISRC/os/kernel $CHIDST/os/
cp -r $CHISRC/os/ports/common $CHIDST/os/ports/
cp -r $CHISRC/os/ports/GCC/ARMCMx/*.[ch] $CHIDST/os/ports/GCC/ARMCMx/
cp -r $CHISRC/os/ports/GCC/ARMCMx/rules.mk $CHIDST/os/ports/GCC/ARMCMx/
cp -r $CHISRC/os/ports/GCC/ARMCMx/STM32F0xx $CHIDST/os/ports/GCC/ARMCMx/
cp -r $CHISRC/test $CHIDST/
