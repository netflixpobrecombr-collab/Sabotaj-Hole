# Sabotaj-Hole — Black Hole Studio

Migração de desenvolvimento de **SINGULARITY (Unreal Engine 5.8)** e **Sabotaj reconstruído (Unreal Engine 4.21)** para trocar ou formatar o computador de origem.

## Conteúdo já preservado

- [SINGULARITY](SINGULARITY/): código C++, configuração, mapa `Lvl_Galata`, Blueprints de FPS, materiais, malhas, atores de teste e outros assets UE5
- [Atores externos do SINGULARITY](SINGULARITY/Archives/SINGULARITY_ExternalActors_Objects.zip): ZIP de estruturas do nível (veja instruções de extração)
- [Sabotaj UE4](Sabotaj-UE4/): descritor `.uproject`, configuração e 13 scripts de tratamento de mapas, incluindo atlas real do Galata
- [HANDOFF público do SINGULARITY](SINGULARITY/HANDOFF_PUBLIC.md): resumo sanitizado, sem segredos
- [Estado da migração](MIGRATION_STATUS.md): inventário de arquivos verificados e pendências
- [Como restaurar](RESTORE_INSTRUCTIONS.md): instruções para reconstituir parte do ambiente

## AVISO MUITO IMPORTANTE

**A migração está incompleta — NÃO FORMATE O PC ORIGINAL.** Na última verificação da árvore Git havia 106 arquivos de conteúdo publicados (cerca de 4,26 MB), mas os projetos/extrações locais somavam dezenas de GiB. Faltam vários arquivos essenciais de `Content/`, `SourceAssets/`, arquivos de recuperação e recursos grandes. O repositório ainda NÃO é um backup completo nem abre os dois projetos de forma independente.

**Nenhuma chave AES ou credencial deve ser publicada em texto aberto neste repositório público.** Os arquivos originais permanecem no computador até serem guardados em um destino apropriado e uma restauração ser verificada.
