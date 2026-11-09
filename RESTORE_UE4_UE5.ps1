# RESTAURAR PROJETOS UE5 + UE4 (09/11/2026)
# Baixe os 5 arquivos .aes256 e SHA256SUMS.txt da Release para a mesma pasta deste script.
# A senha de restauracao nao foi enviada ao GitHub: obtenha-a na conversa privada com ChatGPT.
param(
  [string]$DestinationRoot = (Join-Path $env:USERPROFILE 'Documents\Unreal Projects')
)
$ErrorActionPreference = 'Stop'
$Here = Split-Path -Parent $MyInvocation.MyCommand.Path
$Bundles = @(
  @{ Name='SINGULARITY_UE58_Project.zip.aes256'; Project='SINGULARITY'; Sha='9FC6BBA7AD912BD33F999D0E8C8577A7DA22B7D638CA3226129E2912E0EFE89F' },
  @{ Name='SINGULARITY_UE58_Saved.zip.aes256'; Project='SINGULARITY'; Sha='27FAB593D7A859B656438595E05C73D4E03D6D84A26A375A6AC3606A95561B29' },
  @{ Name='SINGULARITY_UE58_Binaries.zip.aes256'; Project='SINGULARITY'; Sha='5A6B7D50C914E05A753BD6B7C578DC1018845594061D6A25F1D4598AD48B4770' },
  @{ Name='Sabotaj_UE421_Galata_Project.zip.aes256'; Project='Sabotaj'; Sha='5880994B9E183E4B213128376D6AEC76C6D0635CA7478282C77F358C28ACD61B' },
  @{ Name='Sabotaj_UE421_Saved.zip.aes256'; Project='Sabotaj'; Sha='E016374F762A56FBEDBF90BADFBD6C2876FA7350F92B2662AD7D82E008D90690' }
)
$PrivateRelative = 'ENCRYPTED_PRIVATE\PRIVATE_UE_RECOVERY_NOTES.zip.aes256'
if (Test-Path -LiteralPath (Join-Path $Here $PrivateRelative) -PathType Leaf) {
  $Bundles += @{ Name=$PrivateRelative; Project='Sabotaj_Recovery_Notes'; Sha='3ACFE59FAE77E99743449D18C9F4B917CAAF5A6A7E6DD29BDD0B899183FA816A' }
  Write-Host 'Arquivo de anotacoes privadas encontrado: sera restaurado separadamente.'
}
$OpenSSL = (Get-Command openssl.exe -ErrorAction SilentlyContinue | Select-Object -First 1 -ExpandProperty Source)
if (-not $OpenSSL) {
  $GitSSL = 'C:\Program Files\Git\mingw64\bin\openssl.exe'
  if (Test-Path $GitSSL) { $OpenSSL = $GitSSL }
}
if (-not $OpenSSL) { throw 'OpenSSL nao encontrado. Instale Git for Windows (que inclui OpenSSL) antes de restaurar.' }
if (-not (Get-Command tar.exe -ErrorAction SilentlyContinue)) { throw 'tar.exe nao encontrado no Windows.' }
Write-Host 'Verificando integridade dos cinco pacotes criptografados...'
foreach ($Bundle in $Bundles) {
  $Path = Join-Path $Here $Bundle.Name
  if (-not (Test-Path -LiteralPath $Path -PathType Leaf)) { throw ('Falta o arquivo: '+$Bundle.Name) }
  $Actual = (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash
  if ($Actual -ne $Bundle.Sha) { throw ('Falha SHA-256: '+$Bundle.Name) }
  Write-Host ('OK: '+$Bundle.Name)
}
foreach ($Name in @('SINGULARITY','Sabotaj','Sabotaj_Recovery_Notes')) {
  $Dir = Join-Path $DestinationRoot $Name
  if ((Test-Path $Dir) -and @(Get-ChildItem -LiteralPath $Dir -Force -ErrorAction SilentlyContinue).Count -gt 0) {
    throw ('A pasta destino ja existe e nao esta vazia: '+$Dir+'. Selecione outra raiz com -DestinationRoot.')
  }
}
$Secure = Read-Host 'Digite a senha privada de restauracao (NAO e a chave AES do Sabotaj)' -AsSecureString
$Bstr = [Runtime.InteropServices.Marshal]::SecureStringToBSTR($Secure)
try { $Password = [Runtime.InteropServices.Marshal]::PtrToStringBSTR($Bstr) }
finally { [Runtime.InteropServices.Marshal]::ZeroFreeBSTR($Bstr) }
if (-not $Password) { throw 'Senha vazia.' }
$TemporaryKey = [IO.Path]::GetTempFileName()
try {
  [IO.File]::WriteAllText($TemporaryKey,$Password,[Text.Encoding]::ASCII)
  $Password = $null
  foreach ($Bundle in $Bundles) {
    $Target = Join-Path $DestinationRoot $Bundle.Project
    New-Item -ItemType Directory -Path $Target -Force | Out-Null
    $Encrypted = Join-Path $Here $Bundle.Name
    $Zip = Join-Path $env:TEMP ('mig_ue_'+[Guid]::NewGuid().ToString('N')+'.zip')
    try {
      Write-Host ('Descriptografando '+$Bundle.Name+'...')
      & $OpenSSL enc -d -aes-256-cbc -salt -pbkdf2 -iter 250000 -md sha256 -pass ('file:'+$TemporaryKey) -in $Encrypted -out $Zip
      if ($LASTEXITCODE -ne 0) { throw ('Senha incorreta ou pacote invalido: '+$Bundle.Name) }
      & tar.exe -xf $Zip -C $Target
      if ($LASTEXITCODE -ne 0) { throw ('Falha ao extrair: '+$Bundle.Name) }
      Write-Host ('Extraido em: '+$Target)
    }
    finally { Remove-Item -LiteralPath $Zip -Force -ErrorAction SilentlyContinue }
  }
}
finally { Remove-Item -LiteralPath $TemporaryKey -Force -ErrorAction SilentlyContinue }
if (-not (Test-Path (Join-Path $DestinationRoot 'SINGULARITY\SINGULARITY.uproject'))) { throw 'SINGULARITY.uproject ausente apos restauracao' }
if (-not (Test-Path (Join-Path $DestinationRoot 'Sabotaj\Sabotaj.uproject'))) { throw 'Sabotaj.uproject ausente apos restauracao' }
Write-Host 'ARQUIVOS RESTAURADOS. Abra os projetos com Unreal Engine 5.8 e Unreal Engine 4.21.'
Write-Host 'As pastas Intermediate e DerivedDataCache foram omitidas porque sao caches regeneraveis.'
