OPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && cd Options_Files && pwd )"
EXE_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && cd .. && cd .. && pwd )"

TRAPS=("Oct" "Quad" "Straight_Oct")

CLSTR_STR=""
if [ "$1" != "local" ]; then
        CLSTR_STR="--cluster"
fi

for trap in ${TRAPS[@]}; do
        echo "Running ${trap}..."
        eval "${EXE_DIR}/Make_${trap}_ICs --opts_fn ${OPT_DIR}/${trap}_IC_Opts.txt ${CLSTR_STR}"
done
