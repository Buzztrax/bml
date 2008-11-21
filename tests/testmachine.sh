#!/bin/bash
# $Id$
# iterates over the given directory and tries all buzz machines
#
# if elements hang, they can be skipped by pressing ctrl-c
#
# ./testmachine.sh "machines/*.dll"
# ./testmachine.sh "/home/ensonic/buzztard/lib/Gear-real/Effects/*.dll"
# ./testmachine.sh "/home/ensonic/buzztard/lib/Gear-real/Generators/*.dll"
# ./testmachine.sh "/home/ensonic/buzztard/lib/Gear-real/*/*.dll"
# ./testmachine.sh "/home/ensonic/buzztard/lib/Gear/*.so"
#
# analyze results
#
# search for unk_XXX -> unknown symbols
#  grep -Hn "unk_" testmachine/*.fail | sort | uniq
#  grep -ho "unk_.*" testmachine/*.fail | sort | uniq -c
# search for FIXME -> unimplemented buzz callback entries
#  grep -Hn "FIXME" testmachine/*.fail
# search for "wine/module: Win32 LoadLibrary failed to load:"
#  grep -ho "wine/module: Win32 LoadLibrary failed to load:.*" testmachine/*.fail | sort -f | uniq -ic
# stats
#  ls -1 testmachine/*.okay | wc -l
#  ls -1 testmachine/*.fail | wc -l
#
# TODO:
# also run bmltest_process
# - check for NAN buffers
# - check for insanely lound buffers
# allow running under valgrind
#

. ./bt-cfg.sh

if [ -z "$1" ] ; then
  echo "Usage: $0 <directory>";
  exit
fi

# initialize

machine_glob="$1";
mkdir -p testmachine
m_okay=0;
m_fail=0;
trap "sig_segv=1" SIGSEGV
trap "sig_int=1" INT
touch testmachine.failtmp
rm -f testmachine.body.html
touch testmachine.body.html

# run test loop

for machine in $machine_glob ; do
  #name=`basename "$machine" "$machine_glob"`
  name=`basename "$machine"`
  ext=${name#${name%.*}}
  log_name="./testmachine/$name.txt"
  rm -f "$log_name" "$log_name".okay "$log_name".fail
  # collect used dlls
  fieldLibs=`strings "$machine" | grep -i -F "$ext" | grep -vi "$name" | sort`
  # try to run it
  sig_segv=0
  sig_int=0
  #env >testmachine.log 2>&1 LD_LIBRARY_PATH="../src/" ../src/bmltest_info "$machine"
  #res=$?
  # this suppresses the output of e.g. "Sementation fault"
  res=`env >testmachine.log 2>&1 BML_DEBUG=1 LD_LIBRARY_PATH="../src/:../src/BuzzMachineLoader/.libs:$LD_LIBRARY_PATH" ../src/bmltest_info "$machine"; echo $?`
  cat testmachine.log | grep >"$log_name" -v "Warning: the specified"
  if [ $sig_int -eq "1" ] ; then res=1; fi
  if [ $res -eq "0" ] ; then
    echo "okay : $machine";
    m_okay=$((m_okay+1))
    mv "$log_name" "$log_name".okay
    tablecolor="#E0FFE0"
    tableresult="okay"
  else
    echo "fail : $machine";
    m_fail=$((m_fail+1))
    mv "$log_name" "$log_name".fail
    tablecolor="#FFE0E0"
    tableresult="fail"
    reason=`tail -n1 "$log_name".fail | strings`;
    echo "$reason :: $name" >>testmachine.failtmp
  fi
  cat testmachine.log | iconv >testmachine.tmp -fWINDOWS-1250 -tUTF-8 -c
  fieldShortName=`egrep -o "Short Name: .*$" testmachine.tmp | sed -e 's/Short Name: "\(.*\)"$/\1/'`
  fieldAuthor=`egrep -o "Author: .*$" testmachine.tmp | sed -e 's/Author: "\(.*\)"$/\1/'`
  fieldType=`egrep -o "^    Type: . -> \"MT_.*$" testmachine.tmp | sed -e 's/^\ *Type: . -> "\(.*\)"$/\1/'`
  fieldVersion=`egrep -o "Version: .*$" testmachine.tmp | sed -e 's/Version: \(.*\)$/\1/'`
  fieldFlags=`egrep -o "^    Flags: .*$" testmachine.tmp | sed -e 's/^\ *Flags: \(.*\)$/\1/'`
  fieldMinTracks=`egrep -o "MinTracks: .*$" testmachine.tmp | sed -e 's/MinTracks: \(.*\)$/\1/'`
  fieldMaxTracks=`egrep -o "MaxTracks: .*$" testmachine.tmp | sed -e 's/MaxTracks: \(.*\)$/\1/'`
  fieldInputChannels=`egrep -o "InputChannels: .*$" testmachine.tmp | sed -e 's/InputChannels: \(.*\)$/\1/'`
  fieldOutputChannels=`egrep -o "OutputChannels: .*$" testmachine.tmp | sed -e 's/OutputChannels: \(.*\)$/\1/'`
  fieldNumGlobalParams=`egrep -o "NumGlobalParams: .*$" testmachine.tmp | sed -e 's/NumGlobalParams: \(.*\)$/\1/'`
  fieldNumTrackParams=`egrep -o "NumTrackParams: .*$" testmachine.tmp | sed -e 's/NumTrackParams: \(.*\)$/\1/'`
  fieldNumAttributes=`egrep -o "NumAttributes: .*$" testmachine.tmp | sed -e 's/NumAttributes: \(.*\)$/\1/'`
  cat >>testmachine.body.html <<END_OF_HTML
      <tr bgcolor="$tablecolor">
        <td><a href="$log_name.$tableresult">$tableresult</a></td>
        <td>$name</td>
        <td>$fieldShortName</td>
        <td>$fieldAuthor</td>
        <td>$fieldType</td>
        <td>$fieldVersion</td>
        <td>$fieldFlags</td>
        <td>$fieldMinTracks</td>
        <td>$fieldMaxTracks</td>
        <td>$fieldInputChannels</td>
        <td>$fieldOutputChannels</td>
        <td>$fieldNumGlobalParams</td>
        <td>$fieldNumTrackParams</td>
        <td>$fieldNumAttributes</td>
        <td>$fieldLibs</td>
      </tr>
END_OF_HTML
done

# cleanup and report

rm testmachine.log testmachine.tmp
sort testmachine.failtmp >testmachine.fails
rm testmachine.failtmp

cat >testmachine.html <<END_OF_HTML
<html>
  <head>
    <script src="sorttable.js"></script>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
  </head>
  <body>
    <table class="sortable" border="1" cellspacing="0">
      <tr>
        <th>Res.</th>
        <th>Plugin Lib.</th>
        <th>Name</th>
        <th>Author</th>
        <th>Type</th>
        <th>API Ver.</th>
        <th>Flags</th>
        <th>Min Trk.</th>
        <th>Max Trk.</th>
        <th>Input Ch.</th>
        <th>Output Ch.</th>
        <th>Global Par.</th>
        <th>Track Par.</th>
        <th>Attr.</th>
        <th>Libs</th>
      </tr>
END_OF_HTML
cat >>testmachine.html testmachine.body.html
cat >>testmachine.html <<END_OF_HTML
    </table>
  </body>
</html>
END_OF_HTML
rm testmachine.body.html

m_all=$((m_fail+m_okay))
echo "Of $m_all machine(s) $m_okay machine(s) worked and $m_fail machine(s) failed."
echo "See testmachine.fails and testmachine.html for details"

