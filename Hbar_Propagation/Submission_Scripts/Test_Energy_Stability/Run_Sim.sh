OPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && cd Options_Files && pwd )"
EXE_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && cd .. && cd .. && pwd )"


for trap in Oct Quad Straight_Oct
do
#	for STEP_NS in 1000 500 2000
	for STEP_NS in 100
	do

		DET_OPT_DIR="${OPT_DIR}/${STEP_NS}"
		eval "mkdir -p ${DET_OPT_DIR}"
		
		OUTFN="${DET_OPT_DIR}/${trap}.txt"
	        eval "cat ${OPT_DIR}/${trap}.txt > ${OUTFN}"
		eval "echo Excited State 2J = 1 >> ${OUTFN}"
		eval "echo Excited State 2mJ = 1 >> ${OUTFN}"
		eval "echo Excited State 2mI = 1 >> ${OUTFN}"
	        eval "echo Laser Detuning = 0.0e6 >> ${OUTFN}"
		eval "echo Time Per Step = ${STEP_NS}e-9" >> ${OUTFN}
	        eval "echo Output File Path = Test_Energy_Stability/${STEP_NS}/${trap}_Quad_interp.bin >> ${OUTFN}"

		eval "cd ${EXE_DIR} && ${EXE_DIR}/Test_Energy_Stability --opts_fn ${OUTFN} --cluster"
	done
done
