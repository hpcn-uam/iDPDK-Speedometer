export RTE_TARGET=x86_64-native-linuxapp-gcc
export RTE_SDK=$(pwd)/../Intel_DPDK/DPDKCUR

export EXTRA_LDFLAGS=-L/root/dpdk/Intel_DPDK/extensions/mlx4/2015-03-26-DPDK-v2.0.0/install/usr/local/lib

clear
make clean -j9
make -j9
if [ $? -eq 0 ] ; then

	# c = numero de procesadores
	# n = numero de canales de memoria
	# --rx "(PORT, QUEUE, LCORE), ..." : List of NIC RX ports and queues       
        # tx "(PORT, LCORE), ..." : List of NIC TX ports handled by the I/O TX   
        # w "LCORE, ..." : List of the worker lcores                             
        # OPTIONAL:                                                                     
	# rsz "A, B, C, D" : Ring sizes                                          
        #   A = Size (in number of buffer descriptors) of each of the NIC RX    
        #       rings read by the I/O RX lcores (default value is 1024)           
        #   B = Size (in number of elements) of each of the SW rings used by the
        #       I/O RX lcores to send packets to worker lcores (default value is
        #       1024)                                                             
        #   C = Size (in number of elements) of each of the SW rings used by the
        #       worker lcores to send packets to I/O TX lcores (default value is
        #       1024)                                                             
        #   D = Size (in number of buffer descriptors) of each of the NIC TX    
        #       rings written by I/O TX lcores (default value is 1024)            
	# bsz "(A, B), (C, D), (E, F)" :  Burst sizes                            
        #   A = I/O RX lcore read burst size from NIC RX (default value is 144)  
        #   B = I/O RX lcore write burst size to output SW rings (default value 
        #       is 144)                                                          
        #   C = Worker lcore read burst size from input SW rings (default value 
        #       is 144)                                                          
        #   D = Worker lcore write burst size to output SW rings (default value 
        #       is 144)                                                          
        #   E = I/O TX lcore read burst size from input SW rings (default value 
        #       is 144)                                                          
        #   F = I/O TX lcore write burst size to NIC TX (default value is 144)   
	# pos-lb POS : Position of the 1-byte field within the input packet used by
        #   the I/O RX lcores to identify the worker lcore for the current      
        #   packet (default value is 29)    

	build/app/hpcn_n2d -c 3E -n 2 $1 -- --rx "(0,0,1),(1,0,2)" --tx "(0,1),(1,2)" \
                --rsz "1024, 64, 64, 64" \
                --bsz "(144, 512), (512, 144), (144, 144)" 


else
	echo ""
	echo "Error en la compilacion..."
fi
