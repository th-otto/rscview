#!/bin/sh


eval "$(ssh-agent -s)"
git config --global user.email "$COMMITER_EMAIL"
git config --global user.name "$COMMITER_NAME"

git clone --branch builds "${PUBLISH_REPO}" ".travis/publish" && cd ".travis/publish"
mkdir -p "${PUBLISH_PATH}"
cp -r "${OUT}"/* "${PUBLISH_PATH}"
git add "${PUBLISH_PATH}"
git commit -m "${COMMIT_MESSAGE}"
git push
