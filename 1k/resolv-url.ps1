# resolve artifact url
param(
    [Alias('artifact')]
    $name,
    $source_path,
    $target_os,
    $build_id,
    $branch,
    $out_var
)

$artifact_url = $null

if ($source_path -eq 'gcloud') {
    function Get-LatestGoodBuild {
        param (
            [string]$branch,
            [string]$target_os
        )

        $apiURL = "https://androidbuildinternal.googleapis.com/android/internal/build/v3/builds?branches=$([uri]::EscapeDataString($branch))&buildAttemptStatus=complete&buildType=submitted&maxResults=1&successful=true&target=$([uri]::EscapeDataString($target_os))"
        $body = Invoke-WebRequest -Uri $apiURL
        $buildData = $body | ConvertFrom-Json
        if ($buildData.builds.Count -eq 0) {
            throw "No build ID is found"
        }
        return $buildData.builds[0].buildId
    }

    # Validate input parameters
    $artifact = $name
    if (-not $target_os) { throw "Missing target_os." }
    if (-not $artifact) { throw "Missing artifact." }
    if (-not $build_id -and -not $branch) { throw "Missing build_id or branch." }
    if (-not $build_id -and $branch) {
        $build_id = Get-LatestGoodBuild -branch $branch -target $target_os
    }

    $artifact_url = "https://androidbuildinternal.googleapis.com/android/internal/build/v3/builds/$([uri]::EscapeDataString($build_id))/$([uri]::EscapeDataString($target_os))/attempts/latest/artifacts/$([uri]::EscapeDataString($artifact))/url"
}
else {
    if (!$source_path) { $source_path = Join-Path $PSScriptRoot 'sources.json' }
    if (Test-Path $source_path -PathType Leaf) {
        . (Join-Path $PSScriptRoot 'extensions.ps1')

        $1k_env_file = Join-Path $PSScriptRoot '.env'
        if (Test-Path $1k_env_file -PathType Leaf) {
            $1k_env = ConvertFrom-Props (Get-Content $1k_env_file)
            $active_mirror = $1k_env.active_mirror
        }
        else {
            $active_mirror = 'origin'
        }

        $sources_conf = ConvertFrom-Json (Get-Content $source_path -raw)
        $url = $sources_conf.dependencies.$name.sources.$active_mirror

        $versions = ConvertFrom-Props (Get-Content $(Join-Path $PSScriptRoot 'build.profiles'))
        $ver = $versions[$name]

        $artifact_url = "$url#$ver"
    }
}

if($artifact_url) {
    if(!$out_var) {
        Write-Host $artifact_url -NoNewline
    } else {
        Write-Information $artifact_url -InformationVariable $out_var
    }
}
