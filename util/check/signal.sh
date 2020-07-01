noks_r=$(grep -c received $recordfile)
noks_a=$(grep -c received $answerfile)
nngs_r=$(grep -c '\[  NG  \]' $recordfile)

if (( noks_r != noks_a )) || (( nngs_r != 0 )); then
    rc=1
fi
