#!/bin/sh

OUTPUT=`mktemp`

run_test() {
	test_num=$1
	test_dsc=$2
	test_cmd=$3

	echo -n "Run test $test_num: ${test_dsc}... "
	eval "$test_cmd" > "$OUTPUT"
	cmp -s "$OUTPUT" "tests/res${test_num}.txt"
	if [ $? -eq 0 ]; then
		echo "pass"
	else
		echo "**fail**"
		result=1
	fi
}

result=0

run_test 001 "showfigfonts output" "./showfigfonts"
run_test 002 "text rendering in all fonts" \
  "for i in fonts/*.flf; do cat tests/input.txt|./figlet -f \$i; done"
run_test 003 "long text rendering" "cat tests/longtext.txt|./figlet"
run_test 004 "left-to-right text" "cat tests/input.txt|./figlet -L"
run_test 005 "right-to-left text" "cat tests/input.txt|./figlet -R"
run_test 006 "flush-left justification" "cat tests/input.txt|./figlet -l"
run_test 007 "flush-right justification" "cat tests/input.txt|./figlet -r"
run_test 008 "center justification" "cat tests/input.txt|./figlet -c"
run_test 009 "kerning mode" "cat tests/input.txt|./figlet -k"
run_test 010 "full width mode" "cat tests/input.txt|./figlet -W"
run_test 011 "overlap mode" "cat tests/input.txt|./figlet -o"
run_test 012 "TLF font rendering" "cat tests/input.txt|./figlet -f tests/emboss"

rm -f "$OUTPUT"

exit $result
