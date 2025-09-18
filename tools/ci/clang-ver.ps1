
param (
    [string]$DllPath
)

# Check if the type is already defined to avoid duplicate Add-Type
if (-not ([System.Management.Automation.PSTypeName]'LibClang.NativeMethods').Type) {
    $code_str = @"
using System;
using System.Runtime.InteropServices;
namespace LibClang {
public static class NativeMethods
{
[StructLayout(LayoutKind.Sequential)]
public struct CXString
{
    public IntPtr data;
    public uint private_flags;
}

[DllImport(@"$DllPath", CallingConvention = CallingConvention.Cdecl)]
public static extern CXString clang_getClangVersion();

[DllImport(@"$DllPath", CallingConvention = CallingConvention.Cdecl)]
public static extern IntPtr clang_getCString(CXString string_);

[DllImport(@"$DllPath", CallingConvention = CallingConvention.Cdecl)]
public static extern void clang_disposeString(CXString string_);
}
}
"@
    Add-Type -TypeDefinition $code_str
}

try {
    # Call libclang API to get version string
    $cxString = [LibClang.NativeMethods]::clang_getClangVersion()
    $versionPtr = [LibClang.NativeMethods]::clang_getCString($cxString)
    $versionStr = [System.Runtime.InteropServices.Marshal]::PtrToStringAnsi($versionPtr)

    # Release memory
    [LibClang.NativeMethods]::clang_disposeString($cxString)

    Write-Output $versionStr
}
catch {
    Write-Host "Failed to load libclang.dll or invoke API: $($_.Exception.Message)"
    Write-Output '0.0.0'
}
