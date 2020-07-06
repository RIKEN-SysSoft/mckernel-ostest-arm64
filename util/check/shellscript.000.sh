ndiff=$(diff $recordfile $answerfile |grep -cE '^>')
(( ndiff != 1 )) && rc=1
