#!/bin/sh
#

# run tests on the release if possible

if ! [ -x "$(command -v matlab)" ]; then
  echo 'matlab is not installed, not running tests using Matlab.' >&2
else
  matlab -nodesktop -r "restoredefaultpath; cd release/xfemm_linux64/mfemm; mfemm_setup('Runtests', true, 'ForceMexRecompile', true); quit"
fi

if ! [ -x "$(command -v git)" ]; then
  echo 'octave is not installed, not running tests using Octave.' >&2
else
  octave-cli --no-init-file --eval "cd release/xfemm_linux64/mfemm; mfemm_setup('Runtests', true, 'ForceMexRecompile', true); quit"
fi

