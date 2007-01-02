#!/bin/bash

test=testSelectEvents

function die { echo Failure $1: status $2 ; exit $2 ; }

pushd ${LOCAL_TMP_DIR}

  cmsRun -p ${LOCAL_TEST_DIR}/${test}HLT.cfg || die "cmsRun ${test}HLT.cfg" $?

  cmsRun -p ${LOCAL_TEST_DIR}/${test}PROD.cfg || die "cmsRun ${test}PROD.cfg" $?

  cmsRun -p ${LOCAL_TEST_DIR}/${test}TEST.cfg || die "cmsRun ${test}TEST.cfg" $?

popd

exit 0
