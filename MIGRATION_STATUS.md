# Estado da migração — 09/11/2026

Destino: https://github.com/netflixpobrecombr-collab/Sabotaj-Hole

## Enviado e verificado no GitHub

- `README.md`
- `SINGULARITY/SINGULARITY.uproject`
- `SINGULARITY/Source/`: 20 arquivos de código C++ e configuração de compilação do Unreal Engine 5.8
- `SINGULARITY/Config/DefaultInput.ini` e `DefaultGame.ini`
- `SINGULARITY/.gitignore` e `.gitattributes`
- `Sabotaj-UE4/Sabotaj.uproject`
- `Sabotaj-UE4/Config/DefaultInput.ini`
- `Sabotaj-UE4/Tools/`: `galata_real_atlas.py`, `galata_render_atlas.py` e `galata_palette.py`

## NÃO enviado; ainda no PC de origem

- `C:\Users\Henrique\Documents\Unreal Projects\SINGULARITY\Content`, `SourceAssets`, backups locais e histórico `.git`; há também alterações ainda não confirmadas no repositório Git local.
- `C:\Users\Henrique\Documents\Unreal Projects\Sabotaj\Content`: mapas, Blueprints, armas, personagens, Galata com seus atlas e demais assets. O projeto UE4 tinha cerca de 3,06 GiB na inspeção.
- `C:\Users\Henrique\Tools\_projetos\Sabotaj`: aproximadamente 69,01 GiB de extrações, exports, ferramentas, cenas e arquivos recuperados; contém arquivos individuais maiores que 2 GiB.
- `HANDOFF.md` completo, chaves AES, segredos, credenciais e arquivos internos de recuperação — devem permanecer fora de um repositório público não criptografado.
- Dependências e instalações das engines Unreal 4.21/5.8, plugins de editor e utilitários locais.

## IMPORTANTE

**NÃO FORMATE O PC. ESTA MIGRAÇÃO É PARCIAL E NÃO RESTAURA OS PROJETOS COMPLETOS.**

A publicação dos fontes C++ e scripts não substitui os arquivos binários de mapas e assets Unreal (`.uasset`, `.umap`), nem os arquivos recuperados. Os arquivos grandes exigem outra etapa de transferência, idealmente com volumes verificados por SHA-256 e conteúdo sensível criptografado.

Este documento não contém os valores das chaves, credenciais ou dados de autenticação.
