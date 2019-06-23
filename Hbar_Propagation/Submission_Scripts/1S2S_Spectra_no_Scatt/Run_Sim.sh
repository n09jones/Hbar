OPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && cd Options_Files && pwd )"
EXE_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && cd .. && cd .. && pwd )"

DET_OPT_DIR="${OPT_DIR}/Spec_Det"
eval "mkdir -p ${DET_OPT_DIR}"

TRAP=${1}

OUTFN="${DET_OPT_DIR}/${TRAP}_LO_${2}_HI_${3}_INCR_${4}.txt"
eval "cat ${OPT_DIR}/${TRAP}.txt > ${OUTFN}"
eval "echo Excited State F = 1 >> ${OUTFN}"
eval "echo Excited State mF = -1 >> ${OUTFN}"
eval "echo Output File Path = STD_CW_1S2S_Spectrum_Cooled_no_Scatt/${TRAP}/In_Hz_LO_${2}_HI_${3}_INCR_${4}.bin >> ${OUTFN}"

for (( DET=${2}; DET<=${3}; DET+=${4} ))
do
	OUTFN_DET="${DET_OPT_DIR}/${TRAP}_LO_${2}_HI_${3}_INCR_${4}_${DET}.txt"
	eval "cat ${OUTFN} > ${OUTFN_DET}"
	eval "echo Laser Detuning = ${DET} >> ${OUTFN_DET}"
done


SRC_NM="${EXE_DIR}/src/1S2S_Multiple_Det_no_Scatt_${TRAP}_${2}_${3}_${4}.cpp"

echo '#include "'"${EXE_DIR}"'/src/1S2S_Multiple_Det_no_Scatt.hpp"' > "${SRC_NM}"
echo "" >> "${SRC_NM}"
echo 'const std::vector<int> gl_detuning_vec' >> "${SRC_NM}"
echo '{' >> "${SRC_NM}"
for (( DET=${2}; DET<=${3}; DET+=${4} ))
do
	echo "${DET}," >> "${SRC_NM}"
done
echo '};' >> "${SRC_NM}"
echo "" >> "${SRC_NM}"

cd "${EXE_DIR}" && make
