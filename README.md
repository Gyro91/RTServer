# RTServer

Before run:

1) In /proc/sys/fs/mqueue/
		modify msg_max & queues_max to 10000

2) ulimit to modify byte reserved to mqueue in the terminal where you want to run ( 1500000 should be ok )
