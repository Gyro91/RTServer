#!/bin/sh

	EXIT_SUCCESS=0
	var1=0
	var2=0
	message=50	
	period=50
	
	rm ../server/media/out_c*
	#gnuplot
	#plot 'plotting_data1.dat' with linespoints ls 1		
	cd ../server
	gnome-terminal -e ./server &
	sleep 1
	cd ../generator
	gnome-terminal -x bash -c "./generator -m$message -p$period" &
	sleep 5
	
	while true;
	do
	var1=$(grep -c ^ ../server/media/out_c1.txt)
	var2=$(grep -c ^ ../server/media/out_c2.txt)
	var1=$(( var1 + var2 ))
	echo $var1

	if [ $var1 = $message ]
	then
        break
	fi

	sleep 5
	done
	
	exit $EXIT_SUCCESS
