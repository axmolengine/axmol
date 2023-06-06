$VER=$args[0]
echo "VER=$VER"

$AX_ROOT=(Resolve-Path "$PSScriptRoot/../..").Path
echo "AX_ROOT=$AX_ROOT"

function update_lib
{
    $lib_name=$args[0]
    $lib_folder=$args[1]
    echo "lib_name=$lib_name"
    $lib_dir="./thirdparty/$lib_folder$lib_name"
    $prebuilt_dir="$lib_dir/prebuilt"
    $inc_dir="$lib_dir/include"
    
    echo "Updating lib files for ${lib_dir} from ./tmp/buildware_dist_$VER/$lib_name ..."
    rm -rf $prebuilt_dir
    cp -r ./tmp/buildware_dist_$VER/$lib_name/prebuilt $lib_dir/
    
    if ( Test-Path "./tmp/buildware_dist_$VER/$lib_name/include" -PathType Container ) {
        echo "Update inc files for ${lib_dir}"
        rm -rf $inc_dir
        cp -r ./tmp/buildware_dist_$VER/$lib_name/include $lib_dir/
    }
}

# start updating
cd $AX_ROOT
mkdir -p ./tmp

# ensure yaml parser module
if ($null -eq (Get-Module -ListAvailable -Name powershell-yaml)) {
    Install-Module -Name powershell-yaml -Force -Repository PSGallery -Scope CurrentUser
}

# check upstream prebuilts version
if (!(Test-Path ./tmp/verlist.yml -PathType Leaf)) {
    curl -L https://github.com/axmolengine/buildware/releases/download/$VER/verlist.yml -o ./tmp/verlist.yml
}

$newVerList = ConvertFrom-Yaml -Yaml (Get-Content './tmp/verlist.yml' -raw)
$myVerList = ConvertFrom-Yaml -Yaml (Get-Content './thirdparty/prebuilts.yml' -raw)

$needUpdate = $false;
foreach ($item in $myVerList.GetEnumerator() )
{
  if ($item.Value -ne $newVerList[$item.Name]) {
    $needUpdate = $true;
    break;
  }
}


if ($needUpdate) {
    echo "Updating libs ..."

    # download buildware_dist_xxx.zip
    if ( ! (Test-Path "./tmp/buildware_dist_$VER" -PathType Container) ) {
        wget -O ./tmp/buildware_dist_$VER.zip https://github.com/axmolengine/buildware/releases/download/$VER/buildware_dist_$VER.zip
        unzip ./tmp/buildware_dist_$VER.zip -d ./tmp/
    }

    # update libs
    update_lib angle
    update_lib curl
    update_lib "glsl-optimizer"
    update_lib "jpeg-turbo"
    update_lib openssl
    update_lib zlib
    update_lib luajit lua/

    # update README.md
    $content = $(Get-Content -Path ./thirdparty/README.md.in -raw)
    foreach ($item in $newVerList.GetEnumerator() )
    {
        $key = ([Regex]::Replace($item.Name, '-', '_')).ToUpper()
        $key = "${key}_VERSION"
        $content = $content -replace "\$\{$key\}",$item.Value
    }
    Set-Content -Path ./thirdparty/README.md -Value "$content"
    Copy-Item -Path './tmp/verlist.yml' './thirdparty/prebuilts.yml' -Force
} else {
    echo "No any lib need update."
    if ("$env.RUNNER_OS" -ne "") {
        echo "AX_PREBUILTS_NO_UPDATE=true" >> $GITHUB_ENV
    }
}

