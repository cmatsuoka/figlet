#!/bin/sh

LC_ALL=POSIX
export LC_ALL

TESTDIR=tests
OUTPUT=`mktemp`
LOGFILE=tests.log
CMD=./figlet
FONTDIR="$1"

run_test() {
	test_num=$1
	test_dsc=$2
	test_cmd=$3

	echo >> $LOGFILE
	printf "Run test $test_num: ${test_dsc}... " | tee -a $LOGFILE
	echo >> $LOGFILE
	echo "Command: $test_cmd" >> $LOGFILE
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

file="$TESTDIR/input.txt"
cmd="cat $file|$CMD"

printf "Default font dir: "; $CMD -I2
if [ -n "$FONTDIR" ]; then
	FIGLET_FONTDIR="$FONTDIR"
	export FIGLET_FONTDIR
fi
printf "Current font dir: "; $CMD -I2
printf "Default font: "; $CMD -I3
$CMD -f small "Test results" | tee -a $LOGFILE

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
run_test 012 "tlf2 font rendering" "$cmd -f tests/emboss"
run_test 013 "kerning flush-left right-to-left mode" "$cmd -klR"
run_test 014 "kerning centered right-to-left mode (slant)" "$cmd -kcR -f slant"
run_test 015 "full-width flush-right right-to-left mode" "$cmd -WrR"
run_test 016 "overlap flush-right mode (big)" "$cmd -or -f big"
run_test 017 "tlf2 kerning flush-right mode" "$cmd -kr -f tests/emboss"
run_test 018 "tlf2 overlap centered mode" "$cmd -oc -f tests/emboss"
run_test 019 "tlf2 full-width flush-left right-to-left mode" \
  "$cmd -WRl -f tests/emboss"
run_test 020 "specify font directory" \
  "X=`mktemp -d`;cp fonts/script.flf \$X/foo.flf;$cmd -d\$X -ffoo;rm -Rf \$X"
run_test 021 "paragraph mode long line output" "$cmd -p -w250"
run_test 022 "short line output" "$cmd -w5"
run_test 023 "kerning paragraph centered mode (small)" "$cmd -kpc -fsmall"
run_test 024 "list of control files" "ls fonts/*flc"
run_test 025 "uskata control file" "printf 'ABCDE'|$CMD -fbanner -Cuskata"
run_test 026 "jis0201 control file" "printf '\261\262\263\264\265'|$CMD -fbanner -Cjis0201"

rm -f "$OUTPUT"

echo
if [ $result -ne 0 ]; then
	echo " $fail tests failed. See $LOGFILE for result details"
else
	echo " All tests passed."
fi

exit $result
