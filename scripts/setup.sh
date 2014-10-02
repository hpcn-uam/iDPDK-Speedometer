export RTE_TARGET=x86_64-native-linuxapp-gcc
export RTE_SDK=../Intel_DPDK/DPDKCUR

SCRIPTNAME=dpdk_nic_bind.py

curr=$(pwd)
cd $RTE_SDK
make config T=$RTE_TARGET -j9
make clean -j9
make install T=$RTE_TARGET -j9
cd $RTE_TARGET
modprobe uio
make -j9
rmmod igb_uio
insmod kmod/igb_uio.ko
cd ../tools
./$SCRIPTNAME --force -b igb_uio 0000:04:00.0
./$SCRIPTNAME --force -b igb_uio 0000:04:00.1
#./$SCRIPTNAME --force -b ixgbe 0000:04:00.1
./$SCRIPTNAME --status
cd $curr
