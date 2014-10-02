echo $1 > /sys/bus/pci/devices/0000\:04\:00.0/max_vfs
#echo $1 > /sys/bus/pci/devices/0000\:04\:00.1/max_vfs

echo -n "Nic 0: "
cat /sys/bus/pci/devices/0000\:04\:00.0/max_vfs

echo -n "Nic 1: "
cat /sys/bus/pci/devices/0000\:04\:00.1/max_vfs
