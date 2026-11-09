# Estado da migração — 09/11/2026

Destino: https://github.com/netflixpobrecombr-collab/Sabotaj-Hole

## Transferência REAL e verificada

**Checkpoint do repositório:** commit `a742c426f7abe8a3ea31f82d7db1e694523492c9`.
Na conferência da árvore remota, havia **106 arquivos de conteúdo Git**, somando **4.256.659 bytes** de payload (não contar metadados Git). A migração segue PARCIAL: esses megabytes não substituem os aproximadamente 80 GiB encontrados no computador.

### SINGULARITY / Unreal Engine 5.8

- `SINGULARITY/SINGULARITY.uproject`
- `SINGULARITY/Source/`: 20 arquivos C++ e de compilação, incluindo alterações que estavam na árvore de trabalho local
- `SINGULARITY/Config/`: cinco arquivos de configuração
- `SINGULARITY/Content/`: **55 arquivos individuais**, incluindo mapa `Maps/Lvl_Galata.umap`, ações Enhanced Input, Blueprint do personagem, PlayerController, GameMode, câmera, animações, materiais, shaders, malhas, protótipos interativos e controles touch
- `SINGULARITY/Archives/SINGULARITY_ExternalActors_Objects.zip`: ZIP de 141.713 bytes com **64 arquivos** do conteúdo de atores/objetos externos (84 entradas incluindo pastas). SHA-256: `FAE2CF470F7ECC6959BDD19AC6E2C58D453CBC723DA6F9C77A8EDCA2FCE4FFBF`
- `SINGULARITY/HANDOFF_PUBLIC.md`: versão de referência sanitizada do handoff; **não** é a cópia local completa

Os arquivos binários enviados foram transformados em blobs Git com conteúdo exato; seus SHA de blob foram comparados aos hashes `git hash-object` locais antes de criar os commits.

### Sabotaj / Unreal Engine 4.21

- `Sabotaj-UE4/Sabotaj.uproject`
- `Sabotaj-UE4/Config/`: quatro arquivos de configuração
- `Sabotaj-UE4/Tools/`: 13 scripts de recuperação, exportação e construção de mapas, inclusive o atlas real do Galata
- Não foram publicados mapas/meshes/texturas recuperados nem a configuração secreta do jogo original.

## AINDA NÃO enviado — permanece somente no PC

- **UE5 SINGULARITY:** muitos dos assets do `Content/` (incluindo Mannequins, Galata texturizado, referências do protótipo); a pasta `SourceAssets/` com FBX e imagens; backups locais e histórico completo de `.git`; caches e binários de build (os últimos geralmente são regeneráveis).
- **UE4 Sabotaj:** diretório `Content/` (cerca de 1,64 GiB de mapas, Blueprints, malhas, armas e materiais). O mapa real-texturizado `Galata_textured.uasset` e o `FirstPersonExampleMap.umap` jogável **não estão aqui**.
- **Recuperação do Sabotaj:** diretório `C:\Users\Henrique\Tools\_projetos\Sabotaj` de cerca de **69,01 GiB**, incluindo assets extraídos, exports, `.pak`, `.rar`, projetos auxiliares e ferramentas. Há arquivos individuais superiores aos limites típicos de arquivos em repositórios Git.
- **Segurança:** `HANDOFF.md` completo com referências de recuperação, arquivos de chave AES e credenciais não foram enviados ao GitHub público.
- **Ambiente:** instalações locais do Unreal Engine 4.21 / 5.8, plugins, programas e toolchains não foram migrados.

## RESTAURAÇÃO

Veja `RESTORE_INSTRUCTIONS.md` para os passos de extração e implantação parcial no novo PC.

> **NÃO FORMATE O COMPUTADOR AINDA.** O repositório é uma cópia verificada de alguns arquivos e NÃO é uma restauração completa dos projetos. Para não perder mapas e dados secretos, os recursos ausentes precisam ser guardados em um destino externo seguro e uma restauração real precisa ser testada.

Este arquivo contém metadados de migração; nenhum segredo, senha, token ou chave AES.
