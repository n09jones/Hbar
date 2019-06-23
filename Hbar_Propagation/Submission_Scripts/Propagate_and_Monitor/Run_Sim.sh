OPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && cd Options_Files && pwd )"
EXE_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && cd .. && cd .. && pwd )"

for FN in ${OPT_DIR}/*.txt
do
	echo "Processing ${FN}"
	eval "cd ${EXE_DIR} && ${EXE_DIR}/Propagate_and_Monitor --opts_fn ${FN} --part_group_idx 0 --part_member_idx $1"
done
