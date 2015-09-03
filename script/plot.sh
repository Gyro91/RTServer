#!/bin/sh

	EXIT_SUCCESS=0
	var1=0
	var2=0
	message=50	
	period=50
	media=0
	worst=0

	rm -f tool/out_c*
	#gnuplot
	#plot 'plotting_data1.dat' with linespoints ls 1		
	cd ../server
	gnome-terminal -e ./server &
	sleep 1
	cd ../generator
	gnome-terminal -x bash -c "./generator -m$message -p$period" &
	sleep 5
	
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
	rm media1.dat
	rm media2.dat
	media=$(./media out_c1.txt)
	echo "  $media   $period" >> media1.dat
	media=$(./media out_c2.txt)
	echo "  $media   $period" >> media2.dat
	
	rm worst1.dat
	rm worst2.dat
	worst=$(./worst out_c1.txt)
	echo "  $worst   $period" >> worst1.dat
	worst=$(./worst out_c2.txt)
	echo "  $worst   $period" >> worst2.dat

	exit $EXIT_SUCCESS
