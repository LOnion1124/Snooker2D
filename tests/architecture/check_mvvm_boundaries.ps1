param(
    [switch]$Verbose
)

$ErrorActionPreference = "Continue"
$violations = @()
$root = $PSScriptRoot + "/../.."

function Check-Violation {
    param([string]$Description, [string[]]$Lines)
    if ($Lines.Count -gt 0) {
        $violations += "[FAIL] $Description"
        if ($Verbose) {
            foreach ($line in $Lines) {
                Write-Host "  $line" -ForegroundColor Red
            }
        }
    } else {
        Write-Host "[PASS] $Description" -ForegroundColor Green
    }
}

# 1. View layer should not include viewmodel/model headers
$viewVmIncludes = Select-String -Path "$root/src/view/*.cpp","$root/src/view/*.h" `
    -Pattern '#include\s+".*(viewmodel|model)/' 2>$null
Check-Violation "View must not include viewmodel/model headers" $viewVmIncludes

# 2. View layer should not reference concrete ViewModel types
$viewVmTypes = Select-String -Path "$root/src/view/*.cpp","$root/src/view/*.h" `
    -Pattern 'GameViewModel|CueControlViewModel|ScoreViewModel' 2>$null
$viewVmTypes = $viewVmTypes | Where-Object { 
    $_.Line -notmatch 'class\s+\w+ViewModel\s*;'
}
Check-Violation "View must not reference concrete ViewModel class names" $viewVmTypes

# 3. View should not have setViewModel methods
$setViewModelCalls = Select-String -Path "$root/src/view/*.cpp","$root/src/view/*.h" `
    -Pattern 'setViewModel' 2>$null
Check-Violation "View must not have setViewModel methods" $setViewModelCalls

# 4. App entry point (main.cpp) should not call Model business methods
$appBusinessCalls = Select-String -Path "$root/src/main.cpp" `
    -Pattern 'startNewGame|setupBindings|performShot' 2>$null
Check-Violation "App must not call Model business methods" $appBusinessCalls

# 5. CMakeLists.txt: snooker_view must not link snooker_viewmodel/snooker_model
$cmakeContent = Get-Content "$root/CMakeLists.txt" -Raw 2>$null
if ($cmakeContent) {
    # Use regex with whitespace to match only snooker_view, not snooker_viewmodel
    $viewLinkBlock = [regex]::Match($cmakeContent, 
        'target_link_libraries\(snooker_view\s[^)]*\)')
    if ($viewLinkBlock.Success) {
        $block = $viewLinkBlock.Value
        if ($block -match "snooker_viewmodel") {
            $violations += "[FAIL] snooker_view CMake must not link snooker_viewmodel"
        } else {
            Write-Host "[PASS] snooker_view does not link snooker_viewmodel" -ForegroundColor Green
        }
        if ($block -match "snooker_model") {
            $violations += "[FAIL] snooker_view CMake must not link snooker_model"
        } else {
            Write-Host "[PASS] snooker_view does not link snooker_model" -ForegroundColor Green
        }
    }

    $viewIncBlock = [regex]::Match($cmakeContent, 
        'target_include_directories\(snooker_view\s[^)]*\)')
    if ($viewIncBlock.Success) {
        $incBlock = $viewIncBlock.Value
        if ($incBlock -match 'src/viewmodel') {
            $violations += "[FAIL] snooker_view include path must not contain src/viewmodel"
        } else {
            Write-Host "[PASS] snooker_view include path does not contain src/viewmodel" -ForegroundColor Green
        }
    }
}

# Summary
Write-Host ""
Write-Host "============================================================"
if ($violations.Count -eq 0) {
    Write-Host "Architecture check PASSED!" -ForegroundColor Green
    exit 0
} else {
    $count = $violations.Count
    Write-Host "Architecture check FAILED: $count violation(s)" -ForegroundColor Red
    foreach ($v in $violations) {
        Write-Host ("  " + $v) -ForegroundColor Red
    }
    exit 1
}
