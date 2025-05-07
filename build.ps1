param(
    [Parameter(Mandatory = $true, HelpMessage = "Provide CMake Configure Preset")]
    [string]$configurePreset,

    [Parameter(Mandatory = $true, HelpMessage = "Provide CMake Build Preset")]
    [string]$buildPreset,

    [Parameter(Mandatory = $false, HelpMessage = "Dont Update Dependencies")]
    [switch]$noupdate = $false,

    [Parameter(Mandatory = $false, HelpMessage = "make clean")]
    [switch]$clean = $false
)

$CMakeSrcDir = "./CHIP8" #Relative path to top level CMakeLists.txt file

#List of dependencies
#   path - Relative path where dependency is expected
#   repo - Remote git URL
$dependencies = @(
    @{path = "./CHIP8/external/RayLib"; repo = "https://github.com/raysan5/raylib.git"}
)

#Update a dependency
function updateDependency ([string]$path, [string]$repo){
    if(-not (Test-Path $path)) {
        Write-Host "Cloning Dependency..."
        git clone $repo $path
        Write-Host "Dependency Cloned at $(Resolve-Path $path)"
    } else {
        Write-Host "Checking For Dependecy Updates $(Resolve-Path $path)"

        Push-Location $path

        git fetch origin
        $local = git rev-parse HEAD
        $remote = git rev-parse origin/$(git branch --show-current)

        if(-not($local -eq $remote)) {
            Write-Host "Updates Found, Pulling..."
            git pull --rebase
        } else {
            Write-Host "No Updates"
        }

        Pop-Location
    }
}

#Build program
function runBuild {
    Push-Location $CMakeSrcDir

    cmake --preset $configurePreset

    if($clean) {
        Write-Host "Building Clean..."
        cmake --build --preset $buildPreset --target clean
    }else {
        Write-Host "Building..."
        cmake --build --preset $buildPreset
    }

    Pop-Location
}

#Update all dependencies
if(-not $noupdate) {
    Write-Host "Updating Dependencies..."
    foreach ($dep in $dependencies) {
        updateDependency -path $dep.path -repo $dep.repo
    }
} else {
    Write-Host "Skipping Dependency Update..."
}

#Build project
runBuild