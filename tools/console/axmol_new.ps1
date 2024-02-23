param(
    $packageName,
    $directory,
    $lang
)

$params = [System.Collections.ArrayList]$args
$portrait_idx = $params.IndexOf('--portrait')
$is_portrait = $portrait_idx -ne -1
if ($is_portrait) {
    $params.RemoveAt($portrait_idx)
}
$projectName = $params[0]

if (!$packageName) {
    throw "The package name can't be empty"
}

if (!$projectName) {
    throw "The project name can't be empty"
}

# convert to absolute path
$directory = realpath $directory

if ($packageName.IndexOf('-') -ne -1) {
    # !!!reason:
    #   - android package not accept '-'
    #   - ios deploy device may failed with unknown error
    $packageName = $packageName.Replace('-', '_')
    println "Warning: package name was converted to '$packageName'!"
}

$sourcePath = Join-Path $env:AX_ROOT "templates/$lang"

if(!(Test-Path $sourcePath)) {
    throw 'Invalid lang, valid is cpp or lua'
}

$destinationPath = Join-Path $directory $projectName

if(!(Test-Path $destinationPath -PathType Container)) {
    Copy-Item $sourcePath $destinationPath -Recurse -Container -Force
} else {
    println "$destinationPath folder is already exist."
    return
}

# replace, file/files, from/pattern

$template_cfg_file = Join-Path $sourcePath 'axproj-template.json'
$template_cfg = ConvertFrom-Json (Get-Content $template_cfg_file -Raw)

# variable for replace
$projectDir = $(Resolve-Path $destinationPath).Path

println "Creating project $projectName ..."
println "==> packageName: $packageName"
println "==> destinationPath: $destinationPath"
println "==> lang: $lang"
println "==> is_portrait: $is_portrait"

# actionParam
#   rep
#     from
#     to
#     files
#   cp
#     from
#     to
#   del:
#     files
function perform_action($actionParam) {
    $from = if ($actionParam.from){ . (Invoke-Expression "{ ""$($actionParam.from)"" }") } else { $actionParam.pattern } # expand envs/vars
    $to = . (Invoke-Expression "{ ""$($actionParam.to)"" } ") # expand envs/vars
    switch($actionParam.action) {
        'rep' {
            foreach($filepath in $actionParam.files) {
                $realpath = Join-Path $projectDir $filepath
                $content = [System.IO.File]::ReadAllText($realpath)

                if (!$actionParam.pattern) {
                    $content = $content.Replace($from, $to)
                } else {
                    if ([Regex]::Match($content, $from).Success) {
                        $content = [Regex]::Replace($content, $from, $to)
                    }
                }

                [System.IO.File]::WriteAllText($realpath, $content)
            }
        }
        'cp' {
            if (!$actionParam.is_dir) {
                Copy-Item -Path $from -Destination $to -Force
            } else {
                Copy-Item -Path $from -Destination $to -Recurse -Container -Force
            }
        }
        'ren' {
            Rename-Item $from $to -Force
        }
        'del' {
            foreach($filepath in $actionParam.files) {
                $realpath = Join-Path $projectDir $filepath
                if (Test-Path $realpath -PathType Leaf) {
                    Remove-Item -Path $realpath -Force
                }
            }
        }
    }
}

foreach($actionParam in $template_cfg.do_default) {
    perform_action $actionParam
}

if ($is_portrait) {
    foreach($actionParam in $template_cfg.do_portrait)
    {
        perform_action $actionParam
    }
}

# write .axproj.json
$axprojInfo = @{ engine_version = $axmolVersion; project_type = $lang }
Set-Content -Path "$projectDir/.axproj.json" -Value (ConvertTo-Json -InputObject $axprojInfo)

println "Create project $projectName done."
