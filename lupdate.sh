#!/bin/bash
if [ ! -d "translations/" ];then
  mkdir translations
fi
cd ./translations
rm -f deepin-deepinid-client.ts
lupdate ../ -ts -no-ui-lines -locations none -no-obsolete deepin-deepinid-client.ts
cd ../
