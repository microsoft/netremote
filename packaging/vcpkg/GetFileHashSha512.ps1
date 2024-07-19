
param(
    [string]$Uri
)

# Download the URI content.
$UriOutFile = Join-Path -Path $Env:TEMP -ChildPath $(Split-Path $Uri -Leaf)
Invoke-WebRequest -Uri $Uri -OutFile $UriOutFile

# Calculate the SHA512 hash of the downloaded content.
$HashSha512 = Get-FileHash -Path $UriOutFile -Algorithm SHA512

# Display the hash
$HashSha512 | Select-Object -ExpandProperty Hash

Remove-Item $UriOutFile
