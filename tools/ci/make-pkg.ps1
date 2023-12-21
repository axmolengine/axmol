param(
  $version = $null
)
$AX_ROOT = (Resolve-Path $PSScriptRoot/../../).Path

if(!$version) {
    $axver_file = (Resolve-Path $AX_ROOT/core/axmolver.h.in).Path
    $axver_content = $(Get-Content -Path $axver_file)
    function parse_axver($part) {
        return ($axver_content | Select-String "#define AX_VERSION_$part").Line.Split(' ')[2]
    }
    function get_full_version() {
        $axver = "$(parse_axver 'MAJOR').$(parse_axver 'MINOR').$(parse_axver 'PATCH')"

        $git_prog = (Get-Command 'git' -ErrorAction SilentlyContinue).Source
        if($git_prog) {
            Write-Host "Found git: $git_prog"
            $branchName = $(git -C $AX_ROOT branch --show-current)
            if ($branchName -eq 'dev') {
                $commitHash = $(git -C $AX_ROOT rev-parse --short=7 HEAD)
                $axver += "-$commitHash"
            }
        }
        return $axver
    }
    $version = get_full_version
}

Push-Location $AX_ROOT

# collection files
$excludes = @(
    '.xs'
    '.vs'
    '.vscode'
    'build_*'
    'build'
    '.github'
    'tmp'
    'temp'
    'tools'
    '*.zip'
    'out'
)

$axmol_files = Get-ChildItem $AX_ROOT -Exclude $excludes

# $tools_files = Get-ChildItem $(Join-Path $AX_ROOT 'tools') -Exclude 'external'
# $axmol_files += $tools_files

# $tools_external_files = Get-ChildItem $(Join-Path $AX_ROOT 'tools/external') -Exclude '*.zip'
# $axmol_files += $tools_external_files

$pkg_file_name = "axmol-$version.zip"
$pkg_file_path = $(Join-Path $AX_ROOT $pkg_file_name)

$compress = @{
    Path = $axmol_files
    CompressionLevel = "Optimal"
    DestinationPath = $pkg_file_path
}

Write-Host "Creating package $pkg_file_path ..."
Compress-Archive @compress -PassThru

Pop-Location

if($env:GITHUB_ACTIONS -eq 'true') {
    echo "release_tag=v$version" >> ${env:GITHUB_OUTPUT}
    echo "release_pkg=$pkg_file_name" >> ${env:GITHUB_OUTPUT}
}
