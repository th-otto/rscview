#!/bin/sh
# Use as: ". setup_env.sh"

GITHUB_USER=$(echo "${GITHUB_REPOSITORY}" | cut -d '/' -f 1)
echo "GITHUB_USER=${GITHUB_USER}" >> $GITHUB_ENV
TMP="${GITHUB_WORKSPACE}/.travis/tmp"
echo "TMP=$TMP" >> $GITHUB_ENV
OUT="${GITHUB_WORKSPACE}/.travis/out"
echo "OUT=$OUT" >> $GITHUB_ENV

BASE_RAW_URL="https://raw.githubusercontent.com/${GITHUB_USER}"

PROJECT=$(echo "${GITHUB_REPOSITORY}" | cut -d '/' -f 2)
echo "PROJECT=$PROJECT" >> $GITHUB_ENV
SHORT_ID=$(echo ${GITHUB_SHA} | cut -c 1-7)
echo "SHORT_ID=$SHORT_ID" >> $GITHUB_ENV
BRANCH=$(echo "${GITHUB_REF}" | cut -d '/' -f 3)

# GITHUB_HEAD_REF is only set for pull requests
if [ "${GITHUB_HEAD_REF}" = "" ]
then
    PUBLISH_REPO="git@github.com:${GITHUB_USER}/${PROJECT}"
    PUBLISH_PATH="builds/${PROJECT}/${BRANCH}"
    COMMIT_MESSAGE="[${PROJECT}] [${BRANCH}] Commit: https://github.com/${GITHUB_USER}/${PROJECT}/commit/${GITHUB_SHA}"
else
    PUBLISH_REPO="git@github.com:${GITHUB_USER}/${PROJECT}"
    PUBLISH_PATH="builds/${PROJECT}/${GITHUB_HEAD_REF}"
    COMMIT_MESSAGE="[${GITHUB_REPOSITORY}] Download: https://github.com/${GITHUB_USER}/${PROJECT}/tree/master/${PUBLISH_PATH} ${GITHUB_REPOSITORY}#${TRAVIS_PULL_REQUEST}"
fi

echo "PUBLISH_REPO=$PUBLISH_REPO" >> $GITHUB_ENV
echo "PUBLISH_PATH=$PUBLISH_PATH" >> $GITHUB_ENV
echo "COMMIT_MESSAGE=$COMMIT_MESSAGE" >> $GITHUB_ENV
