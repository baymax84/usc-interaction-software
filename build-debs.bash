#!/bin/bash

buildAndPush()
{
    buildpath=`pwd`/$1
    echo -e "\e[1;34m>> Building debian package located at [ $buildpath ]\e[0m"
    interaction-make-deb $buildpath
    bash ~/cron/update-remote-ppa.bash --no-apt-update
}

readAndExecuteCommand()
{
    local args
    declare -a args

    if [ "$1" != "" ]
    then
        pushd $1
    fi

    if [ -e buildinfo ]
    then
        if [ -e debian ]
        then
            buildAndPush .
        else
            for arg in `cat buildinfo`
            do
                args=(${args[@]} $arg)
            done

            while [ ${#args[@]} -gt 0 ]
            do
                if [ "${args[0]}" == "-s" ] && [ "${args[2]}" == "-t" ]
                then
                    dir=${args[1]}
                    dir_type=${args[3]}
                    args=(${args[@]:4})

                    echo "Running mode $dir_type on target $dir"

                    case $dir_type in
                        dir )       pushd $dir
                                    readAndExecuteCommand
                                    popd
                                    ;;
                        build )     buildAndPush $dir
                                    ;;
                        git )       pushd $dir
                                    while [ "${args[0]}" == "-b" ]
                                    do
                                        git scheckout ${args[1]}
                                        readAndExecuteCommand
                                        args=(${args[@]:2})
                                    done
                                    popd
                                    ;;
                        * )         echo "Uknown type $command_type"
                                    ;;
                    esac
                else
                    echo "Invalid args format: ${args[@]}"
                    args=(${args[@]:1})
                fi
            done
            
        fi
    else
        echo "No buildinfo found in `pwd`; ignoring"
    fi
    
    if [ "$1" != "" ]
    then
        popd
    fi
}

readAndExecuteCommand ~/build_space
