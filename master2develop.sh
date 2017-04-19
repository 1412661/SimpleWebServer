#!/bin/bash

git checkout develop
git pull origin develop
git merge master
git push origin develop
