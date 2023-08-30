param(
    $packageName,
    $directory,
    $lang
)

$is_portrait = ($args[0] -eq '--portrait')
$projectName = $args[$is_portrait]

if ($lang -eq 'cpp' -or $lang -eq 'lua') {
    if (!(Test-Path $directory -PathType Container)) {
        New-Item $directory -ItemType Directory
    }
} else {
    throw 'Invalid lang, valid is cpp or lua'
}

if ($lang -eq 'cpp') {
    $sourcePath = Join-Path $env:AX_ROOT 'templates/cpp-template-default'
}
elseif ($lang -eq 'lua') {
    $sourcePath = Join-Path $env:AX_ROOT 'templates/lua-template-default'
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
$projectDir = $destinationPath

println "Creating project $projectName ..."

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

if ($orient -eq 'portrait') {
    foreach($actionParam in $template_cfg.do_portrait)
    {
        perform_action $actionParam
    }
}

# write .axproj.json
$axprojInfo = @{ engine_version = $axmolVersion; project_type = $lang }
Set-Content -Path "$projectDir/.axproj.json" -Value (ConvertTo-Json -InputObject $axprojInfo)

println "Create project $projectName done."
