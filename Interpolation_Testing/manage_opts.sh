#!/bin/bash

ORDER="LINEAR"
order="lin"

for TRAP in QUAD OCT STRAIGHT_OCT
do
	for DIV in 2 5
	do
		OPTS_FN="tmp_opts_file.txt"
		echo "Coordinate Grid File Name = interp_check_pts.bin" > ${OPTS_FN}

		DIR="CTRAP_${TRAP}_TRAP_QUANTS_${ORDER}_div_${DIV}"
		echo "${DIR}"
		for QUANT in B Bnorm acc_1Sc acc_1Sd acc_2Sc acc_2Sd
		do
			echo "Interpolation Source Directory ${QUANT} = ${DIR}" >> ${OPTS_FN}
		done

		./Test_Interpolation --opts_fn ${OPTS_FN}

		mv /n/scratchlfs/gabrielse_lab/njones/${order}_interp_test /n/scratchlfs/gabrielse_lab/njones/${TRAP}_${ORDER}_${DIV}_interp_test
	done
done
