VER=$1

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
AX_ROOT="$DIR"/../..

cd $AX_ROOT

mkdir -p ./tmp

if [ ! -d "./tmp/buildware_dist_$VER" ]; then
    wget -O ./tmp/buildware_dist_$VER.zip https://github.com/axmolengine/buildware/releases/download/$VER/buildware_dist_$VER.zip
    unzip ./tmp/buildware_dist_$VER.zip -d ./tmp/
fi

function update_lib()
{
    lib_name=$1
    lib_folder=$2
    lib_dir=./thirdparty/$lib_folder$lib_name
    prebuilt_dir=$lib_dir/prebuilt
    inc_dir=$lib_dir/include
    
    echo "Updating lib ${lib_dir} from ./tmp/buildware_dist_$VER/$lib_name ..."
    
    #rm -rf $prebuilt_dir
    #cp -r ./tmp/buildware_dist_$VER/$lib_name/prebuilt $lib_dir/
    
	if [ -d $inc_dir ]; then
        echo "Update inc files for ${lib_name}"
		rm -rf $inc_dir
        cp -r ./tmp/buildware_dist_$VER/$lib_name/include $lib_dir/
	fi
}

update_lib angle
update_lib curl
update_lib "glsl-optimizer"
update_lib "jpeg-turbo"
update_lib openssl
update_lib zlib
update_lib luajit lua/
