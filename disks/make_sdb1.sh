mnt_dir=$1

if [[ ! -d ${mnt_dir} ]]
then
    echo "Disk 2 partition 1: mount directory does not exist"
    exit 1
fi

mkdir ${mnt_dir}/bin
mkdir ${mnt_dir}/lib

sleep 1
