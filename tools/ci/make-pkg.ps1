param(
  $version = $null
)

Write-Host "Creating package $pkg_file_path ..."

$AX_ROOT = (Resolve-Path $PSScriptRoot/../..).Path

if(!$version -or ($version -eq 'auto')) {
    $axver_file = (Resolve-Path $AX_ROOT/core/axmolver.h.in).Path
    $axver_content = $(Get-Content -Path $axver_file)
    function parse_axver($part) {
        return ($axver_content | Select-String "#define AX_VERSION_$part").Line.Split(' ')[2]
    }
    function get_full_version() {
        $axver = "$(parse_axver 'MAJOR').$(parse_axver 'MINOR').$(parse_axver 'PATCH')"

        $git_prog = (Get-Command 'git' -ErrorAction SilentlyContinue).Source
        if($git_prog) {
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
    'core/axmolver.h'
    'core/renderer/RenderConsts.h'
    '.github'
    'tmp'
    'temp'
    'tools/external'
    'axmol-*.zip'
    'out'
    '*/_d/*'
)

$pkg_file_name = "axmol-$version.zip"
$pkg_file_path = $(Join-Path $AX_ROOT $pkg_file_name)

$compress_args = @{
    Path = $AX_ROOT
    CompressionLevel = 'Optimal'
    DestinationPath = $pkg_file_path
    RelativeBasePath = $AX_ROOT
    Exclude = $excludes
    Prefix = "axmol-$version"
}

# Compress-Archive @compress -PassThru

function Compress-ArchiveEx() {
    param(
      $Path,
      $CompressionLevel = 'Optimal',
      $DestinationPath,
      $Exclude,
      $Prefix = '',
      $RelativeBasePath = '',
      [switch]$Force
    )

    if($RelativeBasePath) {
        Push-Location $RelativeBasePath
    }

    # remove old zip file
    if ((Test-Path $DestinationPath) -and $Force) { Remove-Item $DestinationPath -ErrorAction Stop }

    #create zip file
    if (!([System.Management.Automation.PSTypeName]'System.IO.Compression').Type) {
        Add-Type -AssemblyName System.IO.Compression
        Add-Type -AssemblyName System.IO.Compression.FileSystem
    }
    $archive = [System.IO.Compression.ZipFile]::Open($DestinationPath, [System.IO.Compression.ZipArchiveMode]::Create)

    $compressionLevelValue = @{
        'Optimal' = [System.IO.Compression.CompressionLevel]::Optimal 
        'Fastest' = [System.IO.Compression.CompressionLevel]::Fastest
        'NoCompression' = [System.IO.Compression.CompressionLevel]::NoCompression
    }[$CompressionLevel]

    [array]$Excludes = $Exclude
    [array]$Paths = $Path
    $_is_exclude = {
      param($uxpath)
      foreach($exclude in $Excludes) {
          if($uxpath -like $exclude) {
              return $true
          }
      }
      return $false
    }

    $Script:total = 0

    $_zip_add = {
        param($archive, $path, $compressionLevel, $prefix)
        if(!$path.LinkType) {
            # -RelativeBasePath add in powershell 7.4 which github ci is 7.2 not support
            $rname = $(Resolve-Path -Path $path -Relative).Replace('\', '/')
            if ($rname.StartsWith('./')) { $rname = $rname.TrimStart('./') }
            $excluded = (&$_is_exclude -uxpath $rname)
            if(!$excluded) {
                if($prefix) { $rname = Join-Path $prefix $rname }
                
                if (!$path.PSIsContainer) {
                    ++$Script:total
                    $zentry = $archive.CreateEntry($rname)
                    if ($path.UnixStat) {
                        # when run on unix, set permissions same with origin file
                        # refer https://github.com/PowerShell/Microsoft.PowerShell.Archive/pull/146/files
                        $zentry.ExternalAttributes = ((0x8000 -bor $path.UnixStat.Mode) -shl 16)
                    }
                    $zentryWriter = New-Object -TypeName System.IO.BinaryWriter $zentry.Open()
                    $zentryWriter.Write([System.IO.File]::ReadAllBytes($path))
                    $zentryWriter.Flush()
                    $zentryWriter.Close()
                } else {
                    $sub_paths = Get-ChildItem $path
                    foreach($sub_path in $sub_paths) {
                        &$_zip_add $archive $sub_path $compressionLevel $prefix
                    }
                }
            }
            else {
                Write-Host "x` $path"
            }
        }
        else {
            Write-Host "x $path, LinkType=$($Path.LinkType)"
        }
    }

    # write entries with relative paths as names
    foreach ($path in $Paths) {
        if($path.GetType() -eq [string]) {
            $path = Get-Item $path
        }
        &$_zip_add $archive $path $compressionLevelValue $Prefix
    }

    # release zip file
    $archive.Dispose()

    if($RelativeBasePath) {
        Pop-Location
    }

    return $Script:total
}

# Compress-Archive @compress_args
$total = Compress-ArchiveEx @compress_args -Force

$md5_digest = (Get-FileHash $pkg_file_path -Algorithm MD5).Hash

Write-Host "Create package $pkg_file_path done, ${total} files found, MD5: $md5_digest"

Pop-Location

if($env:GITHUB_ACTIONS -eq 'true') {
    $release_note = Join-Path $AX_ROOT "release_note_draft.txt"
    [System.IO.File]::WriteAllText($release_note, "## MD5 Hash of the release artifacts`n  - ``${pkg_file_name}``: $md5_digest")
    echo "release_tag=v$version" >> ${env:GITHUB_OUTPUT}
    echo "release_pkg=$pkg_file_name" >> ${env:GITHUB_OUTPUT}
    echo "release_note=$release_note" >> ${env:GITHUB_OUTPUT}
}
