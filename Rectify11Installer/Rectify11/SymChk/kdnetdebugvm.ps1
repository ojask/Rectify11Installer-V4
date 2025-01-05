#
# Argument initialization
#

$nextarg = "none"
$DebugPort = "unassigned"
$targetcomputer = "."
$VMName = ""
$VMGuid = ""
$AutoAssign = "false"
$DebugOff = "false"

function funHelp()
{
$helpText=@"

DESCRIPTION:
NAME: kdnetdebugvm.ps1
Displays (and optionally sets) the port used to network debug a VM.

PARAMETERS: 
-computerName Specifies the name of the computer on which to run the script
-help         Displays help
-vmname       (optional) Name of the VM of interest
-vmguid       (optional) GUID of the VM of interest
-port         (optional) Network port to use for debugging
-debugoff
-autoassign

Either vmname or vmguid must be specified to identify the VM, but not both.
Note that vmname may not uniquely identify the VM, but vmguid does.

SYNTAX:
kdnetdebugvm.ps1 [-computerName targetcomputer] [-vmname NameOfVM] [-vmguid GuidOfVM] [-port PortNumber]

"@
$helpText
exit
}


foreach ($argument in $args)
{
    # parse commands with no following arguments
    switch ($argument)
    {
        "?"     {funHelp}
        "help"  {funHelp}
        "-help" {funHelp}
        "/?"    {funHelp}
        "-?"    {funHelp}
        "autoassign"    {$AutoAssign = "true"}
        "-autoassign"   {$AutoAssign = "true"}
        "/autoassign"   {$AutoAssign = "true"}
        "debugoff"        {$DebugOff = "true"}
        "-debugoff"       {$DebugOff = "true"}
        "/debugoff"       {$DebugOff = "true"}
        default {}
    }

    # parse values that followed a switch

    switch ($nextarg)
    {
        "vmname"        {$VMName = $argument}
        "-vmname"       {$VMName = $argument}
        "/vmname"       {$VMName = $argument}
        "vmguid"        {$VMGuid = $argument}
        "-vmguid"       {$VMGuid = $argument}
        "/vmguid"       {$VMGuid = $argument}
        "port"          {$DebugPort = $argument}
        "-port"         {$DebugPort = $argument}
        "/port"         {$DebugPort = $argument}
        "computername"  {$targetcomputer = $argument}
        "-computername" {$targetcomputer = $argument}
        "/computername" {$targetcomputer = $argument}
        default         {}
    }

    $nextarg = $argument
}

if (($VMName -eq "") -and ($VMGuid -eq ""))
{
    funHelp
}

if (($VMName -ne "") -and ($VMGuid -ne ""))
{
    funHelp
}

$ns = "root\virtualization\v2"
$VMWPName = "$env:windir\system32\vmwp.exe"

#Get a VMManagementService object
$VMManagementService = gwmi -class "Msvm_VirtualSystemManagementService" -namespace $ns -computername $targetcomputer

#Get the VM object that we want to modify
if ($VMName -ne "")
{
    $VM = Get-VM -computername $targetcomputer -VMName $VMName
}

if ($VMGuid -ne "")
{
    $VM = Get-VM -computername $targetcomputer -Id $VMGuid
}

#Get the VirtualSystemGlobalSettingData of the VM we want to modify
$VMSystemGlobalSettingData = gwmi -namespace $ns -computername $targetcomputer -class Msvm_VirtualSystemSettingData | ? { $_.ConfigurationID -eq $VM.Id }

# Set a new debugport
if ($DebugPort -ne "unassigned")
{
    #Change the ElementName property
    $VMSystemGlobalSettingData.DebugPort = $DebugPort
    $VMSystemGlobalSettingData.DebugPortEnabled = 1

    $VMManagementService.ModifySystemSettings($VMSystemGlobalSettingData.GetText(1))
    $FWRuleName = "SynthDebugInboundRule-$DebugPort"
    New-NetFirewallRule -DisplayName $FWRuleName -Program $VMWPName -Protocol UDP -Action Allow -Direction Inbound -LocalPort $DebugPort
}

# Enable auto assigned debug ports
if ($AutoAssign -ne "false")
{
    #Change the ElementName property
    $VMSystemGlobalSettingData.DebugPortEnabled = 2
    $VMManagementService.ModifySystemSettings($VMSystemGlobalSettingData.GetText(1))
    Write-Host -Foreground Yellow "Firewall Ports for autoassign mode can be opened only after the VM is started."
}

# Turn off debugging
if ($DebugOff -ne "false")
{
    $DebugPort = $VMSystemGlobalSettingData.DebugPort
    #Change the ElementName property
    $VMSystemGlobalSettingData.DebugPortEnabled = 0
    $VMSystemGlobalSettingData.DebugPort = 0
    $VMManagementService.ModifySystemSettings($VMSystemGlobalSettingData.GetText(1))
    # May throw an exception if the rule did not exist already.
    # If two rules exist with the same name, both will be deleted.
    if ($DebugPort -ne 0)
    {
        $FWRuleName = "SynthDebugInboundRule-$DebugPort"
        Remove-NetFirewallRule -DisplayName $FWRuleName
    }
}

$VMSystemGlobalSettingData

exit

# SIG # Begin signature block
# MIImXgYJKoZIhvcNAQcCoIImTzCCJksCAQExDzANBglghkgBZQMEAgEFADB5Bgor
# BgEEAYI3AgEEoGswaTA0BgorBgEEAYI3AgEeMCYCAwEAAAQQH8w7YFlLCE63JNLG
# KX7zUQIBAAIBAAIBAAIBAAIBADAxMA0GCWCGSAFlAwQCAQUABCDbHDLYuqp9Du8W
# ZSYoLwjEyhkYywkzwemxEtpUqBqajKCCC3YwggT+MIID5qADAgECAhMzAAAFplgQ
# Z0s9bHz2AAAAAAWmMA0GCSqGSIb3DQEBCwUAMH4xCzAJBgNVBAYTAlVTMRMwEQYD
# VQQIEwpXYXNoaW5ndG9uMRAwDgYDVQQHEwdSZWRtb25kMR4wHAYDVQQKExVNaWNy
# b3NvZnQgQ29ycG9yYXRpb24xKDAmBgNVBAMTH01pY3Jvc29mdCBDb2RlIFNpZ25p
# bmcgUENBIDIwMTAwHhcNMjQwODIyMTkyNTU3WhcNMjUwNzA1MTkyNTU3WjB0MQsw
# CQYDVQQGEwJVUzETMBEGA1UECBMKV2FzaGluZ3RvbjEQMA4GA1UEBxMHUmVkbW9u
# ZDEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBvcmF0aW9uMR4wHAYDVQQDExVNaWNy
# b3NvZnQgQ29ycG9yYXRpb24wggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIB
# AQC6Y0Y5HouBkWTVMeTa9bc2tcYo/BOP01al7iEpsJe2tphp+h8raHrtRmBxpRMG
# nkOdah2vPDmeHBk8b2CWFSYyP96wcQ0bz/Jrn0df02/qNBjYwtU8H4yv8PCVwG1L
# JfNRt5WeZ2Yg8X6nGQYURAuXwpjRucyYlPlL6zq64nXZKIAJ9247C8TQk4/jhy9O
# w2PLw8Ic2dajd6M1BWYZTPk3OffUuLdbw5ZjknsEoxTZz+V/fj9UyMl2s+ruph4O
# jYgmxgyRI83Ky+fK+iRtpYDijLwA0LuPib467VYv0mWkMVavmA3uN8WltTKqytIr
# 2t+LA2x2YQADzJekgj6oTMcTAgMBAAGjggF9MIIBeTAfBgNVHSUEGDAWBgorBgEE
# AYI3PQYBBggrBgEFBQcDAzAdBgNVHQ4EFgQUTpwj74MWwfMVOTFfzA0ULf1Sli0w
# VAYDVR0RBE0wS6RJMEcxLTArBgNVBAsTJE1pY3Jvc29mdCBJcmVsYW5kIE9wZXJh
# dGlvbnMgTGltaXRlZDEWMBQGA1UEBRMNMjMwODY1KzUwMjcxMjAfBgNVHSMEGDAW
# gBTm/F97uyIAWORyTrX0IXQjMubvrDBWBgNVHR8ETzBNMEugSaBHhkVodHRwOi8v
# Y3JsLm1pY3Jvc29mdC5jb20vcGtpL2NybC9wcm9kdWN0cy9NaWNDb2RTaWdQQ0Ff
# MjAxMC0wNy0wNi5jcmwwWgYIKwYBBQUHAQEETjBMMEoGCCsGAQUFBzAChj5odHRw
# Oi8vd3d3Lm1pY3Jvc29mdC5jb20vcGtpL2NlcnRzL01pY0NvZFNpZ1BDQV8yMDEw
# LTA3LTA2LmNydDAMBgNVHRMBAf8EAjAAMA0GCSqGSIb3DQEBCwUAA4IBAQC1/mao
# WMt6HRh9WayU+YG7qxU3fHyUtN0fZ5jU9ZshONjogSHli0B/rLamY+NGKPt5tl4k
# HWUuN3MOHv/Rj9cb29BQ3K5zNE+MzACTUU1b7DWfw0bZccwvL5gOqUVSNy6/FVJB
# cOos7fviDnWyXvnCe49v66hDIwLqzsF182bfvrFZX7txWrQ98mW0UhyIk+nxvGnr
# ONnqh3f1pxL9BZ7zvXQWliL0Q2JXzjkpQoEVOrgxfp/zhG3PwIMJrn9iieQ1VR6s
# MsqjVvSzqbxvzMxqK7/aze2P3tU/lK8AbPq6HAWectldQlHju7Lu00jp3NY3P1cG
# CbQNJWOR/We91fRAMIIGcDCCBFigAwIBAgIKYQxSTAAAAAAAAzANBgkqhkiG9w0B
# AQsFADCBiDELMAkGA1UEBhMCVVMxEzARBgNVBAgTCldhc2hpbmd0b24xEDAOBgNV
# BAcTB1JlZG1vbmQxHjAcBgNVBAoTFU1pY3Jvc29mdCBDb3Jwb3JhdGlvbjEyMDAG
# A1UEAxMpTWljcm9zb2Z0IFJvb3QgQ2VydGlmaWNhdGUgQXV0aG9yaXR5IDIwMTAw
# HhcNMTAwNzA2MjA0MDE3WhcNMjUwNzA2MjA1MDE3WjB+MQswCQYDVQQGEwJVUzET
# MBEGA1UECBMKV2FzaGluZ3RvbjEQMA4GA1UEBxMHUmVkbW9uZDEeMBwGA1UEChMV
# TWljcm9zb2Z0IENvcnBvcmF0aW9uMSgwJgYDVQQDEx9NaWNyb3NvZnQgQ29kZSBT
# aWduaW5nIFBDQSAyMDEwMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA
# 6Q5kUHlntcTj/QkATJ6UrPdWaOpE2M/FWE+ppXZ8bUW60zmStKQe+fllguQX0o/9
# RJwI6GWTzixVhL99COMuK6hBKxi3oktuSUxrFQfe0dLCiR5xlM21f0u0rwjYzIjW
# axeUOpPOJj/s5v40mFfVHV1J9rIqLtWFu1k/+JC0K4N0yiuzO0bj8EZJwRdmVMkc
# vR3EVWJXcvhnuSUgNN5dpqWVXqsogM3Vsp7lA7Vj07IUyMHIiiYKWX8H7P8O7YAS
# NUwSpr5SW/Wm2uCLC0h31oVH1RC5xuiq7otqLQVcYMa0KlucIxxfReMaFB5vN8sZ
# M4BqiU2jamZjeJPVMM+VHwIDAQABo4IB4zCCAd8wEAYJKwYBBAGCNxUBBAMCAQAw
# HQYDVR0OBBYEFOb8X3u7IgBY5HJOtfQhdCMy5u+sMBkGCSsGAQQBgjcUAgQMHgoA
# UwB1AGIAQwBBMAsGA1UdDwQEAwIBhjAPBgNVHRMBAf8EBTADAQH/MB8GA1UdIwQY
# MBaAFNX2VsuP6KJcYmjRPZSQW9fOmhjEMFYGA1UdHwRPME0wS6BJoEeGRWh0dHA6
# Ly9jcmwubWljcm9zb2Z0LmNvbS9wa2kvY3JsL3Byb2R1Y3RzL01pY1Jvb0NlckF1
# dF8yMDEwLTA2LTIzLmNybDBaBggrBgEFBQcBAQROMEwwSgYIKwYBBQUHMAKGPmh0
# dHA6Ly93d3cubWljcm9zb2Z0LmNvbS9wa2kvY2VydHMvTWljUm9vQ2VyQXV0XzIw
# MTAtMDYtMjMuY3J0MIGdBgNVHSAEgZUwgZIwgY8GCSsGAQQBgjcuAzCBgTA9Bggr
# BgEFBQcCARYxaHR0cDovL3d3dy5taWNyb3NvZnQuY29tL1BLSS9kb2NzL0NQUy9k
# ZWZhdWx0Lmh0bTBABggrBgEFBQcCAjA0HjIgHQBMAGUAZwBhAGwAXwBQAG8AbABp
# AGMAeQBfAFMAdABhAHQAZQBtAGUAbgB0AC4gHTANBgkqhkiG9w0BAQsFAAOCAgEA
# GnTvV08pe8QWhXi4UNMi/AmdrIKX+DT/KiyXlRLl5L/Pv5PI4zSp24G43B4AvtI1
# b6/lf3mVd+UC1PHr2M1OHhthosJaIxrwjKhiUUVnCOM/PB6T+DCFF8g5QKbXDrMh
# KeWloWmMIpPMdJjnoUdD8lOswA8waX/+0iUgbW9h098H1dlyACxphnY9UdumOUjJ
# N2FtB91TGcun1mHCv+KDqw/ga5uV1n0oUbCJSlGkmmzItx9KGg5pqdfcwX7RSXCq
# tq27ckdjF/qm1qKmhuyoEESbY7ayaYkGx0aGehg/6MUdIdV7+QIjLcVBy78dTMgW
# 77Gcf/wiS0mKbhXjpn92W9FTeZGFndXS2z1zNfM8rlSyUkdqwKoTldKOEdqZZ14y
# jPs3hdHcdYWch8ZaV4XCv90Nj4ybLeu07s8n07VeafqkFgQBpyRnc89NT7beBVaX
# evfpUk30dwVPhcbYC/GO7UIJ0Q124yNWeCImNr7KsYxuqh3khdpHM2KPpMmRM19x
# HkCvmGXJIuhCISWKHC1g2TeJQYkqFg/XYTyUaGBS79ZHmaCAQO4VgXc+nOBTGBpQ
# HTiVmx5mMxMnORd4hzbOTsNfsvU9R1O24OXbC2E9KteSLM43Wj5AQjGkHxAIwlac
# vyRdUQKdannSF9PawZSOB3slcUSrBmrm1MbfI5qWdcUxgho+MIIaOgIBATCBlTB+
# MQswCQYDVQQGEwJVUzETMBEGA1UECBMKV2FzaGluZ3RvbjEQMA4GA1UEBxMHUmVk
# bW9uZDEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBvcmF0aW9uMSgwJgYDVQQDEx9N
# aWNyb3NvZnQgQ29kZSBTaWduaW5nIFBDQSAyMDEwAhMzAAAFplgQZ0s9bHz2AAAA
# AAWmMA0GCWCGSAFlAwQCAQUAoIHGMBkGCSqGSIb3DQEJAzEMBgorBgEEAYI3AgEE
# MBwGCisGAQQBgjcCAQsxDjAMBgorBgEEAYI3AgEVMC8GCSqGSIb3DQEJBDEiBCD1
# 2p2EXg5HuAOMXy8l2stP0pmptaSGI99pdu8uRzlUuDBaBgorBgEEAYI3AgEMMUww
# SqAkgCIATQBpAGMAcgBvAHMAbwBmAHQAIABXAGkAbgBkAG8AdwBzoSKAIGh0dHA6
# Ly93d3cubWljcm9zb2Z0LmNvbS93aW5kb3dzMA0GCSqGSIb3DQEBAQUABIIBAAQX
# P1XzugmTad65fqXa9WSXQ83oFGKnSEKeRZHmzvaIFdwehu4RvLSnarg6yaGYPeI0
# 2mIZ+KNIxCGonpLMzckSfCwvrwIvRnIotiDH8Db/DUI9T/h3fXnMLOxyjETIuSxG
# sqqNwiXh0tHdt13mFPWsPfNHLk+fHzc6YSqsY/cXUhgQLqc4vpjBhNN2l5xj7vNh
# MUr/J6JYFGz+YyuIQFQxAGciblWhcAnTRxm1PFBs0Yp727ljoHsEdBkSD8cskK/t
# HLZTQiWAL/m8yF9NhoqFNiCwsGiI2nPN7sYWGGph6d4hotKmElnfgaMR7xRpeez+
# gTxtO5bGHq2WazRXhEmhghewMIIXrAYKKwYBBAGCNwMDATGCF5wwgheYBgkqhkiG
# 9w0BBwKggheJMIIXhQIBAzEPMA0GCWCGSAFlAwQCAQUAMIIBWgYLKoZIhvcNAQkQ
# AQSgggFJBIIBRTCCAUECAQEGCisGAQQBhFkKAwEwMTANBglghkgBZQMEAgEFAAQg
# IkELOXWFVelDWUPtdXwEh+Ca1MTKu8i26L9zKofS1+8CBmbrajL70xgTMjAyNDEx
# MTYwNDM3MDAuOTY1WjAEgAIB9KCB2aSB1jCB0zELMAkGA1UEBhMCVVMxEzARBgNV
# BAgTCldhc2hpbmd0b24xEDAOBgNVBAcTB1JlZG1vbmQxHjAcBgNVBAoTFU1pY3Jv
# c29mdCBDb3Jwb3JhdGlvbjEtMCsGA1UECxMkTWljcm9zb2Z0IElyZWxhbmQgT3Bl
# cmF0aW9ucyBMaW1pdGVkMScwJQYDVQQLEx5uU2hpZWxkIFRTUyBFU046NTcxQS0w
# NUUwLUQ5NDcxJTAjBgNVBAMTHE1pY3Jvc29mdCBUaW1lLVN0YW1wIFNlcnZpY2Wg
# ghH+MIIHKDCCBRCgAwIBAgITMwAAAfvLy2w3Z+UwlQABAAAB+zANBgkqhkiG9w0B
# AQsFADB8MQswCQYDVQQGEwJVUzETMBEGA1UECBMKV2FzaGluZ3RvbjEQMA4GA1UE
# BxMHUmVkbW9uZDEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBvcmF0aW9uMSYwJAYD
# VQQDEx1NaWNyb3NvZnQgVGltZS1TdGFtcCBQQ0EgMjAxMDAeFw0yNDA3MjUxODMx
# MTNaFw0yNTEwMjIxODMxMTNaMIHTMQswCQYDVQQGEwJVUzETMBEGA1UECBMKV2Fz
# aGluZ3RvbjEQMA4GA1UEBxMHUmVkbW9uZDEeMBwGA1UEChMVTWljcm9zb2Z0IENv
# cnBvcmF0aW9uMS0wKwYDVQQLEyRNaWNyb3NvZnQgSXJlbGFuZCBPcGVyYXRpb25z
# IExpbWl0ZWQxJzAlBgNVBAsTHm5TaGllbGQgVFNTIEVTTjo1NzFBLTA1RTAtRDk0
# NzElMCMGA1UEAxMcTWljcm9zb2Z0IFRpbWUtU3RhbXAgU2VydmljZTCCAiIwDQYJ
# KoZIhvcNAQEBBQADggIPADCCAgoCggIBAKjCVkHlgKuC8L0o2LIz9FL4b5tI9GgD
# iYjC4NLC38SqE1wHSg+qDLquaxeaBIjsVfvaMdB/eUPH4aGat8fZcYLmckziuJds
# bXILSQrY10ZZTNm06YzoN+UVKwctHAJaAVPRiQbOywTa3Gx+qwYjr6g0DYnD0WcK
# tescozInVNSdQCbmrfci5+7Won6A+fG5WBHAb5I+XR9ZWvc1POOkA3jqETujXKhy
# 7A8fP81SmcT99JlumO0TLKrQfHBgoBsFVbqzp2jS17N9ak0U8lR1/KaTnaEooQl3
# qnm4CQkcxvMxv3v5NKGgYxRRpfvLhRC8AsoeMCvWefms0832thg+KeoobbJF7N5Z
# 1tOVCnwyYQAA7er4jnNEZP3PMzoqs4dJSqX/3llGNqP4b3Az2TYC2h78nw6m/AFm
# irzt+okWUl6oUsPEsSaNEwqbGwo5rcdC6R56m29VBe3KtPZAnH1kwz3DddqW2C6n
# JNGyCHzym3Ox565DUJLP5km1WU5w8k9zvMxfauAwn1nrEq9WpMnA3bhsQnSgb4LS
# YdWMQ6tbJE8HmMeYgFl5weyjMpbN1kGW07m0wiy7fF5/LfrJXCpuQ5L6G7m5h0q4
# rkwN8E8iMuBcWpkyptFQ7vZlnbPDLY1EiVcDVVZQV2kN2THFY4o8laFDVbgWPTHM
# GHCECutsENtBAgMBAAGjggFJMIIBRTAdBgNVHQ4EFgQUR1UhmFDUN0cDpe9cyALl
# IyCoNSowHwYDVR0jBBgwFoAUn6cVXQBeYl2D9OXSZacbUzUZ6XIwXwYDVR0fBFgw
# VjBUoFKgUIZOaHR0cDovL3d3dy5taWNyb3NvZnQuY29tL3BraW9wcy9jcmwvTWlj
# cm9zb2Z0JTIwVGltZS1TdGFtcCUyMFBDQSUyMDIwMTAoMSkuY3JsMGwGCCsGAQUF
# BwEBBGAwXjBcBggrBgEFBQcwAoZQaHR0cDovL3d3dy5taWNyb3NvZnQuY29tL3Br
# aW9wcy9jZXJ0cy9NaWNyb3NvZnQlMjBUaW1lLVN0YW1wJTIwUENBJTIwMjAxMCgx
# KS5jcnQwDAYDVR0TAQH/BAIwADAWBgNVHSUBAf8EDDAKBggrBgEFBQcDCDAOBgNV
# HQ8BAf8EBAMCB4AwDQYJKoZIhvcNAQELBQADggIBAMM6CCjmNnZ1I31rjIhqM/6L
# 6HNXvOlcFmmTRXYEZjqELkXuJy3bWTjbUxzZN0o955MgbM88Um2RENA3bsihxyOT
# /FfO4xbbRp5UdMDz9thQHm27wG7rZDDFUDBc4VQVolg9FQJ7vcdH44nyygwFVy8K
# Lp+awhasG2rFxXOx/9Az4gvgwZ97VMXn73MVAsrOPgwt7PAmKe1ll6WfFm/73QYQ
# 5Yh5ge6VnJrAfN7nOPz9hpgCNxzJDhLu3wmkmKEIaLljq9O5fyjOE53cpSIq5vH9
# lsF0HBRM5lLyEjOpbnVMBpVTX00yVKtm0wxHd7ZQyrVfQFGN665xcB08Ca8i7U+C
# BYb4AXzQ95i9XnkmpCn+8UyCOCcrdeUl4R3eaCP1xo0oMpICa1gOe6xpwAu67t/2
# WxTQjCvyY+l/F+C+pgTmGtjRisB+AN+2Bg63nCf6l11lGL3y2Khxn/E4WJddmINa
# 8EiqVi6JQPwdXqgcOE0XL1WNCLzTYubJvv/xyfQMOjSbkf7g0e1+7w14nKVzJUTY
# BTMgA2/ABSL0D3R6nEaUaK2PmFBpb83icf9oDWMnswKJG6xYQArCdgX8ni8ghKOg
# LsBB5+ddTyhPHSuCb5Zi0qB4+1RUdzRw5N80ZMdBMZJhfGjnab6CobsAQsaGfyYW
# 80s672e+BlYyiiMreRQNMIIHcTCCBVmgAwIBAgITMwAAABXF52ueAptJmQAAAAAA
# FTANBgkqhkiG9w0BAQsFADCBiDELMAkGA1UEBhMCVVMxEzARBgNVBAgTCldhc2hp
# bmd0b24xEDAOBgNVBAcTB1JlZG1vbmQxHjAcBgNVBAoTFU1pY3Jvc29mdCBDb3Jw
# b3JhdGlvbjEyMDAGA1UEAxMpTWljcm9zb2Z0IFJvb3QgQ2VydGlmaWNhdGUgQXV0
# aG9yaXR5IDIwMTAwHhcNMjEwOTMwMTgyMjI1WhcNMzAwOTMwMTgzMjI1WjB8MQsw
# CQYDVQQGEwJVUzETMBEGA1UECBMKV2FzaGluZ3RvbjEQMA4GA1UEBxMHUmVkbW9u
# ZDEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBvcmF0aW9uMSYwJAYDVQQDEx1NaWNy
# b3NvZnQgVGltZS1TdGFtcCBQQ0EgMjAxMDCCAiIwDQYJKoZIhvcNAQEBBQADggIP
# ADCCAgoCggIBAOThpkzntHIhC3miy9ckeb0O1YLT/e6cBwfSqWxOdcjKNVf2AX9s
# SuDivbk+F2Az/1xPx2b3lVNxWuJ+Slr+uDZnhUYjDLWNE893MsAQGOhgfWpSg0S3
# po5GawcU88V29YZQ3MFEyHFcUTE3oAo4bo3t1w/YJlN8OWECesSq/XJprx2rrPY2
# vjUmZNqYO7oaezOtgFt+jBAcnVL+tuhiJdxqD89d9P6OU8/W7IVWTe/dvI2k45GP
# sjksUZzpcGkNyjYtcI4xyDUoveO0hyTD4MmPfrVUj9z6BVWYbWg7mka97aSueik3
# rMvrg0XnRm7KMtXAhjBcTyziYrLNueKNiOSWrAFKu75xqRdbZ2De+JKRHh09/SDP
# c31BmkZ1zcRfNN0Sidb9pSB9fvzZnkXftnIv231fgLrbqn427DZM9ituqBJR6L8F
# A6PRc6ZNN3SUHDSCD/AQ8rdHGO2n6Jl8P0zbr17C89XYcz1DTsEzOUyOArxCaC4Q
# 6oRRRuLRvWoYWmEBc8pnol7XKHYC4jMYctenIPDC+hIK12NvDMk2ZItboKaDIV1f
# MHSRlJTYuVD5C4lh8zYGNRiER9vcG9H9stQcxWv2XFJRXRLbJbqvUAV6bMURHXLv
# jflSxIUXk8A8FdsaN8cIFRg/eKtFtvUeh17aj54WcmnGrnu3tz5q4i6tAgMBAAGj
# ggHdMIIB2TASBgkrBgEEAYI3FQEEBQIDAQABMCMGCSsGAQQBgjcVAgQWBBQqp1L+
# ZMSavoKRPEY1Kc8Q/y8E7jAdBgNVHQ4EFgQUn6cVXQBeYl2D9OXSZacbUzUZ6XIw
# XAYDVR0gBFUwUzBRBgwrBgEEAYI3TIN9AQEwQTA/BggrBgEFBQcCARYzaHR0cDov
# L3d3dy5taWNyb3NvZnQuY29tL3BraW9wcy9Eb2NzL1JlcG9zaXRvcnkuaHRtMBMG
# A1UdJQQMMAoGCCsGAQUFBwMIMBkGCSsGAQQBgjcUAgQMHgoAUwB1AGIAQwBBMAsG
# A1UdDwQEAwIBhjAPBgNVHRMBAf8EBTADAQH/MB8GA1UdIwQYMBaAFNX2VsuP6KJc
# YmjRPZSQW9fOmhjEMFYGA1UdHwRPME0wS6BJoEeGRWh0dHA6Ly9jcmwubWljcm9z
# b2Z0LmNvbS9wa2kvY3JsL3Byb2R1Y3RzL01pY1Jvb0NlckF1dF8yMDEwLTA2LTIz
# LmNybDBaBggrBgEFBQcBAQROMEwwSgYIKwYBBQUHMAKGPmh0dHA6Ly93d3cubWlj
# cm9zb2Z0LmNvbS9wa2kvY2VydHMvTWljUm9vQ2VyQXV0XzIwMTAtMDYtMjMuY3J0
# MA0GCSqGSIb3DQEBCwUAA4ICAQCdVX38Kq3hLB9nATEkW+Geckv8qW/qXBS2Pk5H
# ZHixBpOXPTEztTnXwnE2P9pkbHzQdTltuw8x5MKP+2zRoZQYIu7pZmc6U03dmLq2
# HnjYNi6cqYJWAAOwBb6J6Gngugnue99qb74py27YP0h1AdkY3m2CDPVtI1TkeFN1
# JFe53Z/zjj3G82jfZfakVqr3lbYoVSfQJL1AoL8ZthISEV09J+BAljis9/kpicO8
# F7BUhUKz/AyeixmJ5/ALaoHCgRlCGVJ1ijbCHcNhcy4sa3tuPywJeBTpkbKpW99J
# o3QMvOyRgNI95ko+ZjtPu4b6MhrZlvSP9pEB9s7GdP32THJvEKt1MMU0sHrYUP4K
# WN1APMdUbZ1jdEgssU5HLcEUBHG/ZPkkvnNtyo4JvbMBV0lUZNlz138eW0QBjloZ
# kWsNn6Qo3GcZKCS6OEuabvshVGtqRRFHqfG3rsjoiV5PndLQTHa1V1QJsWkBRH58
# oWFsc/4Ku+xBZj1p/cvBQUl+fpO+y/g75LcVv7TOPqUxUYS8vwLBgqJ7Fx0ViY1w
# /ue10CgaiQuPNtq6TPmb/wrpNPgkNWcr4A245oyZ1uEi6vAnQj0llOZ0dFtq0Z4+
# 7X6gMTN9vMvpe784cETRkPHIqzqKOghif9lwY1NNje6CbaUFEMFxBmoQtB1VM1iz
# oXBm8qGCA1kwggJBAgEBMIIBAaGB2aSB1jCB0zELMAkGA1UEBhMCVVMxEzARBgNV
# BAgTCldhc2hpbmd0b24xEDAOBgNVBAcTB1JlZG1vbmQxHjAcBgNVBAoTFU1pY3Jv
# c29mdCBDb3Jwb3JhdGlvbjEtMCsGA1UECxMkTWljcm9zb2Z0IElyZWxhbmQgT3Bl
# cmF0aW9ucyBMaW1pdGVkMScwJQYDVQQLEx5uU2hpZWxkIFRTUyBFU046NTcxQS0w
# NUUwLUQ5NDcxJTAjBgNVBAMTHE1pY3Jvc29mdCBUaW1lLVN0YW1wIFNlcnZpY2Wi
# IwoBATAHBgUrDgMCGgMVAARx5+zQhrrGc9kX1W8rsGMD8pAVoIGDMIGApH4wfDEL
# MAkGA1UEBhMCVVMxEzARBgNVBAgTCldhc2hpbmd0b24xEDAOBgNVBAcTB1JlZG1v
# bmQxHjAcBgNVBAoTFU1pY3Jvc29mdCBDb3Jwb3JhdGlvbjEmMCQGA1UEAxMdTWlj
# cm9zb2Z0IFRpbWUtU3RhbXAgUENBIDIwMTAwDQYJKoZIhvcNAQELBQACBQDq4lvL
# MCIYDzIwMjQxMTE1MjM0NjE5WhgPMjAyNDExMTYyMzQ2MTlaMHcwPQYKKwYBBAGE
# WQoEATEvMC0wCgIFAOriW8sCAQAwCgIBAAICJpMCAf8wBwIBAAICE80wCgIFAOrj
# rUsCAQAwNgYKKwYBBAGEWQoEAjEoMCYwDAYKKwYBBAGEWQoDAqAKMAgCAQACAweh
# IKEKMAgCAQACAwGGoDANBgkqhkiG9w0BAQsFAAOCAQEARGZG5qjuYlcjLuL+iQ4h
# tNjRlsN9LTJnihJeMfsukbVz9U23194c+nogUqxanWjTJNKBYpf0P/XA3aNwxVVt
# vxh/XfuFGMFZHJ3RZXt9Ol1WCndmAGPjeuPqzS7jSCrE4qxMop2SUIWIYCUiYtTM
# V38PX4TNtOYhHaDu5pXTPnHMw1pW56a4u7D/lbxzs532vJhsODJ8uGtes2QqN/Zz
# hWYb1W3PL2yluIv7p2R4vuF4zEKvXInvkWwvJbuD+TMayinqVdoLCMYLbDR+Twx5
# dIyPGJbGMjNlFx/XhCpzMiJur7dsIEtBA3ehD8rjJifCmQdEVRIT8L4ym/sTr2ZY
# wDGCBA0wggQJAgEBMIGTMHwxCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpXYXNoaW5n
# dG9uMRAwDgYDVQQHEwdSZWRtb25kMR4wHAYDVQQKExVNaWNyb3NvZnQgQ29ycG9y
# YXRpb24xJjAkBgNVBAMTHU1pY3Jvc29mdCBUaW1lLVN0YW1wIFBDQSAyMDEwAhMz
# AAAB+8vLbDdn5TCVAAEAAAH7MA0GCWCGSAFlAwQCAQUAoIIBSjAaBgkqhkiG9w0B
# CQMxDQYLKoZIhvcNAQkQAQQwLwYJKoZIhvcNAQkEMSIEIAES/eviU8Am+5FQTeSo
# LplUUsLZT3RmxELKnC1mvARVMIH6BgsqhkiG9w0BCRACLzGB6jCB5zCB5DCBvQQg
# OdsCq/yghZVTWIlrAi7AeKoYxGBD98R6mKg7tUkk5RgwgZgwgYCkfjB8MQswCQYD
# VQQGEwJVUzETMBEGA1UECBMKV2FzaGluZ3RvbjEQMA4GA1UEBxMHUmVkbW9uZDEe
# MBwGA1UEChMVTWljcm9zb2Z0IENvcnBvcmF0aW9uMSYwJAYDVQQDEx1NaWNyb3Nv
# ZnQgVGltZS1TdGFtcCBQQ0EgMjAxMAITMwAAAfvLy2w3Z+UwlQABAAAB+zAiBCBu
# WvDORO9tzMVz8EWgkdq8cgmCtOmWqj9MK5VX+g4tgzANBgkqhkiG9w0BAQsFAASC
# AgAcfDze0RMZNhSDVKSYoEwpSKKCW6CpARN6YHsz6kvuZVg6d637YDOYHKuq72GI
# v5NWrTmXb5ccMFISpF9D+eeVX6NXWNmEeXZMt6st/LPzEhh9kR5JTyJVYEXnlUR6
# J0+LFKvaS8/fgXEJCYm2DxkPhYyYu1ObV0myEGe7oyx0Q5TKr5yD7Ii5WtJrE8jR
# 94DQ7RFPeQWzvB3jwrzC5N892XmStmbl2Tgtci7flRZOUNmgC0o5vTbyqVyaUYmP
# I66lEnj9IyqXcZ1pjzec9ApMp+FwnjSXwIcrN4iZS4OZwGMpqbZtGTkVAg8LPUcR
# L4QhjbAV/6Zuu++HeH1mLOZtEwuagos1LraY1Rp2blYbv9668joQXN3fk9iLjYPJ
# 1qDMJyhwBe9Rn9GvI18vCl0jHEYhRfIyDCXOkQ1k/KgL/bE17h9C9x60NB/XQk9b
# 5B4wlce8W0U5LrCc2s3tqfkQ2yX+X3Tgmwcgrmh26L476TlnxvfQn4jFzRIM2AEJ
# 5B+t1Q5ASjZYyxGO3u0899leEaRhp/oBAsUEXFBZhxcJh7YvvOZr4FgCsVgojbYj
# CdApPLM+5rQCvwh5fZlrp83pv3FFU+mX+g3TSrRP78nd4F3Uh4S7GKOvGkJH7w/h
# fNglKooIb9maB2V1cLulXkngD8uFftJJ3QfP/5vkeIVRHg==
# SIG # End signature block
