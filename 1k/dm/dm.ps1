# need plan args to avoid powershell preprocess args unexpected behavior
$cc = $args[0]
$cflags = $args[1]

$IsWin = $IsWindows -or ("$env:OS" -eq 'Windows_NT')

echo "cflags=${cflags}"

if($cflags) { $cflags = Invoke-Expression -Command "echo $cflags" }

if($cc.EndsWith('cl.exe')) {
    # vs2019+ support: 
    &$cc /EP /Zc:preprocessor /PD $cflags (Join-Path $PSScriptRoot 'dm.c')
} else {
    Write-Host "dump command: <<$cc -E -dM $cflags ->>"
    echo ''| &$cc -E -dM $cflags -
}
