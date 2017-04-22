#!/bin/bash
#
# Run this script to test the program
#

wget http://$1:$2 --delete-after
wget http://$1:$2/result.html --delete-after
wget http://$1:$2/result.html?country=Vietnam --delete-after
wget http://$1:$2/result.html?country=Nguy%26%237877%3Bn+Qu%26%237889%3Bc+B%26%237843%3Bo --delete-after
wget http://$1:$2/result.html?country=Viet+Nam+ --delete-after
wget http://$1:$2/result.html --delete-after
wget http://$1:$2/result.html? --delete-after
wget http://$1:$2?country=Vietnam --delete-after
wget http://$1:$2/abc.img --delete-after
wget http://$1:$2/index.html --delete-after
