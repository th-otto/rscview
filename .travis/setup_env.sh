#!/bin/sh
# Use as: ". setup_env.sh"

export GITHUB_USER=$(echo "${TRAVIS_REPO_SLUG}" | cut -d '/' -f 1)
export BASE_RAW_URL="https://raw.githubusercontent.com/${GITHUB_USER}"
export PROJECT=$(echo "${TRAVIS_REPO_SLUG}" | cut -d '/' -f 2)
export SHORT_ID=$(git log -n1 --format="%h")

if [ "${TRAVIS_PULL_REQUEST}" = "false" ]
then
    export PUBLISH_REPO="git@github.com:${GITHUB_USER}/${PROJECT}"
    export PUBLISH_PATH="builds/${PROJECT}/${TRAVIS_BRANCH}"
    export COMMIT_MESSAGE="[${PROJECT}] [${TRAVIS_BRANCH}] Commit: https://github.com/${GITHUB_USER}/${PROJECT}/commit/${TRAVIS_COMMIT}"
else
    export PUBLISH_REPO="git@github.com:${GITHUB_USER}/${PROJECT}"
    export PUBLISH_PATH="builds/${PROJECT}/${TRAVIS_PULL_REQUEST}"
    export COMMIT_MESSAGE="[${TRAVIS_REPO_SLUG}] Download: https://github.com/${GITHUB_USER}/${PROJECT}/tree/master/${PUBLISH_PATH} ${TRAVIS_REPO_SLUG}#${TRAVIS_PULL_REQUEST}"
fi
