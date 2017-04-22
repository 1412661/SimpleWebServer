#!/bin/bash
# 
# Run this script to test the program
#

wget http://$1:$2
wget http://$1:$2/result.html
wget http://$1:$2/result.html?country=Vietnam
wget http://localhost:$2/result.html?country=Nguy%26%237877%3Bn+Qu%26%237889%3Bc+B%26%237843%3Bo
wget http://localhost:$2/result.html?country=Viet+Nam+
wget http://localhost:$2/result.html
wget http://localhost:$2/result.html?
wget http://localhost:$2?country=Vietnam
wget http://localhost:$2/abc.img
wget http://localhost:$2/index.html
