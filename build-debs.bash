#!/bin/bash

safeSwitchBranch()
{
    branch=$1
    current_branch=`git branch | grep "\*" | sed 's:\* ::1'`

    if [ "$branch" != "$current_branch" ]; then
        echo "
>> Target branch is [ $branch ] but current branch is [ $current_branch ]; stashing any uncommitted changes and switching."
        git autostash
    fi

    git checkout $branch

    stash_id=`git stash list | grep "Pre-build cleanup" | grep "$branch" --max-count 1 | grep -o "\{[0-9]*\}"`

    if [ "$stash_id" != "" ]; then
        echo "
>> Branch [ $branch ] has stashed changes; grabbing them now."
        git stash pop stash@$stash_id
    fi
}

buildAndPush()
{
    buildpath=`pwd`/$1
    echo -e "\e[1;34m>> Building debian package located at [ $buildpath ]\e[0m"
    interaction-make-deb $buildpath
    bash ~/cron/update-remote-ppa.bash
}

echo "`date` starting $0"
echo "----------"

if ( ! sudo apt-get update ); then
    echo -e "\e[31mUnable to update package lists; aborting build.\e[0m"
    exit 0
fi

cd ~/workspace/usc-interaction-software

buildAndPush scripts

buildAndPush probabilistics

cd ~/workspace/usc-interaction-software.ros

#for branch in electric_unstable diamondback_unstable; do
for branch in electric_unstable; do
    git scheckout $branch

    echo -e "
\e[1;34m>> Building packages for branch [ $branch ]\e[0m"

    buildAndPush quickdev

    buildAndPush humanoid

    buildAndPush humanoid-sensing

    buildAndPush sbl

    buildAndPush test-proprietary-stack
done

cd ~/workspace/seabee3-ros-pkg

#for branch in electric_unstable diamondback_unstable; do
for branch in electric_unstable; do
   git scheckout $branch

    echo -e "
\e[1;34m>> Building packages for branch [ $branch ]\e[0m"

    buildAndPush .
done

cd ~/workspace/usc-interaction-software.ros

git scheckout electric_unstable

buildAndPush p2os

bash ~/cron/update-remote-ppa.bash

echo "----------"
echo "`date` finished $0"
