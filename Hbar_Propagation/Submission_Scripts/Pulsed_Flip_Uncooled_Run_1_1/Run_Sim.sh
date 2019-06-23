OPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && cd Options_Files && pwd )"
EXE_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && cd .. && cd .. && pwd )"

DET_OPT_DIR="${OPT_DIR}/Spec_Det"
eval "mkdir -p ${DET_OPT_DIR}"

TRAPS=("Oct" "Quad" "Straight_Oct")
DETS=("-100" "-500" "-100")
LAS_AX=("3" "x" "z")

GOOD_2J=1
GOOD_2mJ=1
GOOD_2mI=-1

CLSTR_STR=""
if [ "$1" != "local" ]; then
        CLSTR_STR="--cluster"
fi

for trapidx in {0..2}
do
	trap=${TRAPS[${trapidx}]}

	for ax in ${LAS_AX[@]}
	do
        	echo "Running ${trap} with laser state ${ax}..."
		
		for (( DET=${DETS[${trapidx}]}; DET<=${DETS[${trapidx}]}; DET+=50 ))
		do
			echo "Detuning = ${DET}"

			if [ ${DET} -lt 0 ]
        		then
            			DETNM=$(( 0 - DET ))
                		DETNM="n${DETNM}"
        		else
            			DETNM=${DET}
        		fi

			for (( TWO_J=1; TWO_J<=3; TWO_J+=2 ))
			do
				for (( TWO_mJ=-TWO_J; TWO_mJ<=TWO_J; TWO_mJ+=2 ))
				do
					for (( TWO_mI=-1; TWO_mI<=1; TWO_mI+=2 ))
					do
						BADDIR="${DET_OPT_DIR}/Bad_Transitions/2J_${TWO_J}_2mJ_${TWO_mJ}_2mI_${TWO_mI}"
						eval "mkdir -p ${BADDIR}"

						OUTFN="${BADDIR}/${trap}_${ax}_${DETNM}.txt"
						eval "cat ${OPT_DIR}/${trap}_${ax}.txt > ${OUTFN}"
			                        eval "echo Excited State 2J = ${TWO_J} >> ${OUTFN}"
                			        eval "echo Excited State 2mJ = ${TWO_mJ} >> ${OUTFN}"
                        			eval "echo Excited State 2mI = ${TWO_mI} >> ${OUTFN}"
                        			eval "echo Laser Detuning = ${DET}e6 >> ${OUTFN}"
					done
				done
			done

			OUTFN="${DET_OPT_DIR}/${trap}_${ax}_${DETNM}.txt"
		        eval "cat ${OPT_DIR}/${trap}_${ax}.txt > ${OUTFN}"
			eval "echo Excited State 2J = ${GOOD_2J} >> ${OUTFN}"
			eval "echo Excited State 2mJ = ${GOOD_2mJ} >> ${OUTFN}"
			eval "echo Excited State 2mI = ${GOOD_2mI} >> ${OUTFN}"
		        eval "echo Laser Detuning = ${DET}e6 >> ${OUTFN}"
		        eval "echo Output File Path = STD_Pulsed_Flip_Uncooled_Run_${GOOD_2J}_${GOOD_2mJ}/${trap}_${ax}/Det_${DETNM}.bin >> ${OUTFN}"

			eval "cd ${EXE_DIR} && ${EXE_DIR}/Pulsed_LyAlpha_Monitor_w_other_transitions --opts_fn ${OUTFN} ${CLSTR_STR}"
		done
	done
done
