# Sabotaj-Hole — Black Hole Studio

Backup e migração dos projetos **SINGULARITY (Unreal Engine 5.8)** e **Sabotaj reconstruído (Unreal Engine 4.21)**.

## Backup integral dos dois projetos

Os recursos de desenvolvimento dos dois projetos estão publicados **criptografados** na [Release UE4 + UE5 de 09/11/2026](https://github.com/netflixpobrecombr-collab/Sabotaj-Hole/releases/tag/ue4-ue5-restorable-20261109), com cinco pacotes somando **3.504.788.880 bytes (3,50 GB)**.

- UE5: projeto com `Content`, `Source`, `SourceAssets`, `Scripts`, `Config`, `Build`, `Reports`, `Backups`, `.git` local e `HANDOFF.md`; mais `Saved` e `Binaries` em pacotes separados.
- UE4: `Sabotaj.uproject`, `Config` e `Content` (incluindo o Galata texturizado); mais `Saved` em pacote separado.
- As únicas pastas omitidas foram `Intermediate` e `DerivedDataCache` (caches regeneráveis).

**Validação de 09/11/2026:** os cinco pacotes criptografados têm hashes SHA-256 idênticos aos publicados pelo GitHub. Foram descriptografados e extraídos com sucesso no PC original; **2.750 arquivos** restaurados foram comparados com o inventário original, sem ausências nem diferenças de tamanho. Arquivos essenciais também tiveram o SHA-256 comparado individualmente.

**Importante:** essa foi uma validação de backup/restauração de arquivos. Abrir os projetos e testar a jogabilidade em um editor Unreal instalado ainda é uma verificação adicional recomendada.

## Conteúdo acessível diretamente no Git

- [SINGULARITY](SINGULARITY/): parte do código C++, configurações e recursos individuais
- [Atores externos do SINGULARITY](SINGULARITY/Archives/SINGULARITY_ExternalActors_Objects.zip)
- [Esqueleto e scripts do UE4](Sabotaj-UE4/)
- [Estado e verificação da migração](MIGRATION_STATUS.md)
- [Instruções de restauração](RESTORE_INSTRUCTIONS.md)
- [Script de recuperação](RESTORE_UE4_UE5.ps1)

## Antes de formatar o PC de origem

**Guarde a senha privada da restauração FORA do computador que será formatado.** Ela não foi publicada no repositório nem incluída nos pacotes. No PC de origem, está em `C:\Users\Henrique\Tools\_projetos\Sabotaj\_work\MigrationUE_20261109\SENHA_PRIVADA_NAO_ENVIAR.txt`. Transfira-a para um gerenciador de senhas ou outro local pessoal seguro. Sem ela, não será possível abrir os arquivos criptografados.

O repositório é público: **não publique a senha, chaves AES ou credenciais em texto aberto.** O backup **não inclui** os ~69 GiB de arquivos brutos e instalações originais de Sabotaj mantidos fora dos dois projetos Unreal.
