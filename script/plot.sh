#!/bin/sh
	TERMINAL="gnome-terminal"
#TERMINAL="konsole --separate"

	EXIT_SUCCESS=0
	var1=0
	var2=0
	message=1000
	period_min=10
	period_max=100000
	period_step=10
	period_step_log=true
	media=0
	worst=0

	rm tools/*.dat
  
	#gnuplot
	#plot 'plotting_data1.dat' with linespoints ls 1	
	period=$period_min
	#for i in `seq 1 10`;
	while [ $period -le $period_max ]; do
            cd /tmp
            rm -f wait_c
            cd -
            rm -f tools/out_c*
            touch tools/out_c1.txt
            touch tools/out_c2.txt
            cd ../server
	    gnome-terminal -e ./server &
            sleep 1
            cd ../generator
            #$TERMINAL -x bash -c "./generator -m$message -p$period" &
           gnome-terminal -x bash -c "./generator -m$message -p$period" &
            sleep 10
            
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
            if $period_step_log ; then
                period=$(( period * period_step ))
            else
                period=$(( period + period_step ))
            fi
        done

	cd tools
	#gnuplot	
	exit $EXIT_SUCCESS
