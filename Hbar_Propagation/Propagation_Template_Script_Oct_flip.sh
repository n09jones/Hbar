#!/bin/bash

INFN=$1
OUTDIR=$2
OUTPUTBASE=$3
CMD=$4

if [ ! -d "$OUTDIR" ]; then
	eval "mkdir ${OUTDIR}"
fi

for (( DET=-300; DET<=0; DET+=50 ))
do
	echo "Detuning = ${DET}"

	if [ ${DET} -lt 0 ]
	then
		DETNM=$(( 0 - DET ))
		DETNM="n${DETNM}"
	else
		DETNM=${DET}
	fi

	OUTFN="${OUTDIR}/Options_File_${DETNM}.txt"
	eval "cat ${INFN} > ${OUTFN}"
	eval "echo Laser Detuning = ${DET}e6 >> ${OUTFN}"
	eval "echo Output File Path = ${OUTPUTBASE}_${DETNM}.bin >> ${OUTFN}"
	eval "${CMD} --opts_fn ${OUTFN}"
done
