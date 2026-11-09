# Estado da migração — 09/11/2026

Destino: https://github.com/netflixpobrecombr-collab/Sabotaj-Hole

## Projetos Unreal: pacotes publicados e restauração de teste validada

**Release:** https://github.com/netflixpobrecombr-collab/Sabotaj-Hole/releases/tag/ue4-ue5-restorable-20261109

| Pacote criptografado | Bytes |
| --- | ---: |
| SINGULARITY_UE58_Project.zip.aes256 | 761.802.432 |
| SINGULARITY_UE58_Saved.zip.aes256 | 892.784.592 |
| SINGULARITY_UE58_Binaries.zip.aes256 | 254.087.296 |
| Sabotaj_UE421_Galata_Project.zip.aes256 | 1.150.403.616 |
| Sabotaj_UE421_Saved.zip.aes256 | 445.710.944 |
| **Total** | **3.504.788.880** |

O GitHub informa os hashes SHA-256 de cada pacote. **Todos os cinco hashes foram comparados ao arquivo original no PC e corresponderam.**

### O que foi preservado

- **SINGULARITY (UE5.8):** `SINGULARITY.uproject`, `Content/`, `Source/`, `SourceAssets/`, `Scripts/`, `Build/`, `Config/`, `Backups/`, `Reports/`, `Saved/`, `Binaries/`, histórico local `.git/`, `HANDOFF.md` e outros arquivos da raiz. Os recursos do Galata e das armas dentro dessas pastas foram incluídos.
- **Sabotaj (UE4.21):** `Sabotaj.uproject`, `Content/` (incluindo `GalataRealAtlas/Galata_textured.uasset`, mapas, Blueprints e outros recursos), `Config/` e `Saved/`.
- **Recuperação privada:** `ENCRYPTED_PRIVATE/PRIVATE_UE_RECOVERY_NOTES.zip.aes256` no Git; criptografado. Senha não publicada.

Somente os arquivos dentro de `Intermediate/` e `DerivedDataCache/` foram omitidos dos projetos, pois são geráveis novamente.

### Teste efetivo de restauração no PC original

Em 09/11/2026:
1. Os cinco pacotes foram descriptografados com a senha privada e comparados byte a byte aos ZIPs de origem pelo SHA-256.
2. Os cinco ZIPs foram extraídos em pasta de teste independente.
3. O inventário de arquivos restaurados foi comparado ao inventário original, excluindo apenas os caches especificados:
   - UE5: **1.662 esperados / 1.662 restaurados / 0 ausentes / 0 tamanhos divergentes**.
   - UE4: **1.088 esperados / 1.088 restaurados / 0 ausentes / 0 tamanhos divergentes**.
4. Sete arquivos essenciais tiveram hashes SHA-256 iguais aos correspondentes originais, incluindo os arquivos `.uproject`, `Lvl_Galata.umap`, `Galata_textured.uasset` e `Galata_textured.fbx`.
5. A pasta temporária de teste foi removida após validação; o relatório local está em `MigrationUE_20261109/VALIDATION_REPORT.txt`.

**Limite do teste:** não houve confirmação de abertura e jogabilidade dos dois projetos nos respectivos Unreal Editors em um PC novo. A restauração de arquivos foi verificada.

### Código Git e arquivos menores

Na verificação mais recente, o ramo `main` continha 110 arquivos Git de documentação, scripts, código e parte de `Content/`. Os recursos completos estão na **Release criptografada**, e não como arquivos individuais na árvore Git.

### Requisito antes de formatar

**A senha de restauração está somente no PC original até ser salva em outro local seguro.** Caminho:

`C:\Users\Henrique\Tools\_projetos\Sabotaj\_work\MigrationUE_20261109\SENHA_PRIVADA_NAO_ENVIAR.txt`

**Não formate até salvar essa senha fora do computador.** Recomenda-se também testar a abertura dos projetos em Unreal Engine 5.8 e 4.21 antes de apagar a fonte.

Os aproximadamente 69 GiB de arquivos brutos e instalações do Sabotaj **não** estão inclusos neste backup, conforme o escopo restrito aos dois projetos Unreal.
