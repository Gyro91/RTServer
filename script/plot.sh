#!/bin/sh

	EXIT_SUCCESS=0
	var1=0
	var2=0
	message=1000	
	period=1000
	media=0
	worst=0

	rm tools/*.dat
  
	#gnuplot
	#plot 'plotting_data1.dat' with linespoints ls 1		
	for i in `seq 1 4`;
	do
	cd /tmp
	rm -f myfifo*
	cd /home/matteo/Desktop/project/script
	rm -f tools/out_c*
	cd ../server
	gnome-terminal -e ./server &
	sleep 1
	cd ../generator
	gnome-terminal -x bash -c "./generator -m$message -p$period" &
	sleep 7
	
	cd ../script
	while true;
		do
		var1=$(grep -c ^ tools/out_c1.txt)
		var2=$(grep -c ^ tools/out_c2.txt)
		var1=$(( var1 + var2 ))
	
	if [ $var1 = $message ]
	then
        	break
	fi
	done
	
	cd ../server
	killall  server
	
	cd ../script/tools
	
	media=$(./media out_c1.txt)
	echo "  $period   $media" >> media1.dat
	media=$(./media out_c2.txt)
	echo "  $period   $media" >> media2.dat
	

	worst=$(./worst out_c1.txt)
	echo "  $period   $worst" >> worst1.dat
	worst=$(./worst out_c2.txt)
	echo "  $period   $worst" >> worst2.dat
	cd ..
	period=$(( period + 35000 ))
	done

	cd tools
	gnuplot	
	exit $EXIT_SUCCESS
