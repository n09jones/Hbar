OPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && cd Options_Files && pwd )"
EXE_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && cd .. && cd .. && pwd )"

TRAPS=("Oct" "Quad" "Straight_Oct")
COOLED=("Cooled" "Uncooled")

CLSTR_STR=""
if [ "$1" != "local" ]; then
        CLSTR_STR="--cluster"
fi

for trap in ${TRAPS[@]}
do
	for cooled in ${COOLED[@]}
	do
        	echo "Running for ${cooled} atoms in a ${trap} trap..."

		eval "cd ${EXE_DIR} && ${EXE_DIR}/Dump_Trap --opts_fn ${OPT_DIR}/Dump_${trap}_${cooled}_Opts.txt ${CLSTR_STR}"
	done
done
