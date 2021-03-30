#!/bin/bash
set -e

BUILD_FILE=u-boot.bin
BUILD_MODE=uboot

show_help()
{
    printf "\n add_hash.sh - add git log hash value into uboot,boot0,sboot,fes \n"
    echo " eg :"
    echo " ./add_hash.sh -f input_file -m file_flag "
    echo " file_flag = uboot or boot0 or sboot"
    printf "\n\n"
}


build_uboot()
{
    dd if=./${BUILD_FILE} of=./uboot_back bs=1280 count=1 status=noxfer 2> /dev/null
    dd if=./cur.log  of=./uboot_back ibs=64 conv=notrunc,sync oflag=append obs=64 count=1 status=noxfer 2> /dev/null
    dd if=./${BUILD_FILE} of=./uboot_back ibs=1344 obs=1344 conv=notrunc oflag=append skip=1 status=noxfer 2> /dev/null
    mv uboot_back ${BUILD_FILE}
}

build_boot0()
{
    dd if=./${BUILD_FILE} of=./boot0_back bs=760 count=1 status=noxfer 2> /dev/null
    dd if=./cur.log  of=./boot0_back ibs=64 conv=notrunc,sync oflag=append obs=64 count=1 status=noxfer 2> /dev/null
    dd if=./${BUILD_FILE} of=./boot0_back ibs=824 obs=824 conv=notrunc oflag=append skip=1 status=noxfer 2> /dev/null
    mv boot0_back ${BUILD_FILE}
}


build_sboot()
{
    dd if=./${BUILD_FILE} of=./sboot_back bs=48 count=1 status=noxfer 2> /dev/null
    dd if=./cur.log  of=./sboot_back ibs=64 conv=notrunc,sync oflag=append obs=64 count=1 status=noxfer 2> /dev/null
    dd if=./${BUILD_FILE} of=./sboot_back ibs=112 obs=112 conv=notrunc oflag=append skip=1 status=noxfer 2> /dev/null
    mv sboot_back ${BUILD_FILE}
}

do_common()
{
	if [ -s ./cur.log ] ; then
		if [ "x${BUILD_MODE}" = "xuboot" ] ; then
			build_uboot
		elif [ "x${BUILD_MODE}" = "xboot0" ] ; then
			build_boot0
		elif [ "x${BUILD_MODE}" = "xsboot" ] ; then
			build_sboot
		else
			echo "build none"
		fi
	else
			echo "not set commit log"
	fi
}

while getopts f:m: OPTION
do
    case $OPTION in
        f)
            BUILD_FILE=$OPTARG
            ;;
        m)
            BUILD_MODE=$OPTARG
            ;;
        *)
            show_help
            exit
            ;;
    esac
done

do_common
