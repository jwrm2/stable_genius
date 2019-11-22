mnt_dir=$1

if [[ ! -d ${mnt_dir} ]]
then
    echo "Disk 1 partition 2: mount directory does not exist"
    exit 1
fi

mkdir ${mnt_dir}/jwrm2
echo "jwrm2" > ${mnt_dir}/jwrm2/username

mkdir ${mnt_dir}/other_user
echo "other_user" > ${mnt_dir}/other_user/username

sleep 1
