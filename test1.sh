#!/bin/bash

#CPU clock time
CLK_TCK=$(getconf CLK_TCK)

start_time=$(date +%s%N)
#time for highest accuracy on time taken on the process
time ./process1.sh &
pid=$!
utime=$(grep -w "$pid" /proc/$pid/stat | awk '{print $14}')
stime=$(grep -w "$pid" /proc/$pid/stat | awk '{print $15}')
ctxt=$(grep ctxt /proc/$pid/status | awk '{ print $2 }')
#watch -n.5 grep ctxt /proc/$pid/status
#most accurate approach to acquire end_time if process started in this script:
wait $pid
end_time=$(date +%s%N)

#Warning: CPU time 0 if a process "just" sleeps
#sleep is a non-CPU-bound process!
cpu_time=$(((utime + stime) * 1000000000 /CLK_TCK))

turnaround_time=$((end_time - start_time))
echo "Turnaround Time $pid: $turnaround_time nanoseconds"

waiting_time=$((turnaround_time - cpu_time))
echo "Waiting Time $pid: $waiting_time nanoseconds"

echo "Context Switches $pid (voluntary & involuntary): $ctxt"
