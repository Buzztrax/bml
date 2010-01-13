#!/bin/bash
# $Id$
# iterates over the given directory and tries all buzz machines
#
# if machines hang, they can be skipped by pressing ctrl-c
#
# ./testmachine.sh "machines/*.dll"
# ./testmachine.sh "/home/ensonic/buzztard/lib/Gear-real/Effects/*.dll"
# ./testmachine.sh "/home/ensonic/buzztard/lib/Gear-real/Generators/*.dll"
# ./testmachine.sh "/home/ensonic/buzztard/lib/Gear-real/{G,E}*/*.dll"
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
# - allow running under valgrind
# - if a machine is m2s (fieldFlags), then channels=2 
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
m_info=0;
m_fail=0;
trap "sig_segv=1" SIGSEGV
trap "sig_int=1" INT
touch testmachine.failtmp
rm -f testmachine.body.html
touch testmachine.body.html

if [ ! -e input.raw ]; then
  dd count=10 if=/dev/zero of=input.raw
fi

# run test loop

for machine in $machine_glob ; do
  #name=`basename "$machine" "$machine_glob"`
  name=`basename "$machine"`
  ext=${name#${name%.*}}
  log_name="./testmachine/$name.txt"
  rm -f "$log_name" "$log_name".okay "$log_name".fail "$log_name".info
  # collect used dlls
  fieldLibs=`strings "$machine" | grep -i -F "$ext" | grep -vi "$name" | sort | uniq`
  # try to run it
  sig_segv=0
  sig_int=0
  #env >testmachine.log 2>&1 LD_LIBRARY_PATH="../src/" ../src/bmltest_info "$machine"
  #res=$?
  # this suppresses the output of e.g. "Sementation fault"
  res=`env >bmltest_info.log 2>&1 BML_DEBUG=255 LD_LIBRARY_PATH="../src/:../src/BuzzMachineLoader/.libs:$LD_LIBRARY_PATH" ../src/bmltest_info "$machine"; echo $?`
  cat bmltest_info.log | grep >"$log_name" -v "Warning: the specified"
  if [ $sig_int -eq "1" ] ; then res=1; fi
  cat bmltest_info.log | iconv >bmltest_info.tmp -fWINDOWS-1250 -tUTF-8 -c
  fieldCreateTime=`egrep -o "machine created in .*$" bmltest_info.tmp | sed -e 's/machine created in \(.*\) sec$/\1/'`
  fieldInitTime=`egrep -o "machine initialized in .*$" bmltest_info.tmp | sed -e 's/machine initialized in \(.*\) sec$/\1/'`
  fieldShortName=`egrep -o "Short Name: .*$" bmltest_info.tmp | sed -e 's/Short Name: "\(.*\)"$/\1/'`
  fieldAuthor=`egrep -o "Author: .*$" bmltest_info.tmp | sed -e 's/Author: "\(.*\)"$/\1/'`
  fieldType=`egrep -o "^    Type: . -> \"MT_.*$" bmltest_info.tmp | sed -e 's/^\ *Type: . -> "\(.*\)"$/\1/'`
  fieldVersion=`egrep -o "Version: .*$" bmltest_info.tmp | sed -e 's/Version: \(.*\)$/\1/'`
  fieldFlags=`egrep -o "^    Flags: .*$" bmltest_info.tmp | sed -e 's/^\ *Flags: \(.*\)$/\1/'`
  fieldMinTracks=`egrep -o "MinTracks: .*$" bmltest_info.tmp | sed -e 's/MinTracks: \(.*\)$/\1/'`
  fieldMaxTracks=`egrep -o "MaxTracks: .*$" bmltest_info.tmp | sed -e 's/MaxTracks: \(.*\)$/\1/'`
  fieldInputChannels=`egrep -o "InputChannels: .*$" bmltest_info.tmp | sed -e 's/InputChannels: \(.*\)$/\1/'`
  fieldOutputChannels=`egrep -o "OutputChannels: .*$" bmltest_info.tmp | sed -e 's/OutputChannels: \(.*\)$/\1/'`
  fieldNumGlobalParams=`egrep -o "NumGlobalParams: .*$" bmltest_info.tmp | sed -e 's/NumGlobalParams: \(.*\)$/\1/'`
  fieldNumTrackParams=`egrep -o "NumTrackParams: .*$" bmltest_info.tmp | sed -e 's/NumTrackParams: \(.*\)$/\1/'`
  fieldNumAttributes=`egrep -o "NumAttributes: .*$" bmltest_info.tmp | sed -e 's/NumAttributes: \(.*\)$/\1/'`
  if [ $res -eq "0" ] ; then
    # try to run it again
    sig_segv=0
    sig_int=0
    # this suppresses the output of e.g. "Sementation fault"
    res=`env >bmltest_process.log 2>&1 BML_DEBUG=255 LD_LIBRARY_PATH="../src/:../src/BuzzMachineLoader/.libs:$LD_LIBRARY_PATH" ../src/bmltest_process "$machine" input.raw output.raw; echo $?`
    cat bmltest_process.log | grep >>"$log_name" -v "Warning: the specified"
    if [ $sig_int -eq "1" ] ; then res=1; fi
    if [ $res -eq "0" ] ; then
      echo "okay : $machine";
      m_okay=$((m_okay+1))
      mv "$log_name" "$log_name".okay
      gst-launch-0.10 >/dev/null 2>&1 filesrc location=output.raw ! audio/x-raw-int,width=16,channels=$fieldOutputChannels,rate=44100 ! wavenc ! filesink location="testmachine/$name.wav"
      tablecolor="#E0FFE0"
      tableresult="okay"
    else
      echo "info : $machine";
      m_info=$((m_info+1))
      mv "$log_name" "$log_name".info
      tablecolor="#FFF7E0"
      tableresult="info"
    fi
  else
    echo "fail : $machine";
    m_fail=$((m_fail+1))
    mv "$log_name" "$log_name".fail
    tablecolor="#FFE0E0"
    tableresult="fail"
    reason=`tail -n1 "$log_name".fail | strings`;
    echo "$reason :: $name" >>testmachine.failtmp
    touch bmltest_process.log
  fi
  cat bmltest_process.log | iconv >bmltest_process.tmp -fWINDOWS-1250 -tUTF-8 -c
  fieldMaxAmp=`egrep -o "MaxAmp: .*$" bmltest_process.tmp | sed -e 's/MaxAmp: \(.*\)$/\1/'`
  fieldClipped=`egrep -o "Clipped: .*$" bmltest_process.tmp | sed -e 's/Clipped: \(.*\)$/\1/'`
  fieldMathNaN=`egrep -o "some values are nan" bmltest_process.tmp | sed -e 's/some values are \(.*\)$/\1/'`
  fieldMathInf=`egrep -o "some values are inf" bmltest_process.tmp | sed -e 's/some values are \(.*\)$/\1/'`
  fieldMathDen=`egrep -o "some values are denormal" bmltest_process.tmp | sed -e 's/some values are \(.*\)$/\1/'`
  
  cat >>testmachine.body.html <<END_OF_HTML
      <tr bgcolor="$tablecolor">
        <td><a href="$log_name.$tableresult">$tableresult</a></td>
        <td>$fieldCreateTime</td>
        <td>$fieldInitTime</td>
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
        <td>$fieldMaxAmp</td>
        <td>$fieldClipped</td>
        <td>$fieldMathNaN $fieldMathInf $fieldMathDen</td>
      </tr>
END_OF_HTML
  rm -f bmltest_info.log bmltest_process.log
done

# cleanup and report

rm -f bmltest_info.log bmltest_info.tmp
rm -f bmltest_process.log bmltest_process.tmp
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
        <th>Create sec.</th>
        <th>Init sec.</th>
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
        <th>Max Amp.</th>
        <th>Clipped</th>
        <th>NaN/Inf/Den</th>
      </tr>
END_OF_HTML
cat >>testmachine.html testmachine.body.html
cat >>testmachine.html <<END_OF_HTML
    </table>
  </body>
</html>
END_OF_HTML
rm testmachine.body.html

m_all=$((m_fail+m_info+m_okay))
echo "Of $m_all machine(s) $m_okay worked, $m_info did not processed data and $m_fail failed to load."
echo "See testmachine.fails and testmachine.html for details"

