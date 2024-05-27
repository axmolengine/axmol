param(
    $version = $null
)

$AX_ROOT = (Resolve-Path $PSScriptRoot/../..).Path

$prerelease = 'false'
if (!$version -or ($version -eq 'auto')) {
    $prerelease = 'true'
    $axver_file = (Resolve-Path $AX_ROOT/core/axmolver.h.in).Path
    $axver_content = $(Get-Content -Path $axver_file)
    function parse_axver($part) {
        return ($axver_content | Select-String "#define AX_VERSION_$part").Line.Split(' ')[2]
    }
    function get_full_version() {
        $axver = "$(parse_axver 'MAJOR').$(parse_axver 'MINOR')"

        $git_prog = (Get-Command 'git' -ErrorAction SilentlyContinue).Source
        if ($git_prog) {
            $branchName = $(git -C $AX_ROOT branch --show-current)
            if ($branchName -eq 'dev') {
                $commitHash = $(git -C $AX_ROOT rev-parse --short=7 HEAD)
                $axver += ".$(Get-Date -Format "yyyyMMdd")"
                $axver += "-$commitHash"
                $axver += '-nightly'
            }
        }
        else {
            $axver += $(parse_axver 'PATCH')
        }
        return $axver
    }
    $version = get_full_version
} 
elseif($version -match '.*-(beta.*|rc.*)')
{
    $prerelease = 'true'
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
    '*/_x/*'
)

$pkg_file_name = "axmol-$version.zip"
$pkg_file_path = $(Join-Path $AX_ROOT $pkg_file_name)

Write-Host "Creating package $pkg_file_path ..."

$compress_args = @{
    Path             = $AX_ROOT
    CompressionLevel = 'Optimal'
    DestinationPath  = $pkg_file_path
    RelativeBasePath = $AX_ROOT
    Exclude          = $excludes
    Prefix           = "axmol-$version"
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

    $Script:S_IFREG = 0x8000
    # $S_IFDIR = 0x4000

    if ($RelativeBasePath) {
        Push-Location $RelativeBasePath
    }

    # remove old zip file
    if ((Test-Path $DestinationPath) -and $Force) { Remove-Item $DestinationPath -ErrorAction Stop }

    #create zip file
    if (!([System.Management.Automation.PSTypeName]'System.IO.Compression').Type) {
        Add-Type -AssemblyName System.IO.Compression
        Add-Type -AssemblyName System.IO.Compression.FileSystem
    }

    # import VersionEx
    . (Join-Path $AX_ROOT '1k/extensions.ps1')

    if (([VersionEx]$PSVersionTable.PSVersion.ToString() -ge [VersionEx]'7.0') -and $IsWindows) {

        if (-not ([System.Management.Automation.PSTypeName]'UnixFileStream').Type) {
            Add-Type -TypeDefinition @"
// A hack to create unix style .zip on windows
// refers:
//  - https://github.com/dotnet/runtime/blob/main/src/libraries/System.IO.Compression/src/System/IO/Compression/ZipVersion.cs#L24
//  - https://github.com/dotnet/runtime/blob/main/src/libraries/System.IO.Compression/src/System/IO/Compression/ZipArchiveEntry.cs#L529C26-L529C50

using System.Text;
using System.IO;
using System.IO.Compression;

public class MyZipFile : ZipArchive
{
    public UnixFileStream Stream { get; set; }
    public MyZipFile(UnixFileStream stream, ZipArchiveMode mode, bool leaveOpen) : base(stream, mode, leaveOpen)
    {
        Stream = stream;
    }

    protected override void Dispose(bool disposing)
    {
        if (disposing)
            Stream.IsDisposing = true;
        base.Dispose(disposing);
    }
}

public class UnixFileStream : FileStream
{
    internal enum ZipVersionMadeByPlatform : byte
    {
        Windows = 0,
        Unix = 3
    }

    // public const uint DirectoryFileHeaderSignatureConstant = 0x02014B50;
    // public const uint LocalFileHeaderSignatureConstant = 0x04034B50;
    int m_hints = -1;
    int m_hints2 = -1;

    public UnixFileStream(string path, FileMode mode, FileAccess access, FileShare share, int bufferSize, bool useAsync) : base(path, mode, access, share, bufferSize, useAsync)
    {
    }

    public override void WriteByte(byte value)
    {
        if (m_hints2 != -1) ++m_hints;
        if (IsDisposing)
        {
            if (m_hints != -1) ++m_hints;

            if (m_hints == 2)
            { // hint: CurrentZipPlatform:  hack set to unix
                value = (byte)ZipVersionMadeByPlatform.Unix;
            }
        }
        base.WriteByte(value);
    }

    public override void Write(byte[] array, int offset, int count)
    {
        if (IsDisposing)
        {  // hint: entryHeaderSignature
            if ((count == 4 && array[0] == 0x50 && array[1] == 0x4b && array[2] == 0x01 && array[3] == 0x02) || m_hints != -1)
                ++m_hints;

            if (m_hints == 17) // hint: filepath
            {
                var path = Encoding.UTF8.GetString(array);
                array = Encoding.UTF8.GetBytes(path.Replace('\\', '/'));
                m_hints = -1;
            }
        }

        if ((count == 4 && array[0] == 0x50 && array[1] == 0x4b && array[2] == 0x03 && array[3] == 0x04) || m_hints2 != -1)
            ++m_hints2;

        if (m_hints2 == 10) {
            var path = Encoding.UTF8.GetString(array);
            array = Encoding.UTF8.GetBytes(path.Replace('\\', '/'));
            m_hints2 = -1;
        }

        base.Write(array, offset, count);
    }

    public bool IsDisposing { set; get; } = false;

    public static ZipArchive CreateUnixZipFile(string archiveFileName)
    {
        var fs = new UnixFileStream(archiveFileName, FileMode.CreateNew, FileAccess.Write, FileShare.None, bufferSize: 0x1000, useAsync: false);
        try
        {
            return new MyZipFile(fs, ZipArchiveMode.Create, leaveOpen: false);
        }
        catch
        {
            fs.Dispose();
            throw;
        }
    }
}
"@
        }

        $archive = [UnixFileStream]::CreateUnixZipFile($DestinationPath)
    }
    else {
        $archive = [System.IO.Compression.ZipFile]::Open($DestinationPath, [System.IO.Compression.ZipArchiveMode]::Create)
    }
    $compressionLevelValue = @{
        'Optimal'       = [System.IO.Compression.CompressionLevel]::Optimal 
        'Fastest'       = [System.IO.Compression.CompressionLevel]::Fastest
        'NoCompression' = [System.IO.Compression.CompressionLevel]::NoCompression
    }[$CompressionLevel]

    [array]$Excludes = $Exclude
    [array]$Paths = $Path
    $_is_exclude = {
        param($uxpath)
        foreach ($exclude in $Excludes) {
            if ($uxpath -like $exclude) {
                return $true
            }
        }
        return $false
    }

    $Script:total = 0

    $_zip_add = {
        param($archive, $path, $compressionLevel, $prefix)
        if (!$path.LinkType) {
            # -RelativeBasePath add in powershell 7.4 which github ci is 7.2 not support
            $rname = $(Resolve-Path -Path $path -Relative).Replace('\', '/')
            if ($rname.StartsWith('./')) { $rname = $rname.TrimStart('./') }
            $excluded = (&$_is_exclude -uxpath $rname)
            if (!$excluded) {
                if (!$path.PSIsContainer) {
                    Write-Host "a $rname"
                    # preserve unix file permissions mode
                    # refer https://github.com/PowerShell/Microsoft.PowerShell.Archive/pull/146/files
                    $uxmode = $null
                    if ($path.UnixStat) {
                        $uxmode = $path.UnixStat.Mode
                    } 
                    else {
                        $fileext = Split-Path $rname -Extension
                        if (!$fileext -or $rname.EndsWith('.sh')) {
                            $filestatus = $(git -C $AX_ROOT ls-files -s $rname)
                            if ($filestatus) {
                                $uxmode = [Convert]::ToInt32($filestatus.Split(' ')[0], 8)
                            }
                        }
                    }
                    if (!$uxmode) {
                        # default unix file permissions
                        $uxmode = [Convert]::ToInt32('100644', 8)
                    }
		    
                    if ($prefix) { 
                        $rname = Join-Path $prefix $rname 
                    }
                    $zentry = $archive.CreateEntry($rname)
                    $zentry.ExternalAttributes = (($Script:S_IFREG -bor $uxmode) -shl 16)
                    $zentryWriter = New-Object -TypeName System.IO.BinaryWriter $zentry.Open()
                    $zentryWriter.Write([System.IO.File]::ReadAllBytes($path))
                    $zentryWriter.Flush()
                    $zentryWriter.Close()
                    
                    ++$Script:total
                }
                else {
                    $sub_paths = Get-ChildItem $path
                    foreach ($sub_path in $sub_paths) {
                        &$_zip_add $archive $sub_path $compressionLevel $prefix
                    }
                }
            }
            else {
                Write-Host "x $rname"
            }
        }
        else {
            Write-Host "x $rname, LinkType=$($Path.LinkType)"
        }
    }

    # write entries with relative paths as names
    foreach ($path in $Paths) {
        if ($path.GetType() -eq [string]) {
            $path = Get-Item $path
        }
        &$_zip_add $archive $path $compressionLevelValue $Prefix
    }

    # release zip file
    $archive.Dispose()

    if ($RelativeBasePath) {
        Pop-Location
    }

    return $Script:total
}

# Generate release note
$release_note_content = ''
if ($prerelease -eq 'false') {
    $release_note = Join-Path $AX_ROOT "release_note_${version}.txt"

    $changelog_lines = Get-Content (Join-Path $AX_ROOT 'CHANGELOG.md')
    $release_count = 0
    foreach ($line in $changelog_lines) {
        if ($line.StartsWith('## axmol-')) {
            ++$release_count
            if ($release_count -lt 2) {
                $release_note_content += "*The $version release is a minor ``LTS`` release for bugfixes and improvements*`n"
            } else {
                break
            }
        }
        else {
            $release_note_content += "$line`n"
        }
    }
}
else {
    $release_note = Join-Path $AX_ROOT "release_note_draft.txt"
}

New-Item -Path $release_note -ItemType File -Value $release_note_content -Force

# Compress-Archive @compress_args
$total = Compress-ArchiveEx @compress_args -Force

$md5_digest = (Get-FileHash $pkg_file_path -Algorithm MD5).Hash

Write-Host "Create package $pkg_file_path done, ${total} files found, MD5: $md5_digest"

Pop-Location

if ($env:GITHUB_ACTIONS -eq 'true') {
    [System.IO.File]::AppendAllText($release_note, "## MD5 Hash of the release artifacts`n  - ``${pkg_file_name}``: $md5_digest")
    echo "release_tag=v$version" >> ${env:GITHUB_OUTPUT}
    echo "release_pkg=$pkg_file_name" >> ${env:GITHUB_OUTPUT}
    echo "release_note=$release_note" >> ${env:GITHUB_OUTPUT}
    echo "prerelease=$prerelease" >> ${env:GITHUB_OUTPUT}
}
