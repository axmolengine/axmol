param(
    $packageName,
    $directory,
    $lang,
    [switch]$isolated,
    [switch]$repair,
    [switch]$forceOverwrite
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

if ($packageName.Contains('-')) {
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

$projectExists = Test-Path $destinationPath -PathType Container
if($projectExists -and !$repair) {
    println "$destinationPath folder is already exist."
    return
}

if(!$projectExists) { $repair = $false }

# replace, file/files, from/pattern

$template_cfg_file = Join-Path $sourcePath 'axproj-template.json'
$template_cfg = ConvertFrom-Json (Get-Content $template_cfg_file -Raw)

# variable for replace
if(!$repair) {
    println "Creating project $projectName ..."
} else {
    println "Repairing project $projectName ..."
}
println "==> packageName: $packageName"
println "==> destinationPath: $destinationPath"
println "==> lang: $lang"
println "==> is_portrait: $is_portrait"

# copy language spec files
if(!$projectExists) {
    Copy-Item $sourcePath $destinationPath -Recurse -Container -Force
}

$projectDir = $(Resolve-Path $destinationPath).Path

function repair_directories ($Source, $Destination) {
    if($Source -match '[\\/]\*$') {
        $Source = $Source -replace '[\\/]\*$', ''
    }

    if (-not (Test-Path -LiteralPath $Source)) {
        Throw "Source path '$Source' does not exist."
    }

    if (-not (Test-Path -LiteralPath $Destination)) {
        New-Item -ItemType Directory $Destination | Out-Null
    }

    Get-ChildItem -LiteralPath $Source | ForEach-Object {
        $target = Join-Path -Path $Destination -ChildPath $_.Name

        if ($_.PSIsContainer) {
            if (-not (Test-Path -LiteralPath $target)) {
                New-Item -ItemType Directory $target | Out-Null
            }
            repair_directories -Source $_.FullName -Destination $target
        }
        else {
            if (!(Test-Path $target -PathType Leaf)) {
                println "Repairing: $target"
                Copy-Item $_.FullName -Destination $target -Force
            }
            elseif($forceOverwrite) {
                println "Overwriting: $target"
                Copy-Item $_.FullName -Destination $target -Force
            }
            else {
                println "Skipping repairing: $target, already exists."
            }
        }
    }
}

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
            $to = realpath $to
            if(!$repair) {
                if(!$actionParam.is_dir) {
                    Copy-Item -Path $from -Destination $to -Force
                }
                else {
                    Copy-Item -Path $from -Destination $to -Recurse -Container -Force
                }
            } else {
                if (!$actionParam.is_dir) {
                    if (!(Test-Path $to -PathType Leaf)) { 
                        println "Repairing: $to"
                        Copy-Item -Path $from -Destination $to -Force 
                    }
                    elseif($forceOverwrite) {
                        println "Overwriting: $to"
                        Copy-Item -Path $from -Destination $to -Force
                    }
                    else { println "Skipping repairing: $to, already exists." }
                } else {
                    repair_directories -Source $from -Destination $to
                }
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

# engine sources for isolated project, but share axmol cmdlinetool
if($isolated) {
    println "==> Copy whole engine sources to isolated project: $projectName"
    $_ax_root = Join-Path $projectDir 'axmol'
    New-Item $_ax_root -ItemType Directory 1>$null
    $_ax_source_folders = @(
        '1k'
        '3rdparty'
        'cmake'
        'core'
        'extensions'
        'manifest.json'
    )
    foreach($path in $_ax_source_folders) {
        $source_path = Join-Path $env:AX_ROOT $path
        $dest_path = Join-Path $_ax_root $path
        if ($repair -and (Test-Path $dest_path)) {
            println println "Skipping repairing: $dest_path, already exists."
            continue
        }
        if(Test-Path $source_path -PathType Container) {
            Copy-Item $source_path $_ax_root -Container -Recurse -Force
        } else {
            Copy-Item $source_path $_ax_root -Force
        }
    }
}

# write .axproj
$proj_file = Join-Path $projectDir '.axproj'
if (!$repair -or !(Test-Path $proj_file -PathType Leaf)) {
    $axprojInfo = "package_name=$packageName`nengine_version=$axmolVersion`nproject_type=$lang"
    Set-Content -Path $proj_file -Value $axprojInfo
} else {
    println "Skipping repairing: $proj_file, already exists."
}

if(!$repair) {
    println "Create project $projectName done."
} else {
    println "Repair project $projectName done."
}
