#!/bin/bash

# Pass in name and status
function die { echo $1: status $2 ;  exit $2; }

CONFIG=${LOCAL_TEST_DIR}/testBitsPass.cfg

(cmsRun $CONFIG ) || die 'Failure using test_cpc.cfg' $?


