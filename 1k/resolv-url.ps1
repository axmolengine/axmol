# resolve artifact url
param(
    [Alias('artifact')]
    $name,
    $mirror_path,
    $target,
    $build_id,
    $branch,
    $out_var
)

$artifact_url = $null

if ($mirror_path -eq 'gcloud') {
    function Get-LatestGoodBuild {
        param (
            [string]$branch,
            [string]$target
        )
    
        $apiURL = "https://androidbuildinternal.googleapis.com/android/internal/build/v3/builds?branches=$([uri]::EscapeDataString($branch))&buildAttemptStatus=complete&buildType=submitted&maxResults=1&successful=true&target=$([uri]::EscapeDataString($target))"
        $body = Invoke-WebRequest -Uri $apiURL
        $buildData = $body | ConvertFrom-Json
        if ($buildData.builds.Count -eq 0) {
            throw "No build ID is found"
        }
        return $buildData.builds[0].buildId
    }
    
    # Validate input parameters
    $artifact = $name
    if (-not $target) { throw "Missing target." }
    if (-not $artifact) { throw "Missing artifact." }
    if (-not $build_id -and -not $branch) { throw "Missing build_id or branch." }
    if (-not $build_id -and $branch) {
        $build_id = Get-LatestGoodBuild -branch $branch -target $target
    }

    $artifact_url = "https://androidbuildinternal.googleapis.com/android/internal/build/v3/builds/$([uri]::EscapeDataString($build_id))/$([uri]::EscapeDataString($target))/attempts/latest/artifacts/$([uri]::EscapeDataString($artifact))/url"
}
else {
    if (!$mirror_path) { $mirror_path = Join-Path $PSScriptRoot 'mirrors.json' }
    if (Test-Path $mirror_path -PathType Leaf) {
        $active_mirror_file = Join-Path $PSScriptRoot '.active-mirror'
        if (Test-Path $active_mirror_file -PathType Leaf) {
            $active_mirror = Get-Content $active_mirror_file
        }
        else {
            $active_mirror = 'origin'
        }

        $mirrors_conf = ConvertFrom-Json (Get-Content $mirror_path -raw)
        $repo_url = $mirrors_conf.dependencies.$name.mirrors.$active_mirror

        . (Join-Path $PSScriptRoot 'extensions.ps1')
        $versions = ConvertFrom-Props (Get-Content $(Join-Path $PSScriptRoot 'build.profiles'))
        $ver = $versions[$name]

        $artifact_url = "$repo_url#$ver"
    }
}

if($artifact_url) {
    if(!$out_var) {
        Write-Host $artifact_url -NoNewline
    } else {
        Write-Information $artifact_url -InformationVariable $out_var
    }
}
