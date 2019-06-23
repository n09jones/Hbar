OPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && cd Options_Files && pwd )"
EXE_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && cd .. && cd .. && pwd )"

DET_OPT_DIR="${OPT_DIR}/Spec_Det"
eval "mkdir -p ${DET_OPT_DIR}"

TRAPS=("Oct" "Quad" "Straight_Oct")
LAS_AX=("x" "z")

CLSTR_STR=""
if [ "$1" != "local" ]; then
        CLSTR_STR="--cluster"
fi

for trap in ${TRAPS[@]}
do
	for ax in ${LAS_AX[@]}
	do
        	echo "Running ${trap} with laser state ${ax}..."
		
		for (( DET=-300; DET<=300; DET+=50 ))
		do
			echo "Detuning = ${DET}"

			if [ ${DET} -lt 0 ]
        		then
            			DETNM=$(( 0 - DET ))
                		DETNM="n${DETNM}"
        		else
            			DETNM=${DET}
        		fi


			OUTFN="${DET_OPT_DIR}/${trap}_${ax}_${DETNM}.txt"
		        eval "cat ${OPT_DIR}/${trap}_${ax}.txt > ${OUTFN}"
			eval "echo Excited State F = 1 >> ${OUTFN}"
			eval "echo Excited State mF = -1 >> ${OUTFN}"
		        eval "echo Laser Detuning = ${DET}e3 >> ${OUTFN}"
		        eval "echo Output File Path = STD_CW_1S2S_Spectrum_Cooled_Test/${trap}_${ax}/Det_${DETNM}.bin >> ${OUTFN}"

			eval "cd ${EXE_DIR} && ${EXE_DIR}/CW_1S2S --opts_fn ${OUTFN} ${CLSTR_STR}"
		done
	done
done
