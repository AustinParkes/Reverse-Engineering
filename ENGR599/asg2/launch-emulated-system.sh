#!/bin/bash

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 rootfilesystem"
	exit
fi

ROOTFS=$1
KERNEL=${KERNEL:-linux-5.10.4.zImage}

TAPDEV=tap0
LOCALADDR=169.254.15.1/24
ROUTEADDR=169.254.15.0/24
QEMU=qemu-system-arm

if [ "$EUID" -ne 0 ]
then
	echo "Must be executed as root"
	exit
fi

if ip link show $TAPDEV > /dev/null 2>&1
then
	echo "TAP device already exists; skipping initialization"
else
	modprobe tun tap
	ip tuntap add dev $TAPDEV mode tap
	ip addr add $LOCALADDR dev $TAPDEV
	ip link set up dev $TAPDEV
	ip route add $ROUTEADDR dev $TAPDEV
fi

$QEMU -M virt,highmem=off -cpu cortex-a15 -m 128 \
	-kernel $KERNEL \
	-drive file=$ROOTFS,if=virtio,format=raw \
	-append "console=ttyAMA0,115200 root=/dev/vda" \
	-net nic,model=virtio-net-pci,netdev=net0 \
	-netdev tap,id=net0,ifname=$TAPDEV,script=no \
	-chardev stdio,id=char0,signal=off \
	-serial chardev:char0 \
	-s
