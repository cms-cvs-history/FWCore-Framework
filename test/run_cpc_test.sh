#!/bin/bash

# Pass in name and status
function die { echo $1: status $2 ;  exit $2; }

(cmsRun test_cpc.cfg ) || die 'Failure using test_cpc.cfg' $?


