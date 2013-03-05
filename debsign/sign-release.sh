#!/bin/sh

gpg --batch --default-key "USC Interaction Lab" --detach-sign -o Release.gpg.tmp "$1"
mv Release.gpg.tmp Release.gpg

#rsync -azrptL --progress -e "ssh" /home/ekaszubski/archive/unstable ekaszubski@asimov:interaction-ppa/ubuntu/
