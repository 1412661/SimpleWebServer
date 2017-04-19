#!/bin/bash

git remote add upstream https://github.com/1412661/SimpleWebServer.git
git fetch upstream
git checkout master
git merge upstream/master
git pull
git push
