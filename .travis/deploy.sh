#!/bin/sh


OUT="${PWD}/.travis/out"

eval "$(ssh-agent -s)"

git clone --branch builds "${PUBLISH_REPO}" ".travis/publish" && cd ".travis/publish"
mkdir -p "${PUBLISH_PATH}"
cp -r "${OUT}"/* "${PUBLISH_PATH}"
git add "${PUBLISH_PATH}" && git commit -m "${COMMIT_MESSAGE}" && (while true; do git push; [ $? -ne 0 ] || break; git pull --rebase; done)
