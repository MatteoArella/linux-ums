#!/usr/bin/env bash

set -e

BASEDIR=$(dirname $(dirname $(dirname $(realpath "$0"))))
ME=$(basename "$0")

usage() {
  cat <<EOF
Usage: $ME [OPTIONS]... {start|push|shell|kill}

Options:
    -h                    show this help and exit
EOF
}

while getopts ":h" opt; do
  case "$opt" in
  h)
    usage
    exit 0
    ;;
  \?)
    echo "unknown option $OPTARG!" >&2
    usage >&2
    exit 1
    ;;
  :)
    echo "missing value for option [-$OPTARG]" >&2
    usage >&2
    exit 1
    ;;
  esac
done
shift $(($OPTIND - 1))

QEMU_MONITOR_IP=127.0.0.1
QEMU_MONITOR_PORT=5555
SSH_USER=root
SSH_IP=127.0.0.1
SSH_PORT=2222
SSH_OPTS="-oStrictHostKeyChecking=no -oUserKnownHostsFile=/dev/null -o LogLevel=ERROR"
NPROC=1

start_qemu() {
  # start qemu system
  SDK_BASE_PATH="${1}/images"
  SDK_NAME=x86_64-buildroot-linux-gnu_sdk-buildroot

  if [ "$CI" = "true" ] || [ ! -z "$CI" ]; then
    # provide more entropy at boot time
    EXTRA_FLAGS="-object rng-random,id=rng0,filename=/dev/urandom -device virtio-rng-pci,rng=rng0"
  else
    EXTRA_FLAGS="-enable-kvm -device virtio-rng-pci"
  fi

  cd $SDK_BASE_PATH

  ($SDK_NAME/bin/qemu-system-x86_64 \
    -M pc \
    -m 1024M \
    -smp $NPROC \
    -kernel bzImage \
    -drive file=rootfs.ext2,if=virtio,format=raw \
    -append "rootwait root=/dev/vda console=ttyS0" \
    -qmp tcp:$QEMU_MONITOR_IP:$QEMU_MONITOR_PORT,server,nowait \
    -net nic,model=virtio \
    -net user,hostfwd=tcp:$SSH_IP:$SSH_PORT-:22 \
    -nographic \
    $EXTRA_FLAGS 1>/dev/null 2>/dev/null) &

  sleep 10
  while [ true ]; do
    echo "waiting device to become available..."
    ssh -q -p $SSH_PORT $SSH_OPTS -o ConnectTimeout=5 $SSH_USER@$SSH_IP exit && break ||
      sleep 20
  done
}

kill_qemu() {
  nc $QEMU_MONITOR_IP $QEMU_MONITOR_PORT <<EOF
{ "execute": "qmp_capabilities" }
{ "execute": "quit" }
EOF
}

push() {
  SRCFILE=$1
  DESTDIR=$2
  scp -P $SSH_PORT $SSH_OPTS $SRCFILE $SSH_USER@$SSH_IP:$DESTDIR
}

shell() {
  ssh -p $SSH_PORT $SSH_OPTS $SSH_USER@$SSH_IP $1
}

case "$1" in
start)
  start_qemu "$2"
  ;;
push)
  push "$2" "$3"
  ;;
shell)
  shell "$2"
  ;;
kill)
  kill_qemu 1>/dev/null
  ;;
*)
  usage >&2
  exit 1
  ;;
esac
