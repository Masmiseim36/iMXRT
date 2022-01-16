extension="_Registers.xml"
path=$(dirname "$0")
echo $path


#  -reg-size-bytes          SVD register size definitions are in bytes
#  -ignore-description      Ignore description fields in SVD file
#  -irq-table name          Create an assembly code vector table in 'name'.vec
#  -irq-start number        The vector entry number to start from default is 16
#  -irqs irq0;irq1;..       Specify the list of interrupt names
#  -irqs2 filename          Get the interrupt names and CPU info from a CMSIS header 'filename'
#  -ignore-name name        Ignore all entries with 'name'
#  -tcf                     Input file is a tcf file not an SVD file
#  -ddf                     Input file is a ddf file not an SVD file
#  -rdt                     Input file is a rdt file not an SVD file
#  -nvic p                  Create an NVIC registergroup with 'p' priority bits
#  -pp                      Create PPB register groups e.g. nvic, scb, systick, mpu
#  -remove name;name;..     Remove the list of register groups
#  -include f               Add an <include file_name='f'
#  -include-macros m        Add an <include file_name='f' macros='m'
#  -core n                  Core type is Cortex-M'n'
#  -mpu                     Core has an mpu
#  -fpu                     Core has an fpu

for file in *; do 
	if [ -f "$file" ]; then 
		echo "$file" 
		filename="${file%.*}"
		echo $filename$extension
		
		# nvic: M0: 2  -  M4/7: 4  compare __NVIC_PRIO_BITS
		# core: M0, M4, M7,
		
		# For Cortex M4 / M7
		echo "C:\Program Files\Rowley CrossWorks for ARM 4.8\bin\svd2mm.exe" -pp -core 4 -mpu -fpu -pp -nvic 4 -irq-start 0 -irq-table $filename $file $filename$extension
		"C:\Program Files\Rowley CrossWorks for ARM 4.8\bin\svd2mm.exe" -pp -core 4 -mpu -fpu -pp -nvic 4 -irq-start 0 -irq-table $filename $file $filename$extension
		
		# For Cortex M0
#		echo "C:\Program Files\Rowley CrossWorks for ARM 4.8\bin\svd2mm.exe" -pp -core 0 -pp -nvic 2 -irq-start 0 -irq-table $filename $file $filename$extension
#		"C:\Program Files\Rowley CrossWorks for ARM 4.8\bin\svd2mm.exe" -pp -core 0 -pp -nvic 2 -irq-start 0 -irq-table $filename $file $filename$extension

		# For Cortex M33
#		echo "C:\Program Files\Rowley CrossWorks for ARM 4.8\bin\svd2mm.exe" -pp -core 33 -mpu -fpu -pp -nvic 4 -irq-start 0 -irq-table $filename $file $filename$extension
#		"C:\Program Files\Rowley CrossWorks for ARM 4.8\bin\svd2mm.exe" -pp -core 33 -mpu -fpu -pp -nvic 4 -irq-start 0 -irq-table $filename $file $filename$extension
	fi 
done

read -n 1 -s -r -p "Press any key to continue"
