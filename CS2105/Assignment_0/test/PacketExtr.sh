#!/bin/bash

d0="$(dirname "$(readlink -f -- "$0")")"

source "$d0/common.sh"

mkdirTmp
findPy3

run() {
  local cid="$1"
  inf="$cdir/$cid.in"
  "$py3" "$d0/iserv.py" "$cdir/$cid" >(evalRun "$cid" "$evalcmd") "$tmpdir/$cid.stdout"
  rtn="$?"
  #
  state=0
  cmp "$tmpdir/$cid.stdout" "$cdir/$cid.out" \
    | awk '{if (NR == 1) print "Failed: your output differs from the reference answer."; print $0}'
  if [[ "${PIPESTATUS[0]}" == 0 ]]; then
    if [[ "$rtn" == 0 ]]; then
      echo "Passed!"
      state=1
      let pcases++
    else
      state=0.5
      echo "Info: timed out, but the final output is correct."
    fi
  fi
  if [[ -n "$GRADING_RESULT_FILE" ]]; then
    echo "score $state" >> "$GRADING_RESULT_FILE"
  fi
}

launchTest run
