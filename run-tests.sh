#!/bin/sh

TESTDIR=tests
OUTPUT=`mktemp`
LOGFILE=tests.log
CMD=./figlet

run_test() {
	test_num=$1
	test_dsc=$2
	test_cmd=$3

	echo >> $LOGFILE
	echo -n "Run test $test_num: ${test_dsc}... " | tee -a $LOGFILE
	echo >> $LOGFILE
	eval "$test_cmd" > "$OUTPUT" 2>> $LOGFILE
	cmp "$OUTPUT" "tests/res${test_num}.txt" >> $LOGFILE 2>&1
	if [ $? -eq 0 ]; then
		echo "pass" | tee -a $LOGFILE
	else
		echo "**fail**" | tee -a $LOGFILE
		result=1
		fail=`expr $fail + 1`
	fi
}

result=0
fail=0
$CMD -v > $LOGFILE
$CMD -f small "Test results" | tee -a $LOGFILE

file="$TESTDIR/input.txt"
cmd="cat $file|$CMD"

run_test 001 "showfigfonts output" "./showfigfonts"
run_test 002 "text rendering in all fonts" \
  "for i in fonts/*.flf; do $cmd -f \$i; done"
run_test 003 "long text rendering" "cat tests/longtext.txt|./figlet"
run_test 004 "left-to-right text" "$cmd -L"
run_test 005 "right-to-left text" "$cmd -R"
run_test 006 "flush-left justification" "$cmd -l"
run_test 007 "flush-right justification" "$cmd -r"
run_test 008 "center justification" "$cmd -c"
run_test 009 "kerning mode" "$cmd -k"
run_test 010 "full width mode" "$cmd -W"
run_test 011 "overlap mode" "$cmd -o"
run_test 012 "TLF font rendering" "$cmd -f tests/emboss"
run_test 013 "kerning flush-left right-to-left mode" "$cmd -klR"
run_test 014 "kerning centered right-to-left mode (slant)" "$cmd -kcR -f slant"
run_test 015 "full-width flush-right right-to-left mode" "$cmd -WrR"
run_test 016 "overlap flush-right mode (big)" "$cmd -or -f big"

rm -f "$OUTPUT"

if [ $result -ne 0 ]; then
	echo "\n $fail tests failed. See $LOGFILE for result details"
else
	echo "\n All tests passed."
fi

exit $result
