# HANDOFF — versão pública sanitizada

**Este documento é uma cópia parcial de referência.** Trechos contendo referências a chaves, credenciais ou segredos foram omitidos. A cópia local original do projeto deve ser preservada em armazenamento seguro fora do repositório público. **A migração ainda não está concluída e não é seguro formatar o computador.**

# SINGULARITY — Black Hole Studio — HANDOFF

Atualização: 2026-11-06 (BRT), Milestone 1 — primeira cena UE5.

## Identidade
- **Jogo:** SINGULARITY.
- **Empresa/estúdio:** Black Hole Studio (singular).
- **Objetivo:** FPS tático próprio, inspirado no estilo de Valorant; não copiar o jogo nem extrair seus arquivos.
- **Projeto de recuperação Sabotaj/UE4.21:** é separado, NÃO alterar ou apagar arquivos originais.

## Localização e ferramenta
- Unreal Engine 5.8.4: `C:\Program Files\Epic Games\UE_5.8\`
- Projeto novo: `%USERPROFILE%\Documents\Unreal Projects\SINGULARITY\SINGULARITY.uproject`
- Template: First Person Blueprint (`TP_FirstPersonBP`), com conteúdo compartilhado Characters, Input, LevelPrototyping.
- Plugins habilitados no `.uproject`: EnhancedInput, Niagara, ControlRig, ModelingToolsEditorMode, GameplayStateTree, PythonScriptPlugin. Já acompanham o UE5, não reinstalar.
- Visual Studio Community 2022 + suporte C++ Unreal já instalado e validado.

## Milestone 1: feito
1. Projeto SINGULARITY criado e validado pelo UnrealEditor-Cmd.exe com Python Script.
2. Fontes FBX/PNG copiadas, sem alterar as originais, para `SourceAssets\Galata`.
3. Galata importado via script `Scripts\import_galata.py`; relatório `Reports\galata_import.json` (stage=imported). **9 assets** em `/Game/Galata/`: 1 mesh, 4 atlas de textura e 4 instâncias de material.
4. Materiais inspecionados em `Reports\galata_materials.json`. Cada instância M_Galata_RealAtlas_00..03 possui parâmetro `DiffuseColorMap` apontando para o respectivo atlas.
5. Mapa `/Game/Maps/Lvl_Galata` criado/salvo com 8 atores: malha, DirectionalLight, SkyAtmosphere, SkyLight, ExponentialHeightFog, piso de colisão provisório oculto no jogo, PlayerStart e câmera `Galata_Preview_Camera`. Relatório: `Reports\galata_level.json` (stage=saved).
6. `Config\DefaultEngine.ini`: EditorStartupMap e GameDefaultMap apontam para `/Game/Maps/Lvl_Galata.Lvl_Galata`. Template configurado com `r.DynamicGlobalIlluminationMethod=1` e `r.ReflectionMethod=1`.
7. Projeto abriu na GUI real do Unreal Editor; janela `SINGULARITY - Unreal Editor` confirmada.
8. Captura real do viewport UE5, **SEM LUMEN**, `Reports\Galata_UE5_preview_DX11.png` e cópia `%USERPROFILE%\Tools\_projetos\Sabotaj\export\Maps_preview\Galata_UE5_preview_DX11.png`.

## Limitações e pendências honestas
- **GPU Intel Iris Xe:** ao abrir, UE5 selecionou D3D11/SM5. Portanto Lumen completo **não foi confirmado/não está ativo neste modo**, apesar das configurações solicitarem GI/reflections Lumen. Nunca chamar a prévia DX11 de render Lumen. Não foi produzido `Galata_UE5_lumen.png`.
- Prévia inicial ficou acinzentada. Os parâmetros de textura estão ligados corretamente, mas visual, exposição/tonalidade/PBR ainda precisam de avaliação e melhorias.
- Colisão do FBX estava desativada para evitar importação pesada. O piso provisório foi posicionado automaticamente próximo ao limite inferior do bounding box; **reposicionar corretamente na rua** (o UE4 tinha referência piso ~[0,2530,380] e PlayerStart acima). Percurso/jogabilidade não validados.
- Cena contém somente o recorte central do Galata (não a torre, nem o mapa completo).
- Screenshot de alta resolução via AutomationLibrary foi solicitado no script `Scripts\render_galata.py`, mas nenhum arquivo correspondente foi observado; não afirmar que existe.
- Salvar backup/snapshot antes de alterar ativos pesados e não executar 2 editores concorrentes gravando os mesmos assets.

## Próximas ações
1. Verificar no editor o aspecto real dos materiais, considerar criar materiais PBR próprios com BaseColor ligado aos 4 atlas se necessário. Trabalhar em cópias/novos assets para não perder o import original.
2. Reposicionar o piso de colisão e spawn para uma área caminhável, depois testar PIE em primeira pessoa.
3. Melhorar céu, exposição, direção solar e enquadramento de câmera, preservando versão anterior.
4. Capturar screenshot da versão aprimorada. Para Lumen real, testar placa gráfica/ambiente compatível com DX12/SM6.
5. Depois iniciar Milestone 2 (controles/AK74) e futura identidade/HUD original. Nunca tocar Steam/EAC nem copiar código/assets do Valorant.

## Scripts e relatórios
- `Scripts\probe_unreal.py`, `Scripts\import_galata.py`, `Scripts\inspect_galata.py`, `Scripts\inspect_materials.py`, `Scripts\create_galata_level.py`, `Scripts\render_galata.py`.
- `Reports\ue_probe.json`, `Reports\galata_import.json`, `Reports\galata_level.json`, `Reports\galata_materials.json`, `Reports\Galata_UE5_preview_DX11.png`.

---

## META PERMANENTE DO PROJETO — DIRETRIZ DO CRIADOR (2026-11-06)

**Missão principal:** desenvolver e concluir o **SINGULARITY**, FPS tático original da **Black Hole Studio**, até atingir funcionamento completo dentro de um escopo progressivamente validado. Não encerrar o trabalho apenas porque um editor abriu, um asset foi importado ou um render foi produzido. Cada milestone deve aproximar o jogo de uma versão de fato utilizável.

**Modo de trabalho: automation-first / mínima intervenção humana.**
- Preferir executar diretamente tarefas técnicas autorizadas usando terminal, Unreal Python, C++/Blueprints, scripts, Git e Remote Desktop Commander, quando conectados e disponíveis.
- Fazer autonomamente inspeções, setup, importação, configuração, implementação, testes, correções, capturas e documentação sempre que viável; não transferir passos manuais evitáveis ao usuário.
- Antes de instalar algo, verificar o que já existe. Antes de modificar, conferir histórico e caminhos. Antes de declarar conclusão, validar resultado com evidências.
- Preservar backups e o projeto UE4 Sabotaj; não sobrescrever ativos originais, não apagar arquivos em massa, não modificar sistemas externos sem relação, não publicar, gastar dinheiro ou comprometer licenças sem autorização apropriada.
- Manter o próprio desenvolvimento separado de referências: Valorant é inspiração; ativos de terceiros, inclusive do Sabotaj, exigem direitos de uso antes de distribuição pública. Trabalhar rumo a conteúdo autoral/licenciado.
- Registrar ao final de cada sessão: alterações reais, erros, estado dos testes, evidências, pendências e PRÓXIMA AÇÃO EXATA neste HANDOFF. Assim qualquer agente pode retomar sem recomeçar nem perder contexto.
- Sem execução contínua automática fora de uma sessão ou automação explicitamente configurada; não alegar trabalho em segundo plano sem de fato configurá-lo.

### Roteiro mestre por critérios de conclusão
- **M1 — Fundação técnica UE5 / Galata:** abrir e salvar projeto, ativar plugins, importar cena com materiais, resolver aparência e colisão, validar spawn e caminhada, gerar prévia real adequada ao hardware.
- **M2 — Vertical slice FPS:** movimentação, mira, tiro/recuo, ao menos uma arma funcional com animações e efeitos, alvos e dano, áudio e HUD de vida/munição, build local jogável sem erros críticos.
- **M3 — Partida funcional:** regras originais de FPS tático, equipes, rodadas, objetivos, estados de vitória/derrota, menu, configurações, bots/treino e testes repetíveis.
[Trecho com possível informação sensível omitido da versão pública.]
- **M5 — Qualidade e segurança:** arte/iluminação própria ou devidamente licenciada, UX, acessibilidade, áudio, otimização para hardware-alvo, detecção/mitigação anti-cheat defensiva e testes de estabilidade.
- **M6 — Distribuição validada:** packaging, testes de instalação em máquina limpa, desempenho, persistência de configurações, documentação, correções finais e lançamento autorizado.

**Definição de pronto:** um marco só está concluído com execução real, teste reproduzível e evidência (log/captura/build); nunca confundir scripts preparados com funcionalidade testada. Validar em hardware compatível quando houver dependência, especialmente DX12/SM6/Lumen.

**Prioridade imediata (próxima sessão):** concluir pendências M1 do HANDOFF acima: materiais/colorização, piso/colisão/spawn realmente caminháveis, gameplay Play-in-Editor, captura final UE5 apropriada (sem chamar DX11 de Lumen).

---

## EXECUÇÃO DE CONTINUIDADE — 2026-11-07 (BRT, sessão ChatGPT)

### Estado encontrado antes de modificar
- Além dos assets descritos acima, o projeto **já continha** C++ compilado em `Binaries\Win64\UnrealEditor-SINGULARITY.dll` e fontes `Source\SINGULARITY\SGCombatSystem`, `SGHUD`, `SGGameMode`, `SGTargetDummy`.
- O nível já continha atores originais `SG_CombatSystem`, `SG_Target_Center`, `SG_Target_Left`, `SG_Target_Right`, não registrados no HANDOFF anterior.
- `Config\DefaultEngine.ini` já estava apontando o GameMode para `/Script/SINGULARITY.SGGameMode`. Evitar recriá-los.

### Operações executadas e validadas
1. Salvou cópia de segurança pré-alteração do mapa em `Reports\Lvl_Galata_before_gameplay_20261107.umap`.
2. `Scripts\milestone_gameplay.py` reposicionou `Invisible_Player_Floor` para `(0,2530,350)` e `PlayerStart_SINGULARITY` para `(0,2530,500)`. Ele criou três atores de treino adicionais antes de descobrir os atores existentes; **esses três foram removidos** por `Scripts\deduplicate_training.py`, sem mexer nos atores originais. Relatório `Reports\deduplicate_training.json`: stage=saved.
3. `Scripts\expand_training_floor.py`: piso com escala `(22,14,0.25)` — cobre o conjunto de alvos existente em X=600–750; relatório `Reports\training_floor.json`: stage=saved.
4. TESTE REAL de inicialização do jogo pelo `UnrealEditor.exe` com `-game -NullRHI -ExecCmds=quit`: concluiu com código 0. Logs registraram `SINGULARITY_COMBAT_READY Ammo=30` e `SINGULARITY_GAMEMODE_READY HUD=SGHUD Pawn=BP_FirstPersonCharacter_C`. O nível subiu para execução. **Não** é evidência ainda de movimentação, colisão, acertos ou rendering correto.
5. O projeto original Sabotaj/UE4 não foi modificado nesta sessão.

### Pendente — execução seguinte exata
- Testar caminhada/colisão em PIE real (com input) no piso `(0,2530,350)`, spawn `(0,2530,500)`; corrigir qualquer queda ou obstrução.
- Testar tiro LMB, recarga R, hit/dano e eliminação dos alvos `SG_Target_Center/Left/Right`; criar automação de teste se possível.
- Diagnosticar visual acinzentado e gerar captura definitiva no modo gráfico suportado sem alegar Lumen em D3D11.
- Não duplicar combat system e dummies; não sobrescrever o mapa backup.
- Ajustar o roteiro M2 a partir dos C++ existentes, evitando repetir trabalho.

- Verificação adicional: `Engine\Build\BatchFiles\Build.bat SINGULARITYEditor Win64 Development ... -WaitMutex -NoHotReload` retornou **Result: Succeeded / Target is up to date**, sem necessidade de recompilar ações. C++/UnrealBuildTool validado contra configuração atual.

### Investigação adicional de colisão / atlas (mesma sessão)
- Os quatro atlas reais foram verificados visualmente, dimensões 4096×4096 cada; mosaico de diagnóstico: `Reports\Galata_atlas_contact_sheet.png`. Possuem texturas reais (tons cinza, madeira, tijolos, janelas); não foram perdidos.
- `Scripts\collision_inspection.py` e `Reports\collision_inspection.json` confirmaram: `Invisible_Player_Floor` em (0,2530,350), escala (22,14,0.25), malha `/Engine/BasicShapes/Cube`, `CollisionEnabled.QUERY_AND_PHYSICS`, perfil `BlockAll`; `Galata_RealAtlas` também com perfil `BlockAll`. Ponto inicial em (0,2530,500).
- Teste de traçado `Scripts\validate_floor_trace.py` ainda **não validado**: tentativa de ler `HitResult` no mundo do editor com NullRHI falhou por incompatibilidades de wrapper Python, e inspeção preliminar de `HitResult` retornou estrutura vazia. Não inferir colisão real a partir disso; validar em PIE/runtime.
- Compilação incremental: **SUCCESS**, target up to date. Teste de inicialização -game: **SUCCESS** com registro de GameMode e sistema de combate. **Tiros e movimentação ainda não testados de ponta a ponta.**
- Próximo passo: executar PIE com captura e controles reais / criar teste funcional para validar movimentação, raycast, dano; em seguida revisar exposição e materiais de perto. Não criar novo sistema C++ de combate; ele já existe e inicializa.

---

## CONTINUIDADE AUTOMATIZADA — 2026-11-07 (manhã, BRT)

### Resultado comprovado (M1 / início M2)
- **Primeiro teste funcional end-to-end em runtime Unreal** usando um novo ator C++ `ASGSmokeTestActor`, ativado **exclusivamente** com flag de comando `-SGSmokeTest`. Em partidas normais ele não é criado. Arquivos: `Source/SINGULARITY/SGSmokeTestActor.h/.cpp`, integração mínima em `SGGameMode.cpp`. O `SGGameMode.cpp` original está preservado em `Reports/SGGameMode_backup_smoke_20261107.cpp`.
- Compilado com Build.bat Unreal Engine 5.8.4 / MSVC 2022: **Result Succeeded**.
- Rodou no nível Galata com `-game -NullRHI -SGSmokeTest` e saída real `SG_SMOKE_END Passed=12 Failed=0`. Validados spawn, existência de combate/alvos, chão detectável, tiro hitscan, redução de munição, dano ao alvo, recarga, dano direto, eliminação do alvo, desativação de colisão ao eliminar, deslocamento e permanência no chão. Deslocamento ~596 cm sem queda; munição 30→29; primeiro hit tirou 34 HP; alvo chegou a HP zero.
- Automação reutilizável: `Scripts/run_smoke_test.ps1`. Versão testada com sucesso, gera `Reports/smoke_latest.json` (Success True, Passed 12) e `Reports/smoke_latest.log`. Não depende de clicks manuais; não executar com outro UnrealEditor ativo.
- **Escopo do teste:** a chamada C++ de disparo foi automatizada; a resposta a teclas reais LMB/R pelo sistema de input e o HUD visual ainda exigem validação gráfica. O teste roda sem GPU (NullRHI), portanto não valida performance, shader ou Lumen.

### Arte/iluminação e material do Galata
- Backup do mapa antes de alterações visuais em `Reports/Lvl_Galata_before_visual_M1_20261107.umap`; outro backup intermediário `Reports/Lvl_Galata_after_PBR_v1_backup.umap`; backup antes de estilizar os alvos `Reports/Lvl_Galata_before_targets_20261107.umap`.
- `Scripts/improve_galata_materials.py`: criou quatro materiais PBR próprios `/Game/Singularity/Materials/M_SG_GalataAtlas_00..03`, com BaseColor conectado aos quatro atlas originais e roughness=0.83; aplicados **ao ator da cena**, preservando materiais importados e FBX originais. Relatório: `Reports/galata_pbr_report.json` (saved).
- `Scripts/lighting_and_street_camera_v2.py`: reajustou sol a intensidade 2.2, Skylight móvel em 1.4 com captura realtime, neblina 0.0012, preservou câmera aérea e adicionou `Galata_StreetPreview_Camera` (80,2530,580). Relatório `Reports/lighting_v2.json` (saved).
- Capturas reais de viewport UE5 (DX11/SM5) em `Reports/Galata_UE5_PBR_M1_v1.png` (aérea) e `Reports/Galata_UE5_M1_street_v2.png` (nível da rua). Mais detalhes visíveis; ainda existem sombras muito fortes e superfícies/vegetação de aspecto bruto. **Não é render Lumen**.
- `Scripts/style_targets.py`: criou `M_SG_Target_Neon` (material original escuro/ciano com Fresnel emissivo) e aplicou aos três atores existentes `SG_Target_Center/Left/Right`. Relatório `Reports/target_style.json` (saved). Smoke test **12/12 PASS** foi repetido após a alteração.
- `Scripts/focus_editor_galata.py` e `Scripts/focus_editor_street.py` posicionam a câmera de visualização do editor.
- Sem alterações nos arquivos do Sabotaj/UE4. Atenção aos direitos dos assets recuperados: uso interno de protótipo; não distribuir publicamente sem confirmar licenças/direitos.

### Próximas etapas exatas
1. Capturar a perspectiva de rua **após** aplicar material neon nos alvos; revisar se o visual ficou legível.
2. Verificar gameplay com input real em PIE/janela; checar teclas de movimentação, LMB/R e HUD na tela.
3. Validar build empacotado de desenvolvimento Windows e desempenho no hardware disponível, mantendo alternativas sem Lumen para Intel Iris Xe.
4. Planejar uma primeira arma e animações originais/licenciadas e o fluxo de rodada do FPS tático. Não duplicar os sistemas de combate C++ existentes.

---

## VALIDACAO DE BUILD WINDOWS — 2026-11-07, ~08:08 BRT

### Build / packaging realmente concluídos
- Foi corrigida incompatibilidade **editor-only** no smoke test: `GetActorLabel()` não existe em configuração Game Windows. Em `Source/SINGULARITY/SGSmokeTestActor.cpp`, seleção do primeiro `ASGTargetDummy` via iterator, sem APIs editor-only.
- Depois da correção, `Build.bat SINGULARITY Win64 Development` retornou **Result: Succeeded**; gerou `Binaries/Win64/SINGULARITY.exe` (~332 MB, não é pacote autossuficiente).
- Com `RunUAT.bat BuildCookRun` (`-build -cook -stage -pak -package -archive`), Cooking/Stage/Package/Archive concluíram com **BUILD SUCCESSFUL, ExitCode=0**. O Zen Storage Server local na porta 8558 apresentou falha temporária de conexão IPv6, mas se recuperou automaticamente durante as tentativas; nenhum cache apagado.
- **Build local de desenvolvimento completa** (NÃO publicar ainda): `%USERPROFILE%\Documents\Unreal Projects\SINGULARITY_Builds\M1_Development\Windows\SINGULARITY.exe`. Pasta Windows com ~48 arquivos, total ~0,99 GB. O pacote está fora do repositório Git; não foi enviado a nenhum serviço.
- **Teste no executável empacotado:** `Windows/SINGULARITY.exe -NullRHI -SGSmokeTest -log` executado no PC, log em `Windows\SINGULARITY\Saved\Logs\SINGULARITY.log`; saída real `SG_SMOKE_END Passed=12 Failed=0`; processo terminou normalmente.
- **Teste gráfico empacotado:** `Windows/SINGULARITY.exe -d3d11 -windowed -ResX=1280 -ResY=720 -log` iniciou na Intel Iris Xe, processo `SINGULARITY (64-bit Development PCD3D_SM5)`, janela respondendo, GameMode/Combat inicializados, mapa Galata e HUD visíveis. Captura real: `Reports/SINGULARITY_Packaged_Windows_M1.png`.
- **Limitações visuais vistas na captura:** estrutura preta horizontal atravessando parte do campo de visão perto do spawn; bonecos de treino ainda são blocos; sombras muito fortes; pós-processamento, vegetação e geometria do recorte recuperado não finalizados. Nem Lumen nem FPS competitivo foram validados.
- **Direitos:** assets do Galata recuperado destinam-se exclusivamente ao protótipo local até confirmação de licenciamento/autorização; nenhuma publicação.

### Git local / resiliência
- Repositório Git criado localmente no projeto na branch `main`, Git LFS configurado para .uasset/.umap/.fbx/.png, gerados ignorados (.gitignore).
- Primeiro checkpoint `72c7dc6`: `M1: UE5 Galata playable training baseline and automated smoke tests`. Não existe remote configurado/nenhum push feito.
- A correção do smoke-test para o target Game e a captura da build Windows foram registradas em um segundo commit local (`f694ac1`), ainda sem remote ou push.

### Próxima ação exata
1. Corrigir obstrução visual perto do PlayerStart sem quebrar os 12 testes; tirar nova captura.
2. Testar input real LMB, R, W/A/S/D e HUD de ponta a ponta na versão gráfica.
3. Introduzir arma 3D própria/licenciada, efeitos e áudio, melhorar apresentação visual e otimizar recorte para a Intel Iris Xe.
4. Desenvolver regras de rodada/equipes/objetivo e depois multiplayer servidor-autoritativo. Repetir smoke test e embalagem de Windows por milestone.

---

## CONTINUIDADE EXECUTADA — 2026-11-07, manhã (~08:19–08:30 BRT)

### Alterações executadas e testes reais
- Estado inicial: HANDOFF já incluía smoke 12/12, C++ de combate/HUD, build M1_Development completa. Preservado esse trabalho, sem recriar componentes.
- Teste **real de input na build M1_Development**: script `Scripts/test_real_input.ps1` enviou W por 1,2 segundo, clique esquerdo e R à janela do jogo. Log gravou `SINGULARITY_FIRE Hit=StaticMeshActor_0 Ammo=29` e `SINGULARITY_RELOAD Ammo=30`; HUD exibiu mensagem. Movimento W não foi visualmente inequívoco com spawn antigo.
- Diagnóstico visual: `Reports/SINGULARITY_Packaged_Windows_M1.png` mostrava barra preta horizontal grande bloqueando vista perto do PlayerStart antigo em (0,2530,500).
- Backup do mapa feito em `Reports/Lvl_Galata_before_spawn_clearance_20261107.umap`. **Alteração cirúrgica** via `Scripts/reposition_spawn_for_visibility.py`: `PlayerStart_SINGULARITY` movido para **(300,2530,500)**, rotação (0,0,0). `Reports/spawn_visibility.json` confirmou stage saved. Nenhum FBX nem material original alterado.
- Teste gráfico direto do UE5 `-game -d3d11`: captura `Reports/spawn_x300_test.png` mostrou que a barra preta desapareceu.
- Teste de **W real após correção** via `Scripts/test_real_input_editor.ps1`: capturas `Reports/newspawn_before.png` e `Reports/newspawn_after_W.png` mostraram avanço perceptível da câmera e aproximação dos alvos. LMB e R também produziram logs de tiro/recarga. **Não afirmar acerto em alvo pelo clique:** tiro manual ainda acertou `StaticMeshActor_0`, e testes de hitscan/dano no alvo existem no smoke automatizado.
- Reexecutado `Scripts/run_smoke_test.ps1` após novo spawn: `Reports/smoke_latest.json` = **Passed 12, Failed 0, Success True**, BRT ~08:27.
- Nova build separada, sem sobrescrever anterior: `Scripts/package_spawnfix.ps1` chamou RunUAT BuildCookRun com build/cook/stage/pak/package/archive. `Reports/package_spawnfix.log` registrou **BUILD SUCCESSFUL / AutomationTool ExitCode=0**.
- **Novo executável:** `%USERPROFILE%\Documents\Unreal Projects\SINGULARITY_Builds\M1_SpawnFix\Windows\SINGULARITY.exe` (desenvolvimento, não publicável). Teste do NOVO executável `-NullRHI -SGSmokeTest -log`: **exit 0, SG_SMOKE_END Passed=12 Failed=0**.
- Nova build iniciou graficamente `-d3d11 -windowed`, janela respondendo. Captura real `Reports/SINGULARITY_SpawnFix_Packaged.png` confirma cena do Galata com três alvos e **sem barra preta**. Intel Iris Xe continua em DX11/SM5, sem Lumen.

### Próximas ações (prioridade)
1. Refinar a interação de tiro por mouse: controlar câmera/linha de visão para acertar os dummies na jogabilidade real (o disparo automático já provoca dano; clique manual atual pode bater na geometria); testar ADS e recarga de ponta a ponta.
2. Melhorar posicionamento/dimensões e geometria dos três alvos (hoje cubos altos cobrindo a rua) e criar arte de treino autoral.
3. Implementar arma 3D própria/licenciada com animação, muzzle flash Niagara, som e HUD refinado.
4. Projetar fluxo de rodada e objetivos FPS tático original, mantendo smoke 12/12 e build Windows por milestone.
5. Não distribuir Galata sem verificar direitos/licenciamento; preservar UE4/Sabotaj e todos os backups. Evitar Lumen neste hardware DX11.

### Segurança do trabalho
- Executáveis novos ficam fora do Git; somente alterações fonte e evidências pequenas em Git LFS. Sem deploy nem push remoto.

---

## CONTINUIDADE — 2026-11-07 (BRT) — correção de queda + reinício gráfico + M2 alvos

### Correção de estabilidade / M1_FallFix
- Problema confirmado em runtime: com o piso provisório antigo (escala 22,14,0.25), caminhar no eixo +Y provocava queda em T=2,33s (Z=-732,9 em T=3s).
- Backup do mapa: `Reports/Lvl_Galata_before_fall_protection_20261107.umap`.
- Script `Scripts/fix_training_floor.py` expandiu o piso para escala (135,135,0.25), em centro (1845,54;2261,46;350), com colisão QUERY_AND_PHYSICS/BlockAll, sem alterar o FBX Galata.
- Criado ator de teste C++ `SGWalkTestActor` acionado SOMENTE por `-SGWalkTest`, e integração correspondente em `SGGameMode`.
- Antes: reprodução de queda em +Y; depois: caminhada em +Y por 9s, 46,8m sem queda, Z=460,6 estável.
- Implementada recuperação de queda no `SGGameMode`: timer 0,25s, se jogador ficar abaixo Z=-500, movimento é parado e o personagem retorna ao PlayerStart. Teste `-SGWalkTest -SGForceFall` injetou Z=-1800 e comprovou `SG_FALL_RECOVERY Teleported=1` com retorno a Z=580 e estabilização após queda.
- `Scripts/run_smoke_test.ps1`: 12/12 PASS após as alterações.
- Build separada `%USERPROFILE%\Documents\Unreal Projects\SINGULARITY_Builds\M1_FallFix\Windows\SINGULARITY.exe`: UAT BUILD SUCCESSFUL/ExitCode=0.
- O script `Scripts/verify_fallfix_package.ps1` foi corrigido: parâmetro PowerShell `$Args` (reservado) substituído por `$CaseArguments`; verificação de frescor de log adicionada. Testes na build empacotada: `ForcedFall`, `WalkY`, `Smoke` = VERIFIED 3/3, códigos de saída 0 e logs correspondentes.
- A janela antiga foi fechada e a build FallFix reaberta em DX11, confirmada respondendo; imagem real `Reports/M1_FallFix_GUI_Restarted.png`.

### Próxima etapa concreta M2: alvos de treinamento
- Backups dos fontes originais em `Reports/SGTargetDummy_before_silhouette_20261107.h/.cpp`.
- `SGTargetDummy.h/.cpp`: atores de treino mudaram de um bloco gigante para silhueta geométrica modular, com torso, cabeça, braços e suporte. Utilizam apenas Cube/Sphere do próprio Unreal; material já aplicado à cena é reaproveitado. Dano e eliminação continuam no ator; colisões são desligadas ao destruir.
- Build editor compilada com `Result: Succeeded`; smoke do projeto 12/12; posteriormente nova build distribuível de desenvolvimento `%USERPROFILE%\Documents\Unreal Projects\SINGULARITY_Builds\M2_TargetSilhouettes\Windows\SINGULARITY.exe` empacotada via `Scripts/package_target_silhouettes.ps1` (BUILD SUCCESSFUL, ExitCode=0).
- Teste no executável M2: `SG_SMOKE_END Passed=12 Failed=0`, dano 34+34+32, `SINGULARITY_TARGET_DESTROYED`, processo encerrou em código 0.
- Janela M1_FallFix fechada e janela M2_TargetSilhouettes aberta em DX11 1280×720; processo gráfico respondendo. Captura real: `Reports/M2_TargetSilhouettes_GUI.png`.
[Trecho com possível informação sensível omitido da versão pública.]
- **NÃO** é Lumen: hardware Intel Iris Xe em D3D11/SM5. As builds são apenas de desenvolvimento local, sem permissão presumida para redistribuir assets recuperados.

### Próximos passos prioritários
1. Aperfeiçoar formato dos alvos e colocação precisa na rua com aproximação por screenshot, removendo aparência de peças flutuantes.
2. Testar manualmente linha de mira/hitscan nos alvos em vez de atingir malha do Galata; oferecer hit feedback original no HUD.
3. Implementar arma 3D própria ou licenciada, efeitos simples, áudio e recoil/ADS gradualmente com testes de regressão.
4. Continuar testando estabilidade, desempenho DX11 e gameplay com builds separadas por milestone; não sobrescrever o UE4/Sabotaj.

---

## EXECUÇÃO — 2026-11-07, M2: protótipo de arma SG-01 + confirmação de acerto

### Mudanças reais e verificadas
- Lido HANDOFF antes de alterar. Não alterados Galata original / FBX / atlas / projeto Sabotaj UE4 nem arquivos do Valorant.
- Criados **dois materiais originais** via `Scripts/create_sg01_materials.py`: `/Game/Singularity/Weapons/M_SG01_Graphite` (grafite/metal) e `M_SG01_Cyan` (detalhes ciano). Relatório `Reports/sg01_materials.json`, `stage=saved`.
- Nova classe C++ `SGFirstPersonWeapon.h/.cpp` que monta **SG-01 (protótipo geométrico, não arte final)** usando componentes Cube/Cylinder do próprio UE; peças: receptor, coronha, guarda-mão, cano, carregador, empunhadura, trilho, miras, detalhe e boca do cano. Desabilitada colisão/sombras para não afetar o hitscan. Segue a câmera FPS (ViewOffset final (96,30,-31)) e recebe `NotifyShot()` para recuo cosmético de rápida recuperação.
- `SGGameMode.cpp`: nova SG-01 criada automaticamente em runtime normal; não aparece nos testes `-SGSmokeTest/-SGWalkTest`. Nenhuma duplicação do sistema de combate.
- `SGCombatSystem.h/.cpp`: após tiro, envia evento `NotifyShot()` ao primeiro viewmodel; confirmação de dano real a `SGTargetDummy` compara HP antes/depois e registra `SG_HIT_CONFIRM Target=... RemainingHP=...`; getter `HasRecentConfirmedTargetHit()` ativo por 0,24 segundo.
- `SGHUD.cpp`: quando hit confirmado, exibe brevemente marcas e texto cianos `HIT` no centro; rodapé identifica `SG-01`.
- `SGSmokeTestActor.cpp`: teste adicional `HITMARKER_CONFIRMED`. **13/13 PASS** (sem falhas) via `Scripts/run_smoke_test.ps1`, depois de ambos ajustes de arma.
- Durante a primeira compilação houve incompatibilidade C2445 de `TObjectPtr` na expressão ternária; corrigida com `.Get()`. Compilação posterior `Result: Succeeded`, sem erros.
- Capturas reais UE5 em `Reports/SG01_UE5_Viewmodel_preview.png` e `Reports/SG01_UE5_Viewmodel_v2.png`. Versão v2 foi afastada/abaixada e miras reduzidas para liberar o centro da tela.
- Empacotamento completo com `Scripts/package_sg01.ps1`: `BUILD SUCCESSFUL` / AutomationTool ExitCode=0. Novo executável em `%USERPROFILE%\Documents\Unreal Projects\SINGULARITY_Builds\M2_SG01_Prototype\Windows\SINGULARITY.exe`. Versões anteriores M1/M2 preservadas.
- Validação **no executável empacotado** com `-NullRHI -SGSmokeTest`: **13 aprovados / 0 falhas, exit code 0** e log `SG_HIT_CONFIRM` confirmado.
- Jogo empacotado `M2_SG01_Prototype` aberto no Windows em DX11 1280x720; processo gráfico `SINGULARITY (64-bit Development PCD3D_SM5)` respondendo, log `SG01_VIEWMODEL_SPAWNED` e `SG01_VIEWMODEL_READY`. Captura real `Reports/SG01_Packaged_Windows_M2.png`.
- Criado script reutilizável de verificação `Scripts/verify_sg01_package.ps1`, que exige fechar o jogo antes de executar para proteger logs compartilhados. Resultado 13/13 da build já verificado por chamada equivalente anterior.

### Limitações (não confundir com jogo final)
- SG-01 é apenas protótipo estático montado com geometrias primitivas; sem rig/skeletal mesh, animação de braços, som, muzzle flash, VFX de impacto, ADS, recarga animada, visão otimizada para multiplayer ou servidores autoritativos.
- Recuo é cosmético no viewmodel, sem propagação para trajetória do disparo. Hitmarker confirma apenas dano real em SGTargetDummy.
- Artistas devem substituir peças por mesh autoral ou licenciado. Captura DX11/SM5 em Intel Iris Xe, **não Lumen**.
- Geometria recuperada Galata continua somente para prototipagem interna até licenciamento claro.

### Prioridade exata da próxima execução
1. Validar input gráfico LMB/R e hitmarker real com retículo corretamente alinhado ao torso/cabeça de um dummy, sem tiro atravessar paredes.
2. Refinar alvo: torso/cabeça/braços mais proporcionais, materiais mais contrastantes e menor aparência de peças flutuantes.
3. Melhorar SG-01: ADS alinhado ao retículo, recoil real/equilíbrio, animação de recarga, áudio e efeito visual de disparo original, preservando as 13 verificações.
4. Repetir smoke, gerar build própria a cada marco e armazenar evidências; preservar backups/Git local, sem push/deploy por padrão.

---

## 2026-11-07 — M2: mira ADS da SG-01 (RMB / FOV suave) — execução ChatGPT

### Implementação
- Criado backup dos seis arquivos C++ modificados em `Reports/Backups/ADS_PreChange_20261107/`.
- `Source/SINGULARITY/SGCombatSystem.h/.cpp`: `StartAim()`, `StopAim()`, `IsAiming()`; entrada **botão direito pressionado** e saída **botão direito solto** usando `BindKey`. Log `SG_ADS_STATE ENTER/EXIT` acionado somente nas transições; tiro LMB e recarga R preservados.
- `SGFirstPersonWeapon.h/.cpp`: `ADSOffset=(88,0,-13)`, `AimedFOV=65`. `ADSAlpha` faz interpolação `FInterpTo` com velocidade 12 para transição suave da arma entre hipfire `ViewOffset=(96,30,-31)` e centralização ADS. `APlayerCameraManager::SetFOV` aproxima a câmera durante a transição, `UnlockFOV` devolve FOV original ao sair, inclusive no `EndPlay`. A mira é *hold-to-aim*, sem alterar a trajetória hitscan do teste atual.
- `SGHUD.cpp`: dica `RMB: AIM` e pequeno rótulo ciano `ADS` quando mirando.
- `SGSmokeTestActor.cpp`: 2 testes extras `ADS_ENTER` e `ADS_EXIT`. Suíte aumentada de 13 para **15 testes**.
- Compilação `Build.bat SINGULARITYEditor Win64 Development`: **Succeeded**; `Scripts/run_smoke_test.ps1`: **Passed=15 Failed=0**.
- Teste gráfico com clique direito real na janela do UE5, via `Scripts/test_ads_real_input.ps1`: `ADS_REAL_INPUT_PASS`. Capturas `Reports/ADS_HipFire_UE5.png`, `Reports/ADS_Aiming_UE5.png`, `Reports/ADS_Released_UE5.png`. Conferido visualmente que FOV aproxima, arma centraliza e visão retorna ao soltar.

### Build Windows e validação
- Novo script `Scripts/package_sg01_ads.ps1` empacotou separadamente `%USERPROFILE%\Documents\Unreal Projects\SINGULARITY_Builds\M2_SG01_ADS\Windows\SINGULARITY.exe`. UAT **BUILD SUCCESSFUL**, **ExitCode=0**, cooking e staging concluídos. Preservadas builds anteriores.
- `Scripts/verify_sg01_ads_package.ps1`: teste no executável empacotado em NullRHI, código de saída 0, `SG_SMOKE_END Passed=15 Failed=0`, marcadores `ADS_ENTER/ADS_EXIT/SG_HIT_CONFIRM` encontrados.
- Janela gráfica da nova build Windows aberta em `-d3d11 -windowed -ResX=1280 -ResY=720`, respondendo. `Scripts/test_ads_packaged_input.ps1` envia botão direito real e captura `Reports/ADS_HipFire_Packaged.png`, `Reports/ADS_Aiming_Packaged.png`, `Reports/ADS_Released_Packaged.png`. Log da **própria build** confirmou `SG_ADS_STATE ENTER` e `SG_ADS_STATE EXIT` após essa interação. Caminho do log: `SINGULARITY_Builds/M2_SG01_ADS/Windows/SINGULARITY/Saved/Logs/SINGULARITY.log`.
- Nota: script de input empacotado foi corrigido para apontar ao log da nova build, não ao Saved/Logs do editor. Capturas da versão empacotada foram visualmente validadas: aumento de zoom ao mirar e retorno ao soltar.

### Limites e continuidade
- ADS e posição da arma são **protótipo local**, usando viewmodel estático e FOV; não inclui animação esquelética, sensibilidade específica ADS, sway, áudio, impacto visual, ADS server-authoritative ou balanceamento. A geometria da mira ainda cobre parte do centro, passível de polimento.
- Intel Iris Xe em **D3D11/SM5**, não Lumen. Galata recuperado é só para protótipo local até esclarecer direitos. Nenhum deploy, publicação ou push remoto.
- Próximo incremento sugerido: feedback de disparo e recarga com áudio/VFX originais, retículo mais refinado, alvos visualmente proporcionais, testes de mira e hitscan em movimento e validação de regressão.

---

## 2026-11-07 — M2: feedback da SG-01, efeitos originais e recarga temporizada

### Implementado e validado nesta execução
- Lido o HANDOFF original e inspecionado o código/estado Git. Backup dos seis fontes anteriores em `Reports/Backups/FX_PreChange_20261107`. Builds anteriores preservadas.
- Criado `Scripts/generate_sg01_audio.ps1`, gerador determinístico de dois efeitos WAV mono 22050 Hz / 16-bit **originais**: `SourceAssets/Audio/SG01_Fire_Original.wav` (0,22s) e `SG01_Reload_Original.wav` (0,55s); sem áudio copiado de jogos ou bibliotecas externas. `Scripts/import_sg01_audio.py` importou ambos como `SoundWave` em `Content/Singularity/Audio/S_SG01_Fire.uasset` e `S_SG01_Reload.uasset`, relatório `Reports/sg01_audio_import.json` com stage=saved.
- `SGFirstPersonWeapon.h/.cpp`: som local `UGameplayStatics::PlaySound2D` na SG-01 em tiro e recarga, com volumes baixos; clarão temporário ciano na boca do cano usando esfera do Engine/material próprio existente, e uma PointLight de curta duração (0,08s), sem sombras. Ativa `NotifyShot()`, log `SG01_FIRE_FEEDBACK Flash=1 Sound=1`, e oculta efeitos após o tempo. `NotifyReload()` anima modelo de arma e carregador com transição seno por 1,25s, recuando e inclinando a SG-01. Logs de início/fim de animação.
- `SGCombatSystem.h/.cpp`: recarga agora leva **1,25 segundo** (timer) e não restaura imediatamente as balas; `bReloading`, `IsReloading`, `CompleteReload()`. Bloqueia tiros durante recarga sem consumir munição, retorna 30 ao fim, cancela ADS durante recarga. Notifica visual/som da SG-01. A tecla R permanece. `SGHUD.cpp` mostra `RELOADING...` e contagem correta.
- `SGSmokeTestActor.h/.cpp`: testes ampliados de 15 para **23**. Valida recursos: instancia viewmodel em modo de teste, áudio original carregado, muzzle flash iniciado, bloqueio de tiro durante recarga, animação iniciada/concluída, munição restaurada no tempo correto. `Scripts/run_smoke_test.ps1` **23/23 PASS** com exit code 0 e `Build.bat SINGULARITYEditor` **Succeeded**.
- Empacotamento separado `Scripts/package_sg01_fx.ps1`: **BUILD SUCCESSFUL / AutomationTool ExitCode 0**, nova build `%USERPROFILE%\Documents\Unreal Projects\SINGULARITY_Builds\M2_SG01_FX\Windows\SINGULARITY.exe`; anteriormente houve mensagem transitória de ZenServer mas o processo recuperou e terminou sem falha.
- Teste no EXE final `Scripts/verify_sg01_fx_package.ps1`: `SG01_FX_PACKAGED_VALIDATED Passed=23 Failed=0 AudioAssets=2 VFX=1 Reload=1`, com marcadores dos efeitos e dos bloqueios. Log copiado `Reports/SG01_FX_Packaged_Smoke.log`.
- Jogo gráfico FX iniciado `-d3d11 -windowed -ResX=1280 -ResY=720`. Teste real de mouse LMB + teclado R: `Scripts/test_sg01_fx_real_input.ps1` `SG01_FX_REAL_INPUT_PASS`, logs da build marcaram `SG01_FIRE_FEEDBACK Flash=1 Sound=1`, `SG_RELOAD_START Ammo=29`, `SG01_RELOAD_ANIMATION_BEGIN Sound=1`, `SG01_RELOAD_ANIMATION_END`, `SINGULARITY_RELOAD Ammo=30`. Prints reais `Reports/SG01_FX_Before.png`, `SG01_FX_After_Shot.png`, `SG01_FX_Reload_Mid.png`, `SG01_FX_Reload_Done.png`. Captura `Reload_Mid` mostrou arma baixada e HUD RELOADING. Flash dura 0,08s, logo screenshot não garante captura da luz no frame exato; a ativação foi confirmada em testes/logs, não em inspeção quadro a quadro.
- Inicialmente o teste de input gráfico falhou porque o processo ChatGPT manteve o foco (SetForegroundWindow retornou false). `Scripts/focus_sg_game.ps1` usa AttachThreadInput ao foreground; depois logou FOREGROUND=True e FG_PID_FINAL igual ao processo do jogo; **somente depois** o teste de input foi considerado aprovado.
- Não modificado o Galata FBX nem Sabotaj UE4; sem push/deploy. O jogo continua um protótipo local, sem multiplayer, sem Niagara avançado, sem rig esquelético e sem Lumen na Intel Iris Xe DX11.

### Próxima ação
1. Refinar disparo: efeitos de impacto em superfícies com asset original, melhorar sincronização sonora/flash e comportamento em ADS; testar com clique real mirando alvos (manual anteriormente podia acertar a geometria de Galata).
2. Aprimorar SG-01 visual (mesh original mais detalhada, animações de recarga por peças e mãos/rig), sem quebrar o código de arma existente.
3. Revisar design dos 3 alvos para proporções/posicionamento, e implementar um pequeno fluxo de treino/partida com objetivos e feedback, em milestones separados.
4. Preservar 23 testes, registrar screenshots e build separada em cada marco e checar direitos antes de publicação.

---

## 2026-11-07 — M2: impactos originais + teste ADS após movimentação

### Alterações entregues
- Estado anterior verificado: `f2879ed` com `SG-01 FX` e 23 testes. Backup dos arquivos anteriores a esta mudança criado em `Reports/Backups/Impact_PreChange_20261107`. Projeto Sabotaj, FBX e atlas originais preservados.
- Criada classe C++ `SGImpactFX.h/.cpp`: efeito visual efêmero de impacto, feito inteiramente com esfera do Engine, material próprio ciano da SG-01 e PointLight sem sombras. O ator nasce em `Hit.ImpactPoint + Hit.ImpactNormal * 6`, colide com nada, decai por 0,32 segundo, destrói-se sozinho via lifespan. `InitializeImpact` distingue `Target` e `Surface`, registra `SG_IMPACT_FX Kind=...`.
- `SGCombatSystem.cpp`: depois do hitscan `LineTraceSingleByChannel(ECC_Visibility)`, cria efeito apenas onde há impacto; nenhuma mudança na origem da mira, no bloqueio por superfícies ou no cálculo de dano. O hitmarker anterior só confirma dano real a SGTargetDummy.
- `SGSmokeTestActor.cpp`: adicionado teste para impacto criado, classificado como alvo e sem colisão. Depois de movimentar o jogador, cria dummy temporário a 220 cm da câmera SOMENTE no modo `-SGSmokeTest`; reorienta o controle, entra em ADS e dispara de novo, validando dano, consumo de munição e hitmarker **após movimentação**. Não há aim assist incorporado à jogabilidade normal.
- Compilação `Build.bat SINGULARITYEditor Win64 Development` concluída com `Result: Succeeded`. `Scripts/run_smoke_test.ps1`: **30/30 PASS**, 0 falhas.

### Build e teste de interface
- `Scripts/package_impact_m2.ps1`: pacote novo, separado e sem sobrescrever: `%USERPROFILE%\Documents\Unreal Projects\SINGULARITY_Builds\M2_ImpactFX\Windows\SINGULARITY.exe`. UAT encerrou `BUILD SUCCESSFUL`, `AutomationTool ExitCode=0`.
- `Scripts/verify_impact_package.ps1`: teste headless no executável empacotado: **30/30 PASS**, `IMPACT_PACKAGE_VALIDATED Passed=30 Failed=0 Impact=1 ADSAfterMove=1`. Log de evidência `Reports/M2_ImpactFX_Packaged_Smoke.log`.
- Nova janela gráfica `M2_ImpactFX` lançada em DX11, 1280×720, respondendo. `Scripts/test_impact_real_input.ps1` envia clique esquerdo + R por janela com foco e capturas `Reports/Impact_Before.png`, `Impact_After_Shot.png`, `Impact_Reload_Mid.png`, `Impact_Reload_Done.png`. O primeiro teste teve bug no script (`Select-String -Path` sem valor), **corrigido**; repetição passou `SG_IMPACT_REAL_INPUT_PASS` com eventos novos de disparo e recarga e `SG_IMPACT_FX Kind=Surface` no log. Processo gráfico continua aberto.
- Limites: os testes ADS após deslocamento usam alvo de teste criado a curta distância e rotação programática, não provam precisão manual do mouse contra todos os três alvos originais; disparos manuais registrados atingiram superfícies do Galata. O impacto de 0,32 s foi confirmado funcionalmente por testes/logs; não afirmar captura nítida do clarão em um único frame. Apenas protótipo DX11/SM5 na Intel Iris Xe, sem Niagara avançado e sem Lumen.

### Próxima prioridade
1. Ajustar os três alvos originais na cena para estarem totalmente visíveis e acessíveis sem acertar primeiro os prédios/props; preservar backup do mapa antes de mudar posições.
2. Refinar aparência e proporções dos alvos, distinguindo cabeça e torso; depois testar mouse real e ADS contra alvos a diferentes distâncias.
3. Melhorar impacto com material emissivo e feedback proporcional, eventualmente Niagara e áudio ambiental autoral; manter revisão de direitos de assets antes da publicação.

---

## 2026-11-07 — M2 StreetTargets: validação e empacotamento

### Estado da árvore de trabalho encontrado
- Ao iniciar esta execução, arquivos do mapa e `SGTargetDummy.h/.cpp` já tinham alterações não commitadas, inclusive três silhuetas de treinamento, membros, marcação ciano no peito, material original `/Game/Singularity/Targets/M_SG_Training_Mark`, scripts e relatórios de alinhamento. **Foram preservados**, sem recriação nem sobrescrita; as alterações existentes foram inspecionadas.
- Coordenadas finais verificadas em `Reports/street_alignment.json`: spawn (300,2530,-80), alvos central (850,2530,-105), esquerda (1050,2810,-105), direita (1050,2250,-105) e piso (1845.5,2261.5,-230). Os três alvos são visíveis na rua.
- Backup do mapa da versão atual criado em `Reports/Lvl_Galata_before_M2_target_validation_20261107.umap`, sem alterar os arquivos Galata originais.

### Resultados desta execução
- `Scripts/run_smoke_test.ps1`, executado **depois da última alteração conhecida do mapa**, retornou `SG_SMOKE_SUMMARY Passed=35 Failed=0 Success=True`, exit code 0. Inclui verificações dos 3 alvos, linha de visão, spawn em nível da rua, pernas, marcação no peito, dano, ADS e mobilidade.
- Captura real da cena atual via `Scripts/capture_target_validated.ps1`: `Reports/M2_Targets_ValidatedPreview.png`. Mostra os três alvos sobre a rua, corpos escuros e marca ciano; SG-01 na tela.
- **Disparo manual real aprovado no editor** via `Scripts/test_target_center_real_input.ps1`: segurou RMB/ADS, disparou LMB na direção do alvo central, e o log confirmou `SINGULARITY_TARGET_HIT SGTargetDummy_0 Damage=34.0 Health=66.0`, `SG_HIT_CONFIRM`, munição 30→29 e `CENTER_ADS_REAL_CLICK_PASS`. Isso demonstrou tiro real contra alvo original, não apenas alvo temporário.
- Novo pacote separado `Scripts/package_street_targets_m2.ps1`: `BUILD SUCCESSFUL` e `AutomationTool ExitCode=0`. Executável `%USERPROFILE%\Documents\Unreal Projects\SINGULARITY_Builds\M2_StreetTargets\Windows\SINGULARITY.exe`.
- Validação no executável final com `Scripts/verify_street_targets_package.ps1`: `M2_STREET_TARGETS_PACKAGE_VALIDATED Passed=35 Failed=0 ThreeTargetsVisible=True`. Código 0 e verificações individuais. Log em `Reports/M2_StreetTargets_Packaged_Smoke.log`.
- Build gráfica empacotada iniciada com DX11 1280x720, respondendo, captura real `Reports/M2_Targets_PackagedPreview.png`.
- `Scripts/test_target_center_packaged.ps1` foi tentado, mas **NÃO PASSOU** por foco da janela. O foreground permaneceu em PID 6516 e `SetForegroundWindow` retornou false. Nem RMB nem clique foram registrados no log neste teste. `Scripts/focus_target_packaged.ps1` confirmou o problema de foco. **Não usar isto como evidência de erro na arma nem alegar teste manual no executável aprovado.**
- As builds antigas permanecem preservadas. Sem push remoto/deploy. Sem modificação do projeto Sabotaj original.

### Próximos passos
1. Testar ADS/disparo manual na versão empacotada quando a janela puder receber foco, observando dano e acerto real nos três alvos em distâncias diferentes.
2. Revisar o mapa para eventual tiro contra geometria, sem deixar o Galata penetrável; não acrescentar aim assist automático.
3. Otimizar silhuetas e materiais sob Intel Iris Xe/DX11, corrigir saturação de luz e refinar modelo da SG-01; antes de publicar, verificar direitos/licenciamento do Galata.

---

## 2026-11-07 — M2 TrainingChallenge: treino pontuado de 90 segundos

### Implementado no PC (Remote Desktop Commander)
- Antes de alterar, os 8 arquivos C++ modificados foram copiados para `Reports/Backups/Training_PreChange_20261107/`; estados originais de Galata, projeto Sabotaj UE4 e builds M1/M2 anteriores preservados.
- `SGGameMode`: desafio local de 90 segundos, registro apenas dos três `SGTargetDummy` já colocados na rua, +25 pontos por dano efetivo e bônus de +100 por eliminação, contagem de 0/3, encerramento ao eliminar todos ou esgotar o tempo, tempo congelado após conclusão e reinício da rodada.
- `SGTargetDummy`: informa dano aplicado ao GameMode e oferece `ResetTarget()` para recuperar HP, visibilidade e colisões após eliminação.
- `SGCombatSystem`: tecla **T** para reiniciar pontuação, tempo, três alvos e carregador de 30; tiro LMB, ADS RMB, recarga R, hitmarker e FX anteriores preservados.
- `SGHUD`: painel superior direito com **TRAINING / GALATA**, score, alvos, relógio mm:ss e mensagem de vitória/tempo esgotado. Rodapé agora inclui **T: RETRY**.

### Testes e evidências reais
- `Build.bat SINGULARITYEditor Win64 Development -WaitMutex -NoHotReload`: **Result: Succeeded**, relatório `Reports/M2_Training_Build.log`.
- `Scripts/run_smoke_test.ps1`: **44/44 PASS, 0 falhas, exit 0**; inclui regressões dos 35 anteriores mais sessão, pontuação, bônus, eliminação, reset HP/munição/pontuação/cronômetro. Evidências `Reports/smoke_latest.json` e `smoke_latest.log`.
- `Scripts/package_training_m2.ps1`: **BUILD SUCCESSFUL / ExitCode=0**; nova build separada `%USERPROFILE%\Documents\Unreal Projects\SINGULARITY_Builds\M2_TrainingChallenge\Windows\SINGULARITY.exe`. Log `Reports/M2_Training_Package.log`.
- `Scripts/verify_training_package.ps1`: executável empacotado **44/44 PASS**, `Exit=0`, 10 marcadores de treino presentes. Log copiado em `Reports/M2_Training_Packaged_Smoke.log`.
- Janela antiga M2_StreetTargets fechada normalmente, sem encerramento forçado; build gráfica `M2_TrainingChallenge` iniciada em **DX11/SM5 1280×720**, janela respondendo. Screenshot verdadeiro via `Scripts/capture_training_m2.ps1`: `Reports/M2_Training_PackagedPreview.png` (PrintWindow=True), mostra painel score/targets/timer, arma SG-01 e três alvos visíveis.
- Teste real da tecla **T**: `Scripts/test_training_real_input.ps1` verificou primeiro o foreground e **NÃO enviou a tecla** porque PID foreground 17684 não era o do jogo. O método de reinício foi testado com 44/44 no smoke, mas **input real T ainda NÃO está validado**. Não declarar aprovação do input real.
- Nenhuma publicação/push/deploy. Assets Galata recuperados permanecem só para protótipo local até conferir licenças/direitos. Hardware Iris Xe DX11/SM5 **não comprova Lumen**.

### Próximo passo exato
1. Validar **T** no executável com janela realmente focada; validar vitória 3/3 e fim do tempo também em fluxo visual, além do smoke.
2. Refinar geometria e materiais da **SG-01** para silhueta de arma original mais convincente e melhorar sol/exposição do Galata (a captura atual tem regiões estouradas), sem quebrar 44 testes.
3. Adicionar feedback da rodada (sons originais de conclusão e transição de sessão), depois avançar para objetivos, bots e lógica tática. Continuar usando backups, build por milestone e testes.

---

## 2026-11-07 — M2 SG-01 Mk II + Galata exposure tuning (Remote Desktop Commander)

### Estado anterior e medidas de segurança
- Confirmado checkpoint local anterior `3282105` (M2 TrainingChallenge, 44/44 testes). Antes de modificar, salvas cópias dos fontes da arma, teste e mapa em `Reports/Backups/SG01_Visual_PreChange_20261107/`.
- Todos os arquivos originais do Sabotaj/UE4, texturas e FBX Galata, materiais importados, builds M1/M2 anteriores foram preservados. Nenhum push remoto/deploy/publicação.

### Execução real
1. **Controle T pendente resolvido:** `Scripts/test_training_real_input.ps1` alcançou o foreground correto da janela de `M2_TrainingChallenge`, enviou a tecla T e registrou `SG_TRAINING_REAL_T_PASS ForegroundPID=14840 RestartsDelta=1`.
2. **SG-01 Mk II:** `SGFirstPersonWeapon.cpp/h` remodelado com componentes originais do Engine (coronha, buffer, cheek rest, grip, gatilho, carregador com base, cano com sleeve, muzzle brake, trilho denteado, miras e holo optic baixo, linhas ciano discretas). A muzzle flash acompanhou a nova ponta do cano. Continua protótipo low-poly sem mesh humano/arma de terceiros. Todos os componentes cosméticos não colidem com jogador/tiro.
3. **Testes novos:** `SGSmokeTestActor.cpp` agora valida mínimo de 30 partes do viewmodel e ausência de colisão em todas. Suíte subiu de 44 para **46/46 PASS** via `Scripts/run_smoke_test.ps1`, exit code 0. Compilação `SINGULARITYEditor Win64 Development` concluída **Result: Succeeded** em `Reports/SG01_MkII_Build.log`.
4. **Iluminação reversível:** `Scripts/sg_m2_exposure_v1.py` modificou SOMENTE `/Game/Maps/Lvl_Galata`, adicionando volume global `SG_ExposureControl_M2` com exposure bias -0.75 e bloom 0.18; sol 2.2→1.9, Skylight 1.4→1.25. Mapa salvo, `Reports/sg_m2_exposure_report.json` mostra status=saved; commandlet terminou com ExitCode=0 e 0 erros, com 3 avisos de API depreciada. Backup do mapa pré-ajuste preservado.
5. **Nova build independente:** `Scripts/package_sg01_mk2.ps1` concluiu `BUILD SUCCESSFUL / ExitCode=0` e arquivou `%USERPROFILE%\Documents\Unreal Projects\SINGULARITY_Builds\M2_SG01_MkII\Windows\SINGULARITY.exe`; builds anteriores não sobrescritas. `Scripts/verify_sg01_mk2.ps1` testou o EXE em NullRHI: **46/46 PASS**, ExitCode=0, seis marcadores necessários; evidência `Reports/SG01_MkII_Packaged_Smoke.log`.
6. **Teste gráfico real DX11/SM5:** janela anterior fechada normalmente via CloseMainWindow, nova build abriu em 1280×720, janela responsiva. `Scripts/capture_sg01_mk2.ps1` gerou imagem real `Reports/SG01_MkII_PackagedPreview.png` (PrintWindow=True, 1.573.827 bytes). Vista verificada: SG-01 mais articulada, painel TRAINING visível, três alvos acessíveis, fachadas menos superexpostas; ainda existem destaques fortes e sombras duras, não é acabamento final nem Lumen.
7. **Input real nova build:** `Scripts/test_sg01_mk2_restart_input.ps1` confirmou **SG01_MKII_REAL_T_PASS**, ForegroundPID=11836, RestartsDelta=1. `Scripts/test_sg01_mk2_ads_target.ps1` exigiu PID foreground correto antes de enviar mouse RMB/LMB; log da própria build confirmou `SG_ADS_STATE ENTER`, `SINGULARITY_TARGET_HIT SGTargetDummy_0 Damage=34.0 Health=66.0`, `SG_HIT_CONFIRM`, munição 29 e `SG_ADS_STATE EXIT`; resultado **SG01_MKII_ADS_TARGET_HIT_PASS**.

### Continuação prioritária
- Revisar proporcionalidade da arma Mk II em ADS e hipfire; buscar mesh autoral/licenciado com rig e animações de braços/mãos, substituindo peças primitivas progressivamente, sem quebrar colisões e 46 testes.
- Refinar materiais da SG-01 e controlar melhor áreas externas ainda claras / sombras duras no Galata, com comparações em DX11. Lumen DX12/SM6 permanece NÃO comprovado na Intel Iris Xe.
- Continuar fluxo do treino: validação visual de vitória 3/3 e timeout, sons/feedback originais, melhor alvo, regras de rodada e objetivo tático. Sem publicar assets recuperados antes de auditar direitos/licenças.

---

## 2026-11-07 — Mudança de prioridade: aproveitamento controlado do acervo Sabotaj (início)
- Após feedback do criador sobre o ritmo, a prioridade mudou para inventariar e validar armas, modelos, animações e texturas já disponíveis, em vez de refazer tudo com geometria provisória. O build jogável SG-01 MkII (`b988d95`) continua preservado.
- Em `%USERPROFILE%\Tools\_projetos\Sabotaj\export`, inventário inicial constatou **325 FBX, 317 GLB e 13.451 PNG** entre mais formatos. Referências concretas: `FBX_UE/Sabotaj/Content/Weapons_V2/Base/Machines/AK74/Mesh/SK_AK74_v03.fbx`, modelo M4A1 e `Models/Sabotaj/Content/Weapons/Base/Machines/AK74/Textures/T_AK74_D.png`, `T_AK74_N.png`, `T_AK74_ORM.png`. O HANDOFF do Sabotaj menciona animações FPP e 79 skins, mas essa lista ainda exige revisão individual para compatibilidade/atribuição.
- **Cópia isolada e não destrutiva** dos quatro arquivos AK74 para `SourceAssets/ReferencesOnly/Sabotaj/AK74/`. Nenhum arquivo original alterado.
- **Importação REAL no Unreal Engine 5.8.4:** script `Scripts/import_ak74_reference_ue5.py` rodou via `UnrealEditor-Cmd.exe -run=pythonscript`; `Reports/ak74_reference_import.json` registrou `stage=done`; processo terminou com **ExitCode=0, zero erros, zero avisos**. Foram salvos 7 `.uasset` em `/Game/PrototypeReferences/Sabotaj/AK74/`: `SK_AK74_v03`, 2 recursos de skeleton, `Icosphere`, e as texturas `T_AK74_D`, `T_AK74_N`, `T_AK74_ORM`.
- **Atenção:** apenas importação como recurso de referência, NÃO foi feito ainda material PBR definitivo, ajuste de escala/rig, animações, incorporação à câmera do personagem, nem prova de arma jogável AK74. Não substituir SG-01 nem declarar a AK74 funcional no FPS.
- Como os direitos de publicação/distribuição dessas fontes recuperadas não estão confirmados, recursos de `SourceAssets/ReferencesOnly/` e `Content/PrototypeReferences/` foram excluídos do Git por `.gitignore`; uso apenas de referência/testes locais. Não empacotar/publicar builds contendo esses assets sem auditoria/autorização.
- **PRÓXIMA AÇÃO:** inspecionar os 7 assets no UE5, verificar a qualidade do skeletal mesh/UV/material, conectar as texturas em um material PBR de visualização, renderizar arma (e comparação de escala), inventariar skins/animações compatíveis e preparar implementação legal/autoral/licenciada no FPS com teste visível, sem alterar o Sabotaj original.