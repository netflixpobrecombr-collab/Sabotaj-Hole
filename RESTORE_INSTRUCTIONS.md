# Restaurar SINGULARITY UE5.8 e Sabotaj UE4.21

**Estado:** os arquivos essenciais dos **dois projetos Unreal foram publicados criptografados** e os pacotes passaram em teste de restauração no PC original em 09/11/2026.

## 1. Preparar o computador de destino

Instale **Unreal Engine 5.8** para SINGULARITY e **Unreal Engine 4.21** para o projeto Sabotaj, mais Visual Studio / compiladores C++ e plugins exigidos pelos descritores `.uproject`.

Instale Git for Windows, que inclui `openssl.exe`, e use o `tar.exe` do Windows.

## 2. Obter o backup

1. Abra a [Release UE4 + UE5](https://github.com/netflixpobrecombr-collab/Sabotaj-Hole/releases/tag/ue4-ue5-restorable-20261109).
2. Baixe os **cinco arquivos `.zip.aes256`** e o `SHA256SUMS.txt`.
3. Obtenha este repositório (clonar ou baixar ZIP). Coloque os seis arquivos baixados **na mesma pasta** que contém `RESTORE_UE4_UE5.ps1`.
4. Tenha a senha privada de restauração guardada em local seguro; **ela não está no GitHub**.
5. Opcional: o arquivo `ENCRYPTED_PRIVATE/PRIVATE_UE_RECOVERY_NOTES.zip.aes256` também é processado pelo script se permanecer na subpasta `ENCRYPTED_PRIVATE` ao lado dele.

## 3. Executar o script

No PowerShell, estando na pasta que contém o script:

```powershell
powershell.exe -ExecutionPolicy Bypass -File .\RESTORE_UE4_UE5.ps1
```

O script:
- valida o SHA-256 dos pacotes antes de qualquer extração;
- pede a senha privada sem exibi-la na tela;
- descriptografa os pacotes e extrai os dois projetos para `Documents\Unreal Projects\SINGULARITY` e `Documents\Unreal Projects\Sabotaj`;
- bloqueia sobrescrita se as pastas de destino estiverem ocupadas;
- verifica a presença dos dois arquivos `.uproject`.

Se necessário, escolha uma pasta vazia de destino:

```powershell
powershell.exe -ExecutionPolicy Bypass -File .\RESTORE_UE4_UE5.ps1 -DestinationRoot 'D:\Unreal Projects'
```

## 4. Conferir Unreal e Galata

- Abra `SINGULARITY.uproject` com UE5.8. Verifique `Content/Maps/Lvl_Galata.umap`, o mapa, materiais, malhas, Blueprints e a compilação C++.
- Abra `Sabotaj.uproject` com UE4.21. Confirme o Galata em `Content/GalataRealAtlas/`, mapas e Blueprints. Teste a jogabilidade.
- Os diretórios `Intermediate` e `DerivedDataCache` não foram incluídos porque podem ser gerados outra vez. A inicialização ou compilação inicial pode demorar.

## Integridade, segurança e limites

Os cinco arquivos somam **3,50 GB** criptografados e seus hashes são publicados pelo GitHub. Um teste local em 09/11/2026 restaurou **1.662 arquivos UE5 + 1.088 arquivos UE4**, sem diferenças de inventário ou tamanho em relação às fontes, excluindo apenas caches. O teste não incluiu rodar os projetos no editor.

**ANTES DE FORMATAR:** a senha está no PC original em `C:\Users\Henrique\Tools\_projetos\Sabotaj\_work\MigrationUE_20261109\SENHA_PRIVADA_NAO_ENVIAR.txt`. Guarde-a em outro local privado seguro. Nunca publique a senha ou chaves AES no GitHub público.

O backup cobre os dois **projetos Unreal**, não os ~69 GiB de arquivos brutos originais do Sabotaj nem instalações de software.
