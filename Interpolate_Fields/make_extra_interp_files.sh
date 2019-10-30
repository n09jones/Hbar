#!/bin/bash

for TRAP in QUAD OCT STRAIGHT_OCT
do
	for DEG in LINEAR QUADRATIC TRILINEAR
	do
		for div in 2 5
		do
			FN=${TRAP}_gen_${DEG}_div_${div}.txt
			echo "Total Partitions = 100" > ${FN}
			echo "Time (in mins) = 180" >> ${FN}
			echo "SLURM partitions = shared,serial_requeue" >> ${FN}
			echo "Interpolation Source Directory = CTRAP_${TRAP}_TRAP_QUANTS_div_${div}" >> ${FN}
			echo "Interpolation Mode = ${DEG}" >> ${FN}
			echo "Interpolation Output Directory = CTRAP_${TRAP}_TRAP_QUANTS_${DEG}_div_${div}" >> ${FN}

			./Interpolate_Fields --cluster --opts_fn ${FN}
		done
	done
done
