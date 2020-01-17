mnt_dir=$1
user_dir=$2

if [[ ! -d ${mnt_dir} ]]
then
    echo "Disk 1 partition 1: mount directory does not exist"
    exit 1
fi

disk_dir=${PWD}

(cd ${user_dir} && find . -type f -exec install -D "{}" "${disk_dir}/${mnt_dir}/{}" \;)

sleep 1
