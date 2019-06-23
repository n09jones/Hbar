#!/bin/bash

DIRNM="Freqs_-50k_50k_50_Survey"

OPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && cd Options_Files && pwd )"
EXE_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && cd .. && cd .. && pwd )"

# Arguments : Coil, U/C/DL, radius (m), power (W), x/z
# (U/C/DL = Uncooled/Cooled/Doppler Limit)
function write_opts_files
{
	rad="$3"
	pow="$4"

	rad_str="${rad/./p}"
	pow_str="${pow/./p}"

	IC_str="STD_ICs/$1"
	if [ "$2" = "U" ]; then
		IC_str="${IC_str}_ICs_uncooled_fin"
	elif [ "$2" = "C" ]; then
		if [ "$1" = "Straight_Oct" ]; then
			IC_str="${IC_str}_z_cooled"
		else
			IC_str="${IC_str}_3_cooled"
		fi
	elif [ "$2" = "DL" ]; then
		IC_str="${IC_str}_Doppler_Lim_ICs"
	else
		echo "$2 must be U, C, or DL"
		exit 1
	fi
	IC_str="${IC_str}.bin"

	fn="${OPT_DIR}/Full_Opts/$1_$2_r${rad_str}_P${pow_str}_$5_Opts.txt"
	laser_fn="${OPT_DIR}/Laser_Opts/$1_r${rad_str}_P${pow_str}_$5_Opts.txt"

	cat ${OPT_DIR}/template_Opts/gen_template.txt > ${fn}
	echo "" >> ${fn}
	cat ${OPT_DIR}/template_Opts/$1_template.txt >> ${fn}
	echo "" >> ${fn}
	echo "# Relative to "large_storage_dir" in Global_Info.h." >> ${fn}
	echo "Initial Condition File Path = ${IC_str}" >> ${fn}
	echo "" >> ${fn}
	echo "Laser Path 1 File = ${laser_fn}" >> ${fn}
	echo "" >> ${fn}
	echo "Output File Path = STD_CW_1S2S_Spectrum_Arr_Full_Sweep/${DIRNM}/$1_$2_${rad_str}_${pow_str}_$5.bin" >> ${fn}
	echo "" >> ${fn}

	echo "Power = $4" > ${laser_fn}
	echo "" >> ${laser_fn}
	cat ${OPT_DIR}/template_Opts/Laser_Path_$5_template.txt >> ${laser_fn}
	echo "" >> ${laser_fn}
	echo "Beam waist (radius) = $3" >> ${laser_fn}
	echo ""	>> ${laser_fn}
	if [ $1 = "Oct" ]; then
		echo "Focus_z = 0.0226" >> ${laser_fn}
	else
		echo "Focus_z = 0" >> ${laser_fn}
	fi
	echo ""	>> ${laser_fn}

	cd ${EXE_DIR} && ./CW_1S2S_Arr_gen --opts_fn ${fn}
}

declare -a CUarr=("C" "U", "DL")
declare -a rarr=("250e-6" "0.001" "0.003")
declare -a COILarr=("Oct" "Quad" "Straight_Oct")
declare -a xzarr=("x" "z")

for CU in "${CUarr[@]}"
do
	for r in "${rarr[@]}"
	do
		for COIL in "${COILarr[@]}"
		do
			for xz in "${xzarr[@]}"
			do
				write_opts_files ${COIL} ${CU} ${r} 1 ${xz}
			done
		done
	done
done

