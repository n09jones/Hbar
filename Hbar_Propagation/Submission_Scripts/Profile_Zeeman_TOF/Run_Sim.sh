OPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && cd Options_Files && pwd )"
EXE_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && cd .. && cd .. && pwd )"

DET_OPT_DIR="${OPT_DIR}/Spec_Det"
eval "mkdir -p ${DET_OPT_DIR}"

TRAP=$1

if [ "$1" = "Straight_Oct" ]; then
	TOTPARTS="929"
	ISTR="CTRAP_STRAIGHT_OCT_TRAP_QUANTS_Linear_Interp"
elif [ "$1" = "Oct" ]; then
	TOTPARTS="954"
	ISTR="CTRAP_STD_OCT_TRAP_QUANTS_Linear_Interp"
else
	TOTPARTS="916"
	ISTR="CTRAP_STD_QUAD_TRAP_QUANTS_Linear_Interp"
fi


if [ "$2" = "DL" ]; then
	CUNC="_Doppler_Lim_ICs.bin"
elif [ "$2" = "C" ]; then
	if [ "$1" = "Straight_Oct" ]; then
		CUNC="_z_cooled.bin"
	else
		CUNC="_3_cooled.bin"
	fi
else
	TOTPARTS="1000"
	CUNC="_ICs_uncooled_fin.bin"
fi



FN="${OPT_DIR}/$1_$2_Opts.txt"

cat ${OPT_DIR}/Opt_template.txt > ${FN}
echo "Total Partitions = ${TOTPARTS}" >> ${FN}
echo "Interpolation Source Directory B = ${ISTR}" >> ${FN}
echo "Interpolation Source Directory Bnorm = ${ISTR}" >> ${FN}
echo "Interpolation Source Directory acc_1Sd = ${ISTR}" >> ${FN}
echo "Initial Condition File Path = STD_ICs/${TRAP}${CUNC}" >> ${FN}
echo "Laser Path 1 File = Submission_Scripts/Laser_Path_Options_Files/CW_1S2S/${TRAP}/Laser_Path_1.txt" >> ${FN}

for (( IDX=0; IDX<${TOTPARTS}; IDX++ ))
do
	echo "Index : ${IDX}"
	if [ -f "/n/scratchlfs/gabrielse_lab/njones/Propagation_Data/Zeeman_${TRAP}_$2_x_100_${IDX}.bin" ]; then
		echo "File exists"
	else
		cd ${EXE_DIR} && ./Profile_Zeeman_TOF --opts_fn ${FN} --part_group_idx 0 --part_member_idx ${IDX}
	fi
done
