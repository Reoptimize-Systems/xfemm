#!/bin/bash
###
# BSD-2-Clause license:
# Copyright 2016 Johannes Zarl-Zierl <johannes@zarl-zierl.at>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
###

mydir=`dirname "$0"`
mydir=`readlink -f "$mydir"`
bindir="$mydir/bin"
checks=

checkdir=`mktemp -d -t xfemm_check_XXXXXX`
checkdir=`readlink -f "$checkdir"`

init_check()
{
	mkdir "$checkdir/$1"
	cp -ar "$mydir/$1/test" "$checkdir/$1"
	cd "$checkdir/$1"
}

checks="$checks check_fmesher"
check_fmesher()
{
	prefix="$1"
	init_check fmesher
	$bindir/fmesher test/Temp.fem > test/fmesher.out || return 1
	diff -wq test/fmesher.out.check test/fmesher.out || return 1
}

checks="$checks check_fpproc"
check_fpproc()
{
	prefix="$1"
	echo "$prefix only checking for crashes"
	init_check fpproc
	$bindir/fpproc-test > test/fpproc-test.out || return 1
}

checks="$checks check_fsolver"
check_fsolver()
{
	prefix="$1"
	init_check fsolver
	$bindir/fsolver test/Temp > test/fsolver.out || return 1
	diff -wq test/Temp.ans.check test/Temp.ans || return 1
}

checks="$checks check_hpproc"
check_hpproc()
{
	prefix="$1"
	init_check hpproc
	$bindir/hpproc-test > test/hpproc-test.out || return 1
	diff -wq test/hpproc-test.out.check test/hpproc-test.out
}

checks="$checks check_hsolver"
check_hsolver()
{
	prefix="$1"
	init_check hsolver
	for f in Temp0 #Temp1
	do
		echo "$prefix $f"
		$bindir/fmesher test/$f.feh > test/$f.fmesher.out || return 1
		$bindir/hsolver test/$f > test/$f.hsolver.out || return 1
		diff -wq test/$f.anh.check test/$f.anh || return 1
	done
}

#checks="$checks check_femmcli"
#check_femmcli()
#{
#	prefix="$1"
#	init_check femmcli
#	for f in `ls test/*.lua`
#	do
#		echo "$prefix $f"
#		$bindir/femmcli --lua-script=$f > $f.out 2>$f.err || return 1
#		diff -wq $f.out.check $f.out || return 1
#	done
#}

for arg
do
	case "$arg" in
		-h|--help)
			echo "Usage: `basename "$0"` [--keep]"
			echo "--keep        Keep temporary files even when no check fails."
			exit 0
			;;
		--keep)
			keep=1
			;;
		*)
			echo "Unknown argument!"
			echo "Try '$0 --help'..."
			exit 1
			;;
	esac
done

num=0
ok=0
fail=0
for c in $checks
do
	let num++
	echo "[$num] $c: "
	if "$c" "[$num] ->"
	then
		let ok++
		echo "[$num] OK"
	else
		let fail++
		echo "[$num] FAILED"
	fi
done

echo "$ok of $num checks ok."
if [[ ok -ne num || -n "$keep" ]]
then
	echo "You can check the results for yourself in $checkdir"
	exit $fail
else
	rm -rf "$checkdir"
fi
