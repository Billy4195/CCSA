#!bin/bash

function get_ncpu {
  ncpu=$(cat /proc/cpuinfo | grep 'cpu cores'| uniq  | awk {'print $4'})
}

get_ncpu
while [[ $# -gt 1 ]]
do
  key="$1"
  case $key in
    cpu)
      cpu=$2
      cpu=$(($cpu * $ncpu))
      shift
      ;;
    mem)
      mem=$2
      shift
      ;;
    io)
      io=$2
      shift
      ;;
    t)
      t=$2
      shift
      ;;
  esac

  shift
done
echo $cpu $mem $io $t
./cpulimit -i -l $cpu ./workload cpu $cpu mem $mem io $io t $t
